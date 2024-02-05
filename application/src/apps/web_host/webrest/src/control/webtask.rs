use anyhow::Result;
use core::{lock_state_call, with_state, HandleFuture, InternalError, IntoResponse, Reply};
use headers::{HeaderMap, HeaderMapExt, HeaderValue, Range};
use libc::c_int;
use lua::{luaL_Reg, lua_State, wrap_lua_func, Integer, State, ToLua, Type, LUA_REGISTRYINDEX};
use std::collections::HashMap;
use std::ops::Bound;
use std::sync::{Arc, Mutex, Weak};
use uuid::Uuid;
use warp::filters::fs::File as WarpFile;
use warp::{http::Response, reject, Filter, Rejection};

struct DownloadTaskInfo {
  state: Weak<Mutex<State>>,
  fileName: String,
  downloadType: i32,
  href: c_int,
  uuid: String,
}

impl DownloadTaskInfo {
  fn new(state: Weak<Mutex<State>>, href: c_int) -> Self {
    DownloadTaskInfo {
      state: state,
      fileName: String::default(),
      downloadType: 0,
      href,
      uuid: String::default(),
    }
  }
}

impl Drop for DownloadTaskInfo {
  fn drop(&mut self) {
    if let Some(s) = self.state.upgrade() {
      let _ = lock_state_call(&s, |state| {
        if self.href != 0 {
          state.unreference(LUA_REGISTRYINDEX, self.href);
          debug!("destory taskinfo url {}", self.uuid);
        }
        Ok(())
      });
      self.downloadType = 0;
      self.href = 0;
      self.state = Default::default();
    }
  }
}

struct DownloadTaskList {
  task: HashMap<String, DownloadTaskInfo>,
  busy: HashMap<i32, i32>,
}

lazy_static! {
  static ref TASKLIST: Arc<Mutex<DownloadTaskList>> = Arc::new(Mutex::new(DownloadTaskList {
    task: HashMap::new(),
    busy: HashMap::new()
  }));
}

impl DownloadTaskList {
  fn addTaskInfo(&mut self, taskInfo: DownloadTaskInfo) -> bool {
    if !self.busy.contains_key(&taskInfo.downloadType) {
      self.busy.insert(taskInfo.downloadType, 1);
      self.task.insert(taskInfo.uuid.clone(), taskInfo);
      return true;
    } else {
      return false;
    }
  }

  fn removeTaskInfo(&mut self, uuid: &str) {
    self.task.remove(uuid);
  }

  fn finishTaskInfo(&mut self, uuid: &str) {
    if let Some(t) = self.task.get(uuid) {
      self.busy.remove(&t.downloadType);
    }
  }

  fn findTaskInfo(&self, uuid: &str) -> Option<&DownloadTaskInfo> {
    self.task.get(uuid)
  }

  fn newUUID(&self) -> String {
    loop {
      let uuid = Uuid::new_v4().to_string();
      if !self.task.contains_key(&uuid) {
        return uuid;
      }
    }
  }
}

const MAX_FILESIZE: u64 = std::i32::MAX as u64;

pub struct DownloadParam {
  range: (u64, u64),
  fileName: Option<String>,
  href: c_int,
  state: Arc<Mutex<State>>,
  uuid: String,
}

impl ToLua for DownloadParam {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.create_table(0, 2)?;

    L.push_string("fileName")?;
    self.fileName.to_lua(L)?;
    L.set_table(-3)?;

    L.push_string("range")?;
    L.create_table(2, 0)?;
    L.push_integer(self.range.0 as Integer);
    L.raw_seti(-2, 1)?;
    L.push_integer(self.range.1 as Integer);
    L.raw_seti(-2, 2)?;
    L.set_table(-3)?;

    Ok(())
  }
}

fn findTaskInfo(uuid: &str, fileName: Option<String>) -> Option<DownloadParam> {
  let taskList = TASKLIST.lock().unwrap();
  if let Some(taskInfo) = taskList.findTaskInfo(uuid) {
    if fileName.as_ref().map(|v| &taskInfo.fileName == v).unwrap_or(true) {
      if let Some(state) = taskInfo.state.upgrade() {
        return Some(DownloadParam {
          state,
          fileName: Some(taskInfo.fileName.clone()),
          href: taskInfo.href,
          uuid: uuid.into(),
          range: (0, 0),
        });
      }
    }
  }
  None
}

