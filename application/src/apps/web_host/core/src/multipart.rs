use super::formdata;
use super::{InternalError, IntoResponse, Reply, ResponseError, User};
use anyhow::{anyhow, Result};
use bytes::{Buf, BufMut, Bytes, BytesMut};
use futures::{Stream, TryFutureExt, TryStreamExt};
use lua::{Integer, State, ToLua};
use serde_json::{json, Value as JsonValue};
use std::future::Future;
use std::path::Path;
use std::sync::Arc;
use tokio::fs::File as TokioFile;
use tokio::io::AsyncWriteExt;
use warp::{Filter, Rejection};
use chrono::{Utc};

pub struct AutoDeleteFile {
  pub path: String,
  pub file: Option<TokioFile>,
  autoDel: bool,
  forcedDel:bool,
}

impl AutoDeleteFile {
  pub fn new(path: String, file: TokioFile) -> AutoDeleteFile {
    AutoDeleteFile {
      path: path,
      file: Some(file),
      autoDel: true,
      forcedDel: false,
    }
  }

  pub fn new_forcedel(path: String, file: TokioFile) -> AutoDeleteFile {
    AutoDeleteFile {
      path: path,
      file: Some(file),
      autoDel: true,
      forcedDel: true,
    }
  }

  // 关闭文件
  fn close(&mut self) {
    self.file = None;
  }

  // release 后不再自动删除文件
  fn release(&mut self) {
    self.autoDel = false;
  }
}

impl Drop for AutoDeleteFile {
  fn drop(&mut self) {
    self.close();
    // 自动删除，只针对/tmp/目录下的文件，非/tmp目录下文件需要手动删除
    if (self.autoDel && self.path.len() > 5 && &self.path[0..5] == "/tmp/") ||
        self.forcedDel {
      match std::fs::remove_file(&self.path) {
        Ok(_) => info!("remove file"),
        Err(e) => error!("{}", e),
      }
    }
  }
}

#[derive(Clone)]
pub enum FormValue {
  File(Arc<AutoDeleteFile>),
  FileContent((Bytes, String)),
  Data(Bytes),
}

#[derive(Clone)]
pub struct FormField {
  name: String,
  realName: String,
  value: FormValue,
}

#[derive(Clone)]
pub struct FormData {
  fields: Vec<FormField>,
  user: User,
  uri: String,
  error: Option<String>,
  allowFileTypes: Option<&'static [&'static str]>,
}

impl FormData {
  fn new(fields: Vec<FormField>, user: User, uri: warp::path::FullPath) -> Self {
    FormData {
      fields: fields,
      user: user,
      uri: uri.as_str().to_owned(),
      error: None,
      allowFileTypes: None,
    }
  }

  fn error(e: String, user: User, uri: warp::path::FullPath) -> Self {
    FormData {
      fields: vec![],
      user: user,
      uri: uri.as_str().to_owned(),
      error: Some(e),
      allowFileTypes: None,
    }
  }

  pub fn release(&mut self) {
    // 框架暂时不管理文件删除的事情，先 release 一下释放所有权
    for fieled in self.fields.iter_mut() {
      match &mut fieled.value {
        FormValue::File(ref mut autoDelFile) => {
          Arc::get_mut(autoDelFile).map(|file| file.release());
        }
        _ => {}
      }
    }
  }
}

impl ToLua for FormValue {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    match self {
      FormValue::File(file) => file.path.as_str().to_lua(L),
      FormValue::Data(data) => data.to_lua(L),
      FormValue::FileContent((data, file_name)) => {
        L.new_table()?;

        L.push_string("file_name")?;
        file_name.to_lua(L)?;
        L.set_table(-3)?;

        L.push_string("data")?;
        data.to_lua(L)?;
        L.set_table(-3)?;
        Ok(())
      }
    }
  }
}

impl ToLua for FormField {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.new_table()?;

