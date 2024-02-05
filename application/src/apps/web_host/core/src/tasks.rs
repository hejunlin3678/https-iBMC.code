use super::utils::{lock_state_call, with_state};
use anyhow::anyhow;
use bytes::Bytes;
use libc::c_int;
use lua::{luaL_Reg, lua_State, wrap_lua_func, Integer, State, Type, LUA_REGISTRYINDEX};
use std::collections::HashMap;
use std::sync::{Arc, Mutex, Weak};
use std::time::Duration;
use tokio::sync::{mpsc, oneshot};
use tokio::time::sleep;

scoped_thread_local!(static CURRENT: Task);

#[derive(Debug)]
struct Message {
  session: i32,
  data: Option<Bytes>,
}

impl Message {
  fn new(session: i32, data: Option<Bytes>) -> Self {
    Message {
      session: session,
      data: data,
    }
  }
}

struct TaskData {
  name: Option<String>,                // 任务名称
  sessionID: i32,                      // session ID
  quit: Option<oneshot::Sender<bool>>, // 退出信号
}

#[derive(Clone)]
pub(super) struct Task {
  taskID: i32,                            // 任务 ID
  state: Weak<Mutex<State>>,              // 任务所在的 lua 虚拟机
  taskRef: c_int,                         // 任务表的引用
  sender: mpsc::UnboundedSender<Message>, // 消息队列
  data: Arc<Mutex<TaskData>>,             // 任务数据
}

struct Tasks {
  nextTaskID: i32,
  namedIds: HashMap<String, i32>, // <Task名字, taskID>
  tasks: HashMap<i32, Task>,
}

impl Tasks {
  fn new() -> Self {
    Tasks {
      nextTaskID: 1,
      namedIds: HashMap::new(),
      tasks: HashMap::new(),
    }
  }
}

lazy_static! {
  static ref TASKS: Arc<Mutex<Tasks>> = Arc::new(Mutex::new(Tasks::new()));
}

impl Tasks {
  fn newTaskID(&mut self) -> i32 {
    let mut taskID = self.nextTaskID;
    loop {
      if !self.tasks.contains_key(&taskID) {
        self.nextTaskID = taskID + 1;
        if self.nextTaskID <= 0 {
          self.nextTaskID = 1;
        }
        return taskID;
      }
      taskID += 1;
      if taskID <= 0 {
        taskID = 1;
      }
    }
  }

  fn removeTask(&mut self, taskID: i32) {
    if let Some(task) = self.tasks.remove(&taskID) {
      let data = task.data.lock().unwrap();
      if let Some(ref name) = data.name {
        self.namedIds.remove(name);
      }
    }
    debug!("remove task {}", taskID);
  }
}

impl Task {
  fn start(task: Task, mut rx: mpsc::UnboundedReceiver<Message>, mut quit: oneshot::Receiver<bool>) {
    tokio::spawn(async move {
      debug!("start task {} loop", task.taskID);
      loop {
        tokio::select! {
          Some(msg) = rx.recv() => CURRENT.set(&task, || Task::runTaskCB(&task.state, task.taskRef, msg)),
          _ = &mut quit => break,
          else => break,
        }
      }
      rx.close();
      if let Some(s) = task.state.upgrade() {
        let mut state = s.lock().unwrap();
        state.unreference(LUA_REGISTRYINDEX, task.taskRef);
      }
      let mut tasks = TASKS.lock().unwrap();
      tasks.removeTask(task.taskID);
      debug!("exit task {} loop", task.taskID);
    });
  }

  fn runTaskCB(state: &Weak<Mutex<State>>, taskRef: c_int, msg: Message) {
    if let Some(s) = state.upgrade() {
      let _ = lock_state_call(&s, |state| {
        state.raw_geti(LUA_REGISTRYINDEX, taskRef as Integer);
        state.push_integer(msg.session as Integer);
        if let Some(data) = &msg.data {
          state.push_bytes(data)?;
        } else {
          state.push_nil();
        }
        if let Err(e) = state.pcall(2, 1, 0) {
          error!("{}", e);
        }
        Ok(())
      });
    }
  }

  fn exit(&self) {
    let mut data = self.data.lock().unwrap();
    data.quit.take().map(|s| {
      if !s.is_closed() {
        let _ = s.send(true);
      }
    });
  }

  fn newSessionID(&self) -> i32 {
    let mut data = self.data.lock().unwrap();
    if data.sessionID == std::i32::MAX {
      data.sessionID = 1;
      return 1;
    } else {
      data.sessionID += 1;
      data.sessionID
    }
  }

  fn setName(&self, taskName: &str) {
    let mut data = self.data.lock().unwrap();
    data.name = Some(taskName.to_owned());
  }
}

impl Drop for Task {
  fn drop(&mut self) {
    self.exit();
    self.state = Default::default();
  }
}

