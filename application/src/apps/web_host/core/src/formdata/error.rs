use std::error::Error as StdError;
use std::fmt::{self, Debug, Display};

#[derive(Debug, PartialEq)]
pub enum Error {
    MissingContentDisposition,
    ContentDispositionUtf8,
    ContentDispositionNotFormData,
    InvalidContentDispositionParam,
    NoContentDispositionName,
    ContentTypeUtf8,

    UnexpectedBoundarySuffix,
    Headers(httparse::Error),

    UnexpectedData,
}

impl Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match *self {
            Error::MissingContentDisposition => f.write_str("Content-Disposition header not found"),
            Error::ContentDispositionUtf8 => {
                f.write_str("Content-Disposition header isn't valid utf-8")
            }
            Error::ContentDispositionNotFormData => {
                f.write_str("Content-Disposition doesn't begin with 'form-data'")
            }
            Error::InvalidContentDispositionParam => {
                f.write_str("Invalid Content-Disposition parameter")
            }
            Error::NoContentDispositionName => {
                f.write_str("Content-Disposition is missing the name parameter")
            }
            Error::ContentTypeUtf8 => f.write_str("Content-Type header isn't valid utf-8"),
            Error::UnexpectedBoundarySuffix => f.write_str("unexpected boundary suffix"),
            Self::Headers(_) => f.write_str("header parsing error"),
            Error::UnexpectedData => f.write_str("unexpected data"),
        }
    }
}

impl StdError for Error {}