    L.push_string("name")?;
    self.name.to_lua(L)?;
    L.set_table(-3)?;

    L.push_string("value")?;
    self.value.to_lua(L)?;
    L.set_table(-3)?;
    Ok(())
  }
}

impl ToLua for FormData {
  fn to_lua(&self, L: &mut State) -> Result<()> {
    L.new_table()?;

    L.push_string("fields")?;
    L.new_table()?;
    for field in self.fields.iter() {
      field.name.as_str().to_lua(L)?;
      field.value.to_lua(L)?;
      L.set_table(-3)?;

      format!("{}.name", field.name).as_str().to_lua(L)?;
      field.realName.to_lua(L)?;
      L.set_table(-3)?;
    }
    L.set_table(-3)?;

    L.push_string("types")?;
    L.new_table()?;
    for field in self.fields.iter() {
      field.name.as_str().to_lua(L)?;
      match field.value {
        FormValue::File(_) => L.push_string("FILE")?,
        FormValue::FileContent(_) => L.push_string("FILE_CONTENT")?,
        FormValue::Data(_) => L.push_string("TEXT")?,
      }
      L.set_table(-3)?;
    }
    L.set_table(-3)?;

    L.push_string("user")?;
    self.user.to_lua(L)?;
    L.set_table(-3)?;

    L.push_string("uri")?;
    self.uri.to_lua(L)?;
    L.set_table(-3)?;

    if let Some(err) = &self.error {
      L.push_string("error")?;
      err.to_lua(L)?;
      L.set_table(-3)?;
    }

    if let Some(allowFileTypes) = &self.allowFileTypes {
      L.push_string("allowFileTypes")?;
      L.create_table(allowFileTypes.len() as libc::c_int, 0)?;
      for (i, v) in allowFileTypes.into_iter().enumerate() {
        v.to_lua(L)?;
        L.raw_seti(-2, (i + 1) as Integer)?;
      }
      L.set_table(-3)?;
    }

    Ok(())
  }
}

async fn readMultipartDate<T>(s: T, name: String) -> Result<FormField>
where
  T: TryStreamExt,
  T::Ok: Buf,
  T::Error: std::error::Error,
{
  s.try_fold(Vec::new(), |mut buf, data| {
    buf.put(data);
    async move { Ok(buf) }
  })
  .map_ok(move |buf| FormField {
    name: name,
    realName: String::new(),
    value: FormValue::Data(Bytes::from(buf)),
  })
  .map_err(|e| anyhow!("{}", e))
  .await
}

// 生成上传文件的临时存放文件名称
fn get_file_tmp_path(spfileupload: bool) -> Result<(bool, String)>
{
  let mut result = false;
  let mut file_path:String = "N/A".to_string();

  // 重试五次生成临时文件名, 若文件存在则重新生成
  for i in 0..5 {
    // 利用时间戳(精确到毫秒)生成临时文件名
    let time_stamp = Utc::now().timestamp_millis();
    let mut filePath = format!("/tmp/web/{}{}", time_stamp, i);
    if spfileupload {
      filePath = format!("/data/sp/spforbmc/operate/spfwupdate/{}{}", time_stamp, i);
    }

    // 临时文件不再判断软链接，必须寻找一个一定不存在的临时文件名
    if Path::new(filePath.as_str()).exists() {
      // 延时一定时间，防止两次获取的时间戳一致
      std::thread::sleep(std::time::Duration::from_millis(2));
      continue;
    }

    result = true;
    file_path = filePath;
    break;
  }

  Ok((result, file_path))
}

