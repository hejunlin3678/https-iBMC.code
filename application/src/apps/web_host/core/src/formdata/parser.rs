use super::buffer::Buffer;
use super::error::Error;
use super::formdata_headers::{Headers, RawHeader};
use bytes::Bytes;

#[derive(Debug, Clone)]
pub struct Boundary(Bytes);

impl Boundary {
  pub fn new(boundary: &str) -> Self {
    Self(format!("\r\n--{}", boundary).into())
  }

  #[inline]
  pub fn with_dashes(&self) -> Bytes {
    self.0.slice(2..)
  }

  #[inline]
  pub fn with_new_line_and_dashes(&self) -> &Bytes {
    &self.0
  }
}

#[derive(Debug, PartialEq)]
enum State {
  Init,
  Boundary,
  Headers,
  Part,
  Eof,
}

#[derive(Debug, PartialEq)]
pub(super) enum ParserResult {
  NewPart { headers: Headers },
  Part(Bytes),
  PartEof,
  Next,
  NeedData,
  Eof,
}

fn parse_headers(data: &Bytes) -> Result<Option<(Bytes, Headers)>, Error> {
  let mut headers = [httparse::EMPTY_HEADER; 8];
  match httparse::parse_headers(data, &mut headers) {
    Ok(httparse::Status::Complete((read, headers))) => {
      let headers = headers
        .iter()
        .map(|header| RawHeader {
          name: header.name,
          value: header.value,
        })
        .collect::<Vec<_>>();

      let headers = Headers::new(&headers)?;
      Ok(Some((data.slice(read..), headers)))
    }
    Ok(httparse::Status::Partial) => Ok(None),
    Err(err) => Err(Error::Headers(err)),
  }
}

pub(super) struct Parser {
  boundary: Boundary,
  buf: Buffer,
  state: State,
}

impl Parser {
  pub fn new(boundary: &str) -> Self {
    Self {
      boundary: Boundary::new(boundary),
      buf: Buffer::new(),
      state: State::Init,
    }
  }

  pub fn put(&mut self, data: Bytes) -> Result<(), Error> {
    if self.state == State::Eof {
      return Err(Error::UnexpectedData);
    }

    self.buf.put(data);
    Ok(())
  }

  pub fn poll(&mut self) -> Result<ParserResult, Error> {
    match self.state {
      State::Init => self.process_init(),
      State::Boundary => self.process_boundary(),
      State::Headers => self.process_headers(),
      State::Part => self.process_part(),
      State::Eof => Ok(ParserResult::Eof),
    }
  }

  pub fn eof(&mut self) {
    self.state = State::Eof;
  }

  fn process_init(&mut self) -> Result<ParserResult, Error> {
    let boundary = self.boundary.with_dashes();
    match self.buf.read_until_boundary(&boundary) {
      Some((_, true)) => {
        self.buf.skip(boundary.len());
        self.state = State::Boundary;
        Ok(ParserResult::Next)
      }
      _ => Ok(ParserResult::NeedData),
    }
  }

  fn process_boundary(&mut self) -> Result<ParserResult, Error> {
    let data = self.buf.bytes();
    if data.len() < 2 {
      return Ok(ParserResult::NeedData);
    }

    if data.starts_with(b"\r\n") {
      self.buf.skip(2);
      self.state = State::Headers;
      return Ok(ParserResult::Next);
    } else if data.starts_with(b"--") {
      self.state = State::Eof;
      return Ok(ParserResult::Eof);
    } else {
      Err(Error::UnexpectedBoundarySuffix)
    }
  }

  fn process_headers(&mut self) -> Result<ParserResult, Error> {
    match parse_headers(self.buf.bytes()) {
      Ok(None) => Ok(ParserResult::NeedData),
      Ok(Some((rem, headers))) => {
        self.buf.set(rem);
        self.state = State::Part;
        Ok(ParserResult::NewPart { headers })
      }
      Err(e) => Err(e),
    }
  }

  fn process_part(&mut self) -> Result<ParserResult, Error> {
    let boundary = self.boundary.with_new_line_and_dashes();
    match self.buf.read_until_boundary(boundary) {
      Some((bytes, true)) => {
        if bytes.is_empty() {
          self.buf.skip(boundary.len());
          self.state = State::Boundary;
          Ok(ParserResult::PartEof)
        } else {
          Ok(ParserResult::Part(bytes))
        }
      }
      Some((bytes, false)) => Ok(ParserResult::Part(bytes)),
      None => Ok(ParserResult::NeedData),
    }
  }
}

