#[macro_export]
macro_rules! defhandle {
  ($module:ident, {$($(#[$($decorator:expr),+])? $h:ident($($p:ident:$a:ty),*) -> $r:ty;)+}) => {
    pub enum EHandles {
      $($h,)+
      MAX_HANDLE
    }

    const HANDLE_NAMES: [&'static str; EHandles::MAX_HANDLE as usize] = [
      $(&stringify!($h),)+
    ];

    lazy_static! {
      static ref INITDATA: std::sync::Arc<std::sync::Mutex<(std::path::PathBuf, Option<fn(state: &mut lua::State) -> anyhow::Result<()>>)>>
        = std::sync::Arc::new(std::sync::Mutex::new((std::path::PathBuf::default(), None)));
    }

    thread_local! {
      static $module: std::cell::RefCell<Option<Dflib>> = std::cell::RefCell::new(None);
    }

    fn init_dflib(d: &mut Dflib, path: &std::path::Path, mode_init: &Option<fn(state: &mut lua::State) -> anyhow::Result<()>>) -> anyhow::Result<()> {
      d.init(path, mode_init)?;
      d.load_handles(&HANDLE_NAMES)?;
      Ok(())
    }

    pub fn init(path: &std::path::Path, mode_init: Option<fn(state: &mut lua::State) -> anyhow::Result<()>>) -> anyhow::Result<()> {
      $module.with(|c| {
        let mut x = c.borrow_mut();
        match *x {
          Some(_) => Ok(()),
          None => {
            let init_data = {
              *x = Some(Dflib::new(stringify!($module)));
              let mut pp = INITDATA.lock().unwrap();
              let data = (path.to_path_buf(), mode_init);
              *pp = data.clone();
              data
            };
            init_dflib(x.as_mut().unwrap(), &init_data.0, &init_data.1)?;
            Ok(())
          }
        }
      })
    }

    fn get_dflib( x: &mut Option<Dflib>) -> &mut Dflib {
      if let Some(d) = x {
        return d;
      }

      let init_data = {
        *x = Some(Dflib::new(stringify!($module)));
        let pp = INITDATA.lock().unwrap();
        pp.clone()
      };
      let d = x.as_mut().unwrap();
      init_dflib(d, &init_data.0, &init_data.1).unwrap();
      d
    }

    fn with<F, R>(f: F) -> R
    where
      F: FnOnce(&mut Dflib) -> R,
    {
      $module.with(|c| f(get_dflib(&mut *c.borrow_mut())))
    }

    use crate::core::IntoResponse;
    use crate::core::ReplyFinish;
    pub mod raw {
      use super::*;
      $(
        pub async fn $h(p: ($($a,)*)) -> anyhow::Result<$r> {
          let mut s = with(move |d: &mut Dflib| d.call::<_, $r>(&stringify!($h), p)).await?;
          $($($decorator.render(&mut s);)+)?
          s.finish();
          Ok(s)
        }
      )+
    }

    $(
      pub async fn $h($($p:$a),*) -> Result<warp::http::Response<String>, warp::Rejection> {
        match raw::$h(($($p,)*)).await {
          Ok(mut s) => Ok(s.response()),
          Err(e) => Err(warp::reject::custom(core::InternalError(e.to_string()))),
        }
      }
    )+
  };
}
