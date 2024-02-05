use super::parser::{Parser, ParserResult};
use std::io::{Error, ErrorKind, Result};
use std::pin::Pin;
use std::sync::{Arc, Mutex};
use std::task::{Context, Poll};

use bytes::Bytes;
use futures::{Stream, TryStreamExt};
use pin_project::pin_project;

use super::formdata_headers::Headers;

fn other_err<T>(error: &'static str) -> Poll<Option<Result<T>>> {
    Poll::Ready(Some(Err(Error::new(ErrorKind::Other, error))))
}

#[derive(Debug)]
pub enum Read {
    NewPart { headers: Headers },
    Part(Bytes),
    PartEof,
}

#[pin_project]
struct FormDataInner<S> {
    #[pin]
    stream: S,
    parser: Parser,
}

impl<S> FormDataInner<S> {
    pub fn new(stream: S, boundary: &str) -> Self {
        let parser = Parser::new(boundary);
        Self { stream, parser }
    }
}

impl<S> Stream for FormDataInner<S>
where
    S: Stream<Item = Result<Bytes>> + TryStreamExt,
{
    type Item = Result<Read>;

    fn poll_next(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Option<Self::Item>> {
        let mut this = self.project();

        loop {
            match this.parser.poll() {
                Ok(ParserResult::NeedData) => {
                    match Pin::new(&mut this.stream).poll_next(cx) {
                        Poll::Pending => return Poll::Pending,
                        Poll::Ready(Some(Ok(bytes))) => {
                            this.parser
                                .put(bytes)
                                .map_err(|err| Error::new(ErrorKind::Other, err))?;
                        }
                        Poll::Ready(Some(Err(err))) => return Poll::Ready(Some(Err(err))),
                        Poll::Ready(None) => this.parser.eof(),
                    };
                }
                Ok(ParserResult::NewPart { headers }) => {
                    return Poll::Ready(Some(Ok(Read::NewPart { headers })))
                }
                Ok(ParserResult::Part(bytes)) => return Poll::Ready(Some(Ok(Read::Part(bytes)))),
                Ok(ParserResult::PartEof) => return Poll::Ready(Some(Ok(Read::PartEof))),
                Ok(ParserResult::Next) => {}
                Ok(ParserResult::Eof) => return Poll::Ready(None),
                Err(err) => return Poll::Ready(Some(Err(Error::new(ErrorKind::Other, err)))),
            }
        }
    }
}

pub struct FormData<S> {
    inner: Arc<Mutex<FormDataInner<S>>>,
}

pub struct Part<S> {
    pub headers: Headers,
    inner: Option<Arc<Mutex<FormDataInner<S>>>>,
}

impl<S> FormData<S> {
    pub fn new(stream: S, boundary: &str) -> Self {
        let inner = FormDataInner::new(stream, boundary);
        Self {
            inner: Arc::new(Mutex::new(inner)),
        }
    }

    // 判断是否有 Part 持有了 inner 对象
    fn has_part(&mut self) -> bool {
        return Arc::get_mut(&mut self.inner).is_none();
    }
}

impl<S> Stream for FormData<S>
where
    S: Stream<Item = Result<Bytes>> + Unpin,
{
    type Item = Result<Part<S>>;

    fn poll_next(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<Option<Self::Item>> {
        if self.has_part() {
            cx.waker().wake_by_ref();
            return Poll::Pending;
        }

        let mut inner = match self.inner.lock() {
            Ok(inner) => inner,
            Err(_) => return other_err("poll form-data failed: other part"),
        };

        match Pin::new(&mut *inner).poll_next(cx) {
            Poll::Pending => Poll::Pending,
            Poll::Ready(Some(Ok(Read::NewPart { headers }))) => Poll::Ready(Some(Ok(Part {
                headers,
                inner: Some(Arc::clone(&self.inner)),
            }))),
            Poll::Ready(Some(Ok(Read::Part(_)))) | Poll::Ready(Some(Ok(Read::PartEof))) => {
                other_err("poll form-data failed: Part or PartEor")
            }
            Poll::Ready(Some(Err(err))) => Poll::Ready(Some(Err(err))),
            Poll::Ready(None) => Poll::Ready(None),
        }
    }
}

impl<S> Stream for Part<S>
where
    S: Stream<Item = Result<Bytes>> + Unpin,
{
    type Item = Result<Bytes>;

    fn poll_next(mut self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Option<Self::Item>> {
        let inner_ = match &self.inner {
            Some(inner) => inner,
            None => return Poll::Ready(None),
        };

        let mut inner = match inner_.try_lock() {
            Ok(inner) => inner,
            Err(_) => return other_err("poll part failed: too many part"),
        };

        match Pin::new(&mut *inner).poll_next(cx) {
            Poll::Pending => Poll::Pending,
            Poll::Ready(Some(Ok(Read::Part(bytes)))) => Poll::Ready(Some(Ok(bytes))),
            Poll::Ready(Some(Ok(Read::PartEof))) | Poll::Ready(None) => {
                drop(inner);

                self.inner = None;
                Poll::Ready(None)
            }
            Poll::Ready(Some(Err(err))) => Poll::Ready(Some(Err(err))),
            Poll::Ready(Some(Ok(Read::NewPart { .. }))) => other_err("poll part failed: NewPart"),
        }
    }
}
