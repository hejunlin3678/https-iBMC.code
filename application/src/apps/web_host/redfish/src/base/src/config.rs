use anyhow::Result;
use once_cell::sync::OnceCell;
use std::path::PathBuf;

static CONFIG_DIR: OnceCell<String> = OnceCell::new();

pub fn config_init(dir: String) -> Result<()> {
  info!("config redfish dir");
  let _ = CONFIG_DIR.set(dir);
  Ok(())
}

pub fn get_template_file_path(path: &str) -> Option<PathBuf> {
  if let Some(dir) = CONFIG_DIR.get() {
    let mut buf = PathBuf::new();
    buf.push(dir);
    buf.push(path);
    buf.push("index.json");
    Some(buf)
  } else {
    None
  }
}

pub fn get_registries_file_path() -> Option<PathBuf> {
  if let Some(dir) = CONFIG_DIR.get() {
    let mut buf = PathBuf::new();
    buf.push(dir);
    buf.push("redfish/v1/registrystore/messages/en");
    Some(buf)
  } else {
    None
  }
}

pub fn get_schema_file_path(class_name: &str) -> Option<PathBuf> {
  if let Some(dir) = CONFIG_DIR.get() {
    let mut buf = PathBuf::new();
    buf.push(dir);
    buf.push("redfish/v1/schemastore/en");
    buf.push(class_name.to_ascii_lowercase() + ".json");
    Some(buf)
  } else {
    None
  }
}

pub fn get_action_schema_file_path() -> Option<PathBuf> {
  if let Some(dir) = CONFIG_DIR.get() {
    let mut buf = PathBuf::new();
    buf.push(dir);
    buf.push("redfish/v1/redfish_template/actions-schema.json");
    Some(buf)
  } else {
    None
  }
}
