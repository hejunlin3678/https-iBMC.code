use super::error::Error;
use std::str;

pub(super) struct RawHeader<'a> {
    pub name: &'a str,
    pub value: &'a [u8],
}

fn find_header<'a, 'b>(headers: &'a [RawHeader<'b>], name: &str) -> Option<&'a RawHeader<'b>> {
    headers
        .iter()
        .find(|header| header.name.eq_ignore_ascii_case(name))
}

fn parse_content_disp<'a>(headers: &'a [RawHeader]) -> Result<(&'a str, Option<&'a str>), Error> {
    let hd = find_header(headers, "Content-Disposition").ok_or(Error::MissingContentDisposition)?;
    let value = str::from_utf8(hd.value).map_err(|_| Error::ContentDispositionUtf8)?;

    let content_disposition = value
        .strip_prefix("form-data")
        .ok_or(Error::ContentDispositionNotFormData)?;

    let mut name = None;
    let mut filename = None;

    for param in content_disposition.split(';').skip(1) {
        let param = param.trim();

        let mut params = param.split('=');
        let param_name = params.next().ok_or(Error::InvalidContentDispositionParam)?;
        if param_name != "name" && param_name != "filename" {
            continue;
        }

        let param_value = params.next().ok_or(Error::InvalidContentDispositionParam)?;
        let param_value = param_value.trim_matches(|c: char| c.is_whitespace() || c == '"');
        if param_value.len() == 0 {
            return Err(Error::InvalidContentDispositionParam);
        }

        if param_name == "name" {
            name = Some(param_value);
        } else {
            filename = Some(param_value);
        }
    }

    let name = name.ok_or(Error::NoContentDispositionName)?;

    Ok((name, filename))
}

fn parse_content_type<'a>(headers: &'a [RawHeader]) -> Result<Option<&'a str>, Error> {
    if let Some(header) = find_header(headers, "Content-Type") {
        Ok(Some(
            str::from_utf8(header.value).map_err(|_| Error::ContentTypeUtf8)?,
        ))
    } else {
        Ok(None)
    }
}

#[derive(Debug, Clone, PartialEq)]
pub struct Headers {
    pub name: String,
    pub filename: Option<String>,
    pub content_type: Option<String>,
}

impl Headers {
    pub(super) fn new(headers: &[RawHeader]) -> Result<Headers, Error> {
        let (name, filename) = parse_content_disp(headers)?;
        let content_type = parse_content_type(headers)?;
        Ok(Headers {
            name: name.to_owned(),
            filename: filename.map(|v| v.to_owned()),
            content_type: content_type.map(|v| v.to_owned()),
        })
    }
}

#[cfg(test)]
mod tests {
    use super::{Error, Headers, RawHeader};

    #[test]
    fn parser_header() {
        let result = Headers::new(&[
            RawHeader {
                name: "Content-Disposition",
                value: br#"form-data; name="aaa"; filename="file.txt""#,
            },
            RawHeader {
                name: "Content-Type",
                value: br#"text/plain"#,
            },
        ]);
        assert!(result.is_ok());

        let headers = result.unwrap();
        assert_eq!(headers.name, "aaa");
        assert_eq!(headers.filename.as_deref(), Some("file.txt"));
        assert_eq!(headers.content_type.as_deref(), Some("text/plain"));
    }

    #[test]
    fn parser_header_no_form_data() {
        let result = Headers::new(&[RawHeader {
            name: "Content-Disposition",
            value: br#"name="aaa"; filename="file.txt""#,
        }]);
        assert_eq!(result, Err(Error::ContentDispositionNotFormData));
    }

    #[test]
    fn parser_header_invalie_param() {
        let result = Headers::new(&[RawHeader {
            name: "Content-Disposition",
            value: br#"form-data; name="#,
        }]);
        assert_eq!(result, Err(Error::InvalidContentDispositionParam));
    }
}
