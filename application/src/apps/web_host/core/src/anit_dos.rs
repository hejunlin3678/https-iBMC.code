use super::ResponseError;
use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use warp::{http::Response, http::StatusCode, Filter, Rejection};

const DOS_BLOCKING_COUNT: u32 = 16;
const DOS_BLOCKING_TIME: Duration = Duration::from_secs(300);
const ACCESS_LOG_MAX_SIZE: usize = 4096;

struct AccessInfo {
  last_access_time: Instant,
  count: u32,
}

lazy_static! {
  static ref G_ACCESS_LOG: Arc<Mutex<HashMap<String, AccessInfo>>> = Arc::new(Mutex::new(HashMap::new()));
}

fn access_log_record(client_ip: &str) {
  let mut acces_log = G_ACCESS_LOG.lock().unwrap();
  if let Some(info) = acces_log.get_mut(client_ip) {
    if info.last_access_time.elapsed() > DOS_BLOCKING_TIME {
      // 如果距离上次访问时间大于阈值,则之前的访问次数清零
      info.count = 1;
    } else {
      // 如果距离上次访问时间小于等于阈值, 则访问次数+1
      info.count += 1;
    }
    info.last_access_time = Instant::now();
    return;
  }

  /* 不存在则插入一条到哈希表 */
  /* 先判断当前哈希表长度,如果超过设定的最大阈值,则清空之前的记录 */
  if acces_log.len() >= ACCESS_LOG_MAX_SIZE {
    acces_log.clear();
  }

  acces_log.insert(
    client_ip.to_owned(),
    AccessInfo {
      count: 1,
      last_access_time: Instant::now(),
    },
  );
}

fn response_forbid() -> Rejection {
  let mut res = Response::new(String::default());
  *res.status_mut() = StatusCode::FORBIDDEN;
  warp::reject::custom(ResponseError(res))
}

fn is_login_url(uri: &[u8]) -> bool {
  uri.eq_ignore_ascii_case(b"/redfish/v1/sessionservice/sessions") || uri.eq_ignore_ascii_case(b"/ui/rest/login")
}

fn is_free_url(uri: &[u8]) -> bool {
  uri.eq_ignore_ascii_case(b"/redfish/v1") || uri.eq_ignore_ascii_case(b"/redfish")
}

fn update_access_log(client_ip: &str, resp_code: StatusCode, uri: &[u8]) {
  if is_free_url(uri) {
    return;
  }

  if match resp_code {
    StatusCode::UNAUTHORIZED => false,
    StatusCode::BAD_REQUEST if is_login_url(uri) => false,
    _ => true,
  } {
    // 非鉴权错误,则尝试从哈希表中删除可能存在的访问记录
    let mut acces_log = G_ACCESS_LOG.lock().unwrap();
    acces_log.remove(client_ip);
    return;
  }

  access_log_record(client_ip);
}

fn is_client_locked(client_ip: &str) -> bool {
  let mut acces_log = G_ACCESS_LOG.lock().unwrap();
  if let Some(info) = acces_log.get(client_ip) {
    if info.count < DOS_BLOCKING_COUNT {
      return false;
    }

    if info.last_access_time.elapsed() <= DOS_BLOCKING_TIME {
      return false; // 暂时屏蔽IP锁定检查功能，固定返回false
    }

    // 定时间结束,从哈希表中删除该记录
    acces_log.remove(client_ip);
  }

  return false;
}

pub fn is_locked() -> impl Filter<Extract = (Option<String>,), Error = Rejection> + Clone + Send + Sync + 'static {
  warp::header::optional("X-Real-IP").and_then(move |client_ip: Option<String>| async move {
    match client_ip {
      Some(ref ip) if is_client_locked(ip) => Err(response_forbid()),
      _ => Ok(client_ip),
    }
  })
}

pub fn warp<F>(
  filter: F,
) -> impl Filter<Extract = (Response<String>,), Error = Rejection> + Clone + Send + Sync + 'static
where
  F: Filter<Extract = (Response<String>,), Error = Rejection> + Clone + Send + Sync + 'static,
{
  is_locked().and(filter).and(warp::path::full()).map(
    |client_ip: Option<String>, reply: Response<String>, uri: warp::path::FullPath| {
      client_ip.map(|ip| update_access_log(&ip, reply.status(), &uri.as_str().as_bytes()));
      reply
    },
  )
}