fn bytes_range(range: Option<Range>, maxLen: u64) -> (u64, u64) {
  if let Some(range) = range {
    let mut minVal = maxLen;
    let mut maxVal = 0;
    for (start, end) in range.iter() {
      minVal = std::cmp::min(
        match start {
          Bound::Unbounded => 0,
          Bound::Included(s) => s,
          Bound::Excluded(s) => s + 1,
        },
        maxLen,
      );

      maxVal = std::cmp::max(
        match end {
          Bound::Unbounded => maxLen,
          Bound::Included(s) => s + 1,
          Bound::Excluded(s) => s,
        },
        maxLen,
      );
    }

    if minVal <= maxLen {
      return (minVal, maxVal);
    }
  }

  (0, maxLen)
}

const TMP_PATH: &'static str = "/tmp";
pub fn download() -> impl Filter<Extract = (DownloadParam, WarpFile), Error = Rejection> + Clone {
  warp::path::peek()
    .and(warp::header::headers_cloned())
    .and_then(|peek: warp::path::Peek, headers: HeaderMap| async move {
      let uuid = peek.as_str().to_owned();
      if let Some(mut param) = findTaskInfo(&uuid, None) {
        param.range = bytes_range(headers.typed_get::<Range>(), MAX_FILESIZE);
        return Ok(param);
      }
      Err(reject::not_found())
    })
    .and(warp::fs::dir(TMP_PATH))
}

pub async fn DownloadSuccessHandle(
  param: DownloadParam,
  file: WarpFile,
) -> std::result::Result<Box<dyn warp::Reply>, Rejection> {
  let headVal = match &param.fileName {
    Some(fileName) => Some(
      HeaderValue::from_str(&("attachment; filename=".to_owned() + fileName))
        .map_err(|e| warp::reject::custom(InternalError(e.to_string())))?,
    ),
    None => None,
  };
  match HandleFuture::<_, Reply<String>>::new(param.state.clone(), "download", param.href, (param,)).await {
    Ok(_) => {}
    Err(e) => return Err(warp::reject::custom(InternalError(e.to_string()))),
  }
  if let Some(val) = headVal {
    return Ok(Box::new(warp::reply::with_header(file, "Content-Disposition", val)));
  } else {
    return Ok(Box::new(file));
  }
}

pub async fn QryDownloadTaskInfo(uuid: String) -> std::result::Result<Response<String>, warp::Rejection> {
  if let Some(param) = findTaskInfo(&uuid, None) {
    let s = param.state.clone();
    match HandleFuture::<_, Reply<String>>::new(s, "qry_download", param.href, (param,)).await {
      Ok(s) => Ok(s.response()),
      Err(e) => Err(warp::reject::custom(InternalError(e.to_string()))),
    }
  } else {
    Err(warp::reject::not_found())
  }
}

pub(super) extern "C" fn addDownloadTaskInfo(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    state.check_type(3, Type::Function)?;
    state.push_value(3);
    let href = state.reference(LUA_REGISTRYINDEX);
    let mut task = DownloadTaskInfo::new(with_state(|s| Arc::downgrade(s)), href);

    let mut taskList = TASKLIST.lock().unwrap();
    let name = state.check_string(1)?;
    let uuid = taskList.newUUID();
    debug!("regist download taskinfo url {} {}", name, uuid);
    task.fileName = name.to_owned();
    task.uuid = uuid.to_owned();

    let downloadType = state.check_integer(2)?;
    task.downloadType = downloadType as i32;
    if taskList.addTaskInfo(task) {
      state.push_string(&uuid)?;
      Ok(1)
    } else {
      Ok(0)
    }
  })
}

pub(super) extern "C" fn finishDownloadTaskInfo(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let uuid = state.check_string(-1)?;
    let mut taskList = TASKLIST.lock().unwrap();
    taskList.finishTaskInfo(&uuid.to_owned());
    Ok(0)
  })
}

pub(super) extern "C" fn delDownloadTaskInfo(L: *mut lua_State) -> c_int {
  wrap_lua_func(&mut State::from(L), |state| {
    let uuid = state.check_string(-1)?;
    let mut taskList = TASKLIST.lock().unwrap();
    taskList.removeTaskInfo(&uuid.to_owned());
    Ok(0)
  })
}

pub fn open_webtask(state: &mut State) -> Result<()> {
  let _stack = state.save_stack();
  let l = vec![
    luaL_Reg::new(b"addTaskInfo\0", Some(addDownloadTaskInfo)),
    luaL_Reg::new(b"finishTaskInfo\0", Some(finishDownloadTaskInfo)),
    luaL_Reg::new(b"delTaskInfo\0", Some(delDownloadTaskInfo)),
    luaL_Reg::null(),
  ];
  state.register(b"webrest.task\0", &l);
  Ok(())
}
