use warp::{Filter, Rejection, Reply};

#[cfg(feature = "REDFISH")]
use redfish;

#[cfg(feature = "WEBREST")]
use webrest;

pub fn routes() -> impl Filter<Extract = (impl Reply,), Error = Rejection> + Clone {
  let route = warp::path::end().map(|| "web portal service");

  #[cfg(feature = "WEBREST")]
  let route = route.or(webrest::routes());

  #[cfg(feature = "REDFISH")]
  let route = route.or(routes! {
    ["redfish" / "v1" / "Sms" / ..] => (redfish::sms_proxy::routes());
    POST => {
        ["redfish" / "v1" / "Eventclient" / "Sms0"] => (redfish::sms_proxy::routes());
        ["redfish" / "v1" / "Systems" / String / "LogServices" / "HostLog" / "Actions" / "Oem" / String / "LogService.Push"] 
        => and_then(move |_, _| async move {
            Ok::<_, std::convert::Infallible>(())
        }).untuple_one().and(redfish::sms_proxy::routes())
    }
  }).or(redfish::proxy::routes());

  route
}
