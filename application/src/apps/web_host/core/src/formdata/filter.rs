use anyhow::{anyhow, Error};
use std::pin::Pin;
use std::task::{Context, Poll};

use super::formdata::{FormData as FormDataInner, Part as PartInner};
use bytes::{Buf, Bytes};
use futures::{Stream, StreamExt, TryStreamExt};
use headers::{ContentType, Header, HeaderValue};
use mime::Mime;
use warp::reject::Rejection;
use warp::Filter;

pub struct FormData<T> {
  inner: FormDataInner<T>,
}

pub struct Part<T> {
  inner: PartInner<T>,
}

async fn make_form_data<T>(
  boundary: String,
  body: T,
) -> Result<impl Stream<Item = std::io::Result<Part<impl Stream<Item = std::io::Result<Bytes>>>>>, Rejection>
where
  T: TryStreamExt + Unpin,
  T::Ok: Buf,
  T::Error: std::error::Error,
{
  let s = body.into_stream().map(|s| match s {
    Ok(v) => Ok(Bytes::copy_from_slice(v.chunk())),
    Err(err) => Err(std::io::Error::new(std::io::ErrorKind::Other, err.to_string())),
  });
  Ok(FormData {
    inner: FormDataInner::new(s, &boundary),
  })
}

fn get_boundary(header_val: String) -> Result<String, Error> {
  let val = [HeaderValue::from_str(&header_val)?];
  let mut iter = val.iter();
  let ct = ContentType::decode(&mut iter)?;
  let mime = Mime::from(ct);
  let boundary = mime
    .get_param("boundary")
    .map(|v| v.to_string())
    .ok_or_else(|| anyhow!("invalid content-type"))?;
  Ok(boundary)
}

fn check_content_type(content_str: String) -> Result<String, Error> {
  let val = [HeaderValue::from_str(&content_str)?];
  let mut iter = val.iter();
  match ContentType::decode(&mut iter) {
    Ok(_) => {
      Ok(content_str)
    },
    Err(e) => {
      error!("this content is invalid {}", content_str);
      return Err(anyhow!("invalid Content-Type {}", e.to_string()));
    }
  }
}

#[derive(Debug)]
pub struct InvalidContentType(String);
impl warp::reject::Reject for InvalidContentType {}

pub fn form(
  limit: u64,
) -> impl Filter<
  Extract = (impl Stream<Item = std::io::Result<Part<impl Stream<Item = std::io::Result<Bytes>>>>>,),
  Error = Rejection,
> + Clone {
  warp::body::content_length_limit(limit)
    .and(
      warp::header::header::<String>("content-type").and_then(|val| async move {
        get_boundary(val).map_err(|e| warp::reject::custom(InvalidContentType(e.to_string())))
      }),
    )
    .and(warp::body::stream())
    .and_then(make_form_data)
}

impl<T> Part<T> {
  pub fn name(&self) -> &str {
    &self.inner.headers.name
  }

  pub fn filename(&self) -> Option<&str> {
    self.inner.headers.filename.as_deref()
  }

  pub fn content_type(&self) -> Option<&str> {
    self.inner.headers.content_type.as_deref()
  }

  pub fn content_type_valid(&self) -> Result<String, Error> {
    match self.inner.headers.content_type.as_deref() {
      Some(content_str) => {
        check_content_type(content_str.to_string())
      },
      None => {
        Ok("".to_string())
      }
    }
  }
}

impl<T> Stream for FormData<T>
where
  T: Stream<Item = std::io::Result<Bytes>> + Unpin,
{
  type Item = std::io::Result<Part<T>>;

  fn poll_next(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<Option<Self::Item>> {
    match Pin::new(&mut self.inner).poll_next(cx) {
      Poll::Pending => Poll::Pending,
      Poll::Ready(Some(Ok(part))) => Poll::Ready(Some(Ok(Part { inner: part }))),
      Poll::Ready(None) => Poll::Ready(None),
      Poll::Ready(Some(Err(err))) => Poll::Ready(Some(Err(err))),
    }
  }
}

impl<T> Stream for Part<T>
where
  T: Stream<Item = std::io::Result<Bytes>> + Unpin,
{
  type Item = std::io::Result<Bytes>;

  fn poll_next(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<Option<Self::Item>> {
    match Pin::new(&mut self.inner).poll_next(cx) {
      Poll::Pending => Poll::Pending,
      Poll::Ready(Some(Ok(bytes))) => Poll::Ready(Some(Ok(bytes))),
      Poll::Ready(None) => Poll::Ready(None),
      Poll::Ready(Some(Err(err))) => Poll::Ready(Some(Err(err))),
    }
  }
}