async fn readMultipartFile<T>(
  s: T,
  field_name: String,
  fileName: String,
  fileTypes: &'static [&'static str],
  spfileupload: bool,
) -> Result<FormField>
where
  T: TryStreamExt,
  T::Ok: Buf,
  T::Error: std::error::Error,
{
  let path = Path::new(&fileName);
  if path
    .extension()
    .and_then(|ext| {
      let extLower = ext.to_str()?.to_ascii_lowercase();
      fileTypes.iter().find(|&&ftype| ftype.to_ascii_lowercase() == extLower)
    })
    .is_none()
  {
    return Err(anyhow!("invalid filename {}", fileName));
  }

  match format!("{}", path.display()).find("../") {
    Some(_) => {return Err(anyhow!("invalid filename {}", fileName));}
    _ => {}
  }

  let baseName = path
    .file_name()
    .map_or(None, |v| v.to_str())
    .ok_or(anyhow!("invalid filename {}", fileName))?;

  let filePath = match get_file_tmp_path(spfileupload) {
    Ok((false, _)) => {return Err(anyhow!("file {} upload fail", fileName));},
    Ok((true, file_path)) => {
      file_path
    }, 
    Err(e) => {return Err(anyhow!("file {} upload fail {}", fileName, e));}
  };
  
  let file = tokio::fs::OpenOptions::new()
    .create(true)
    .write(true)
    .truncate(true)
    .open(&filePath)
    .await?;
  // 生成的临时文件无论在什么位置在退出时都必须删除
  let autoDelFile = Arc::new(AutoDeleteFile::new_forcedel(filePath, file));
  s.map_err(|e| anyhow!("{}", e.to_string()))
    .try_fold(autoDelFile, |mut autoDelFile, mut data| async move {
      match Arc::get_mut(&mut autoDelFile) {
        Some(af) => match af.file {
          Some(ref mut file) => {
            while data.has_remaining() {
              file.write_buf(&mut data).await?;
            }
            file.flush().await?;
          }
          None => {}
        },
        None => {}
      }
      Ok(autoDelFile)
    })
    .map_ok(|mut autoDelFile| {
      Arc::get_mut(&mut autoDelFile).map(|file| file.close());
      FormField {
        name: field_name,
        realName: baseName.to_string(), 
        value: FormValue::File(autoDelFile),
      }
    })
    .await
}

async fn readMultipartFileToMemory<T>(
  s: T,
  name: String,
  uploadfileName: String,
  fileTypes: &'static [&'static str],
) -> Result<FormField>
where
  T: TryStreamExt,
  T::Ok: Buf,
  T::Error: std::error::Error,
{
  let path = Path::new(&uploadfileName);
  if path
    .extension()
    .and_then(|ext| {
      let extLower = ext.to_str()?.to_ascii_lowercase();
      fileTypes.iter().find(|&&ftype| ftype.to_ascii_lowercase() == extLower)
    })
    .is_none()
  {
    return Err(anyhow!("invalid filename {}", uploadfileName));
  }

  s.map_err(|e| anyhow!("{}", e.to_string()))
    .try_fold(BytesMut::new(), |mut buf, data| async move {
      buf.put(data);
      Ok(buf)
    })
    .map_ok(|buf| FormField {
      name,
      realName: String::new(),
      value: FormValue::FileContent((buf.freeze(), uploadfileName)),
    })
    .await
}

async fn upload<T, V>(
  user: User,
  uri: warp::path::FullPath,
  form_data: T,
  fileTypes: &'static [&'static str],
  fileToMemory: bool,
  spfileupload: bool,
) -> Result<FormData, Rejection>
where
  T: Stream<Item = std::io::Result<formdata::Part<V>>> + Unpin,
  V: Stream<Item = std::io::Result<Bytes>> + Unpin,
{
  match form_data
    .map_err(|e| anyhow!("{}", e))
    .and_then(|part| async {
      let name = part.name().to_string();

      match part.filename().map(|v| v.to_owned()) {
        Some(fileName) if fileToMemory => {
          match part.content_type_valid() {
            Ok(_) => {
              readMultipartFileToMemory(part, name, fileName, fileTypes).await
            },
            Err(e) => {
              error!("upload invalid content_type {}", e.to_string());
              Err(anyhow!("invalid Content-Type"))
            }
          }
        },
        Some(fileName) => {
          match part.content_type_valid() {
            Ok(_) => {
              readMultipartFile(part, name, fileName, fileTypes, spfileupload).await
            },
            Err(e) => {
              error!("upload invalid content_type {}", e.to_string());
              Err(anyhow!("invalid Content-Type"))
            }
          }
        },
        None => {
          readMultipartDate(part, name).await
        }
      }
    })
    .try_collect::<Vec<FormField>>()
    .await
  {
    Ok(fields) => Ok::<_, Rejection>(FormData::new(fields, user, uri)),
    Err(e) => Ok(FormData::error(e.to_string(), user, uri)),
  }
}