#[cfg(test)]
mod tests {
  use super::Headers;
  use super::{Parser, ParserResult, State};
  use bytes::{BufMut, Bytes, BytesMut};

  fn join_bytes(datas: &[&[u8]]) -> Bytes {
    let mut buf = BytesMut::new();
    for data in datas {
      buf.put_slice(data);
    }
    buf.freeze()
  }

  fn make_bytes(data: &'static [u8]) -> Bytes {
    Bytes::from_static(data)
  }

  fn h1() -> Headers {
    Headers {
      name: "aaa".to_owned(),
      filename: None,
      content_type: None,
    }
  }

  fn h2() -> Headers {
    Headers {
      name: "imgFile".to_owned(),
      filename: Some("20221226-101201(WeLinkPC).jpg".to_owned()),
      content_type: Some("image/jpeg".to_owned()),
    }
  }

  fn test_datas() -> (&'static str, Bytes) {
    (
      "--------------------------454816582448802209524263",
      join_bytes(&[
        b"----------------------------454816582448802209524263\r\n",
        b"Content-Disposition: form-data; name=\"aaa\"\r\n\r\n",
        b"1111",
        b"\r\n----------------------------454816582448802209524263\r\n",
        b"Content-Disposition: form-data; name=\"imgFile\"; filename=\"20221226-101201(WeLinkPC).jpg\"\r\n",
        b"Content-Type: image/jpeg\r\n\r\n",
        b"2222",
        b"\r\n----------------------------454816582448802209524263--\r\n",
      ]),
    )
  }

  #[test]
  fn parser_data() {
    let (boundary, data) = test_datas();
    let mut parser = Parser::new(boundary);
    assert_eq!(parser.put(data), Ok(()));

    assert_eq!(parser.poll(), Ok(ParserResult::Next));
    assert_eq!(parser.state, State::Boundary);

    assert_eq!(parser.poll(), Ok(ParserResult::Next));
    assert_eq!(parser.state, State::Headers);

    assert_eq!(parser.poll(), Ok(ParserResult::NewPart { headers: h1() }));
    assert_eq!(parser.state, State::Part);

    assert_eq!(parser.poll(), Ok(ParserResult::Part(make_bytes(b"1111"))));
    assert_eq!(parser.state, State::Part);

    assert_eq!(parser.poll(), Ok(ParserResult::PartEof));
    assert_eq!(parser.state, State::Boundary);

    assert_eq!(parser.poll(), Ok(ParserResult::Next));
    assert_eq!(parser.state, State::Headers);

    assert_eq!(parser.poll(), Ok(ParserResult::NewPart { headers: h2() }));
    assert_eq!(parser.state, State::Part);

    assert_eq!(parser.poll(), Ok(ParserResult::Part(make_bytes(b"2222"))));
    assert_eq!(parser.state, State::Part);

    assert_eq!(parser.poll(), Ok(ParserResult::PartEof));
    assert_eq!(parser.state, State::Boundary);

    assert_eq!(parser.poll(), Ok(ParserResult::Eof));
    assert_eq!(parser.state, State::Eof);
  }

  #[test]
  fn parser_data_step_one_char() {
    let (boundary, datas) = test_datas();
    let mut parser = Parser::new(boundary);
    let mut new_parts = vec![];
    let mut parts = vec![];

    let mut part = BytesMut::new();
    for i in 0..datas.len() {
      let data = Bytes::copy_from_slice(&[datas[i]]);
      assert_eq!(parser.put(data), Ok(()));
      loop {
        match parser.poll() {
          Ok(ParserResult::NewPart { headers }) => {
            new_parts.push(headers);
            continue;
          }
          Ok(ParserResult::Part(bytes)) => {
            part.put(bytes);
            continue;
          }
          Ok(ParserResult::PartEof) => {
            parts.push(part.freeze());
            part = BytesMut::new();
            continue;
          }
          Ok(ParserResult::Next) => continue,
          Ok(ParserResult::NeedData) => break,
          Ok(ParserResult::Eof) => break,
          Err(e) => assert_eq!(Err(e), Ok(())),
        }
      }
    }
    assert_eq!(parser.state, State::Eof);
    assert_eq!(new_parts, vec![h1(), h2()]);
    assert_eq!(parts, vec![make_bytes(b"1111"), make_bytes(b"2222")]);
  }
}