extern "C" fn l_newTask(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    state.check_type(1, Type::Function)?;
    state.push_value(1);
    let taskRef = state.reference(LUA_REGISTRYINDEX);
    let name = if state.get_top() > 1 && !state.is_nil(2) {
      Some(state.check_string(2)?)
    } else {
      None
    };

    let mut tasks = TASKS.lock().unwrap();
    if let Some(n) = name {
      if tasks.namedIds.contains_key(n) {
        state.unreference(LUA_REGISTRYINDEX, taskRef);
        return Ok(0);
      }
    }

    let taskID = tasks.newTaskID();
    let (tx, rx) = mpsc::unbounded_channel::<Message>();
    let (qtx, qrx) = oneshot::channel::<bool>();
    let taskData = Arc::new(Mutex::new(TaskData {
      sessionID: 1,
      name: name.map(|v| v.into()),
      quit: Some(qtx),
    }));
    let task = Task {
      taskID: taskID,
      taskRef: taskRef,
      state: with_state(|s| Arc::downgrade(s)),
      sender: tx,
      data: taskData,
    };
    Task::start(task.clone(), rx, qrx);
    name.map(|n| tasks.namedIds.insert(n.to_owned(), task.taskID));
    tasks.tasks.insert(taskID, task);
    state.push_integer(taskID as Integer);
    Ok(1)
  })
}

extern "C" fn l_register(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let taskName = state.check_string(1)?;
    if !CURRENT.is_set() {
      return Err(anyhow!("register must be called in task"));
    }
    CURRENT.with(|task| {
      debug!("regist task {} name = {}", task.taskID, taskName);
      task.setName(taskName);
      let mut tasks = TASKS.lock().unwrap();
      tasks.namedIds.insert(taskName.to_owned(), task.taskID);
    });
    Ok(0)
  })
}

extern "C" fn l_timeout(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let n = state.check_integer(-1)?;
    if !CURRENT.is_set() {
      return Err(anyhow!("timeout must be called in task"));
    }
    let sessionID = CURRENT.with(move |task| {
      let ss = task.sender.clone();
      let sessionID = task.newSessionID();
      tokio::spawn(async move {
        sleep(Duration::from_millis(n as u64)).await;
        let _ = ss.send(Message::new(sessionID, None));
      });
      sessionID
    });
    state.push_integer(sessionID as Integer);
    Ok(1)
  })
}

extern "C" fn l_newSessionId(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let sessionID = if CURRENT.is_set() {
      CURRENT.with(move |task| task.newSessionID())
    } else {
      let taskID = state.check_integer(1)? as i32;
      let tasks = TASKS.lock().unwrap();
      if let Some(task) = tasks.tasks.get(&taskID) {
        task.newSessionID()
      } else {
        0
      }
    };
    state.push_integer(sessionID as Integer);
    Ok(1)
  })
}

extern "C" fn l_exit(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    if state.get_top() >= 1 {
      let taskID = state.check_integer(1)? as i32;
      let tasks = TASKS.lock().unwrap();
      if let Some(task) = tasks.tasks.get(&taskID) {
        task.exit();
      }
    } else if CURRENT.is_set() {
      CURRENT.with(|task| task.exit());
    }
    Ok(0)
  })
}

extern "C" fn l_getTaskId(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    if state.get_top() >= 1 {
      let name = state.check_string(1)?;
      let tasks = TASKS.lock().unwrap();
      if let Some(taskID) = tasks.namedIds.get(&name.to_owned()) {
        state.push_integer(*taskID as Integer);
        return Ok(1);
      }
    } else if CURRENT.is_set() {
      let taskID = CURRENT.with(|task| task.taskID);
      state.push_integer(taskID as Integer);
      return Ok(1);
    }
    Ok(0)
  })
}

extern "C" fn l_send(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let taskID = state.check_integer(1)? as i32;
    let mut data = None;
    let mut sessionId = 0;
    if state.get_top() >= 2 {
      sessionId = state.check_integer(2)? as i32;
    }
    if state.get_top() >= 3 {
      data = Some(state.check_string(3)?.to_owned());
    }

    let tasks = TASKS.lock().unwrap();
    if let Some(task) = tasks.tasks.get(&taskID) {
      let _ = task.sender.send(Message::new(sessionId, data.map(|d| d.into())));
    }
    Ok(0)
  })
}

pub(super) fn open_tasks(state: &mut State) {
  let _stack = state.save_stack();
  let l = vec![
    luaL_Reg::new(b"newTask\0", Some(l_newTask)),
    luaL_Reg::new(b"timeout\0", Some(l_timeout)),
    luaL_Reg::new(b"register\0", Some(l_register)),
    luaL_Reg::new(b"getTaskId\0", Some(l_getTaskId)),
    luaL_Reg::new(b"exit\0", Some(l_exit)),
    luaL_Reg::new(b"newSessionId\0", Some(l_newSessionId)),
    luaL_Reg::new(b"send\0", Some(l_send)),
    luaL_Reg::null(),
  ];
  state.register(b"portal.tasks\0", &l);
}