pub fn raw_body_multipart(
  auth: impl Filter<Extract = (User,), Error = Rejection> + Clone,
  limit: u64,
  fileTypes: &'static [&'static str],
  fileToMemory: bool,
  spfileupload: bool,
) -> impl Filter<Extract = (FormData,), Error = Rejection> + Clone {
  auth
    .and(warp::path::full())
    .and(formdata::form(limit))
    .and_then(move |user, uri, form_data| async move { 
      upload(user, uri, form_data, fileTypes, fileToMemory, spfileupload).await 
    })
}

fn process_body_multipart<Fut>(
  auth: impl Filter<Extract = (User,), Error = Rejection> + Clone,
  limit: u64,
  fileTypes: &'static [&'static str],
  upload_handle: fn((FormData,)) -> Fut,
  fileToMemory: bool,
  spfileupload: bool,
) -> impl Filter<Extract = (JsonValue,), Error = Rejection> + Clone
where
  Fut: Future<Output = Result<Reply<JsonValue>>> + Send,
{
  raw_body_multipart(auth, limit, fileTypes, fileToMemory, spfileupload).and_then(move |mut formdata: FormData| async move {
    formdata.allowFileTypes = Some(fileTypes);
    match upload_handle((formdata.clone(),)).await {
      Ok(reply) if reply.isOk() => {
        formdata.release();
        Ok(reply.data().map(|v| v.clone()).unwrap_or(json!({})))
      }
      Ok(reply) => Err(warp::reject::custom(ResponseError(reply.response()))),
      Err(e) => {
        error!("upload file faile: {}", e.to_string());
        Err(warp::reject::custom(InternalError(format!("{}", e))))
      }
    }
  })
}

pub fn body_multipart<Fut>(
  auth: impl Filter<Extract = (User,), Error = Rejection> + Clone,
  limit: u64,
  fileTypes: &'static [&'static str],
  upload_handle: fn((FormData,)) -> Fut,
) -> impl Filter<Extract = (JsonValue,), Error = Rejection> + Clone
where
  Fut: Future<Output = Result<Reply<JsonValue>>> + Send,
{
  process_body_multipart(auth, limit, fileTypes, upload_handle, false, false)
}

pub fn body_multipart_save_file_in_memory<Fut>(
  auth: impl Filter<Extract = (User,), Error = Rejection> + Clone,
  limit: u64,
  fileTypes: &'static [&'static str],
  upload_handle: fn((FormData,)) -> Fut,
) -> impl Filter<Extract = (JsonValue,), Error = Rejection> + Clone
where
  Fut: Future<Output = Result<Reply<JsonValue>>> + Send,
{
  process_body_multipart(auth, limit, fileTypes, upload_handle, true, false)
}

pub fn body_multipart_save_file_sp<Fut>(
  auth: impl Filter<Extract = (User,), Error = Rejection> + Clone,
  limit: u64,
  fileTypes: &'static [&'static str],
  upload_handle: fn((FormData,)) -> Fut,
) -> impl Filter<Extract = (JsonValue,), Error = Rejection> + Clone
where
  Fut: Future<Output = Result<Reply<JsonValue>>> + Send,
{
  process_body_multipart(auth, limit, fileTypes, upload_handle, false, true)
}