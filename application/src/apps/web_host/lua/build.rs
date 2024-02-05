use std::io;
use std::env;
use std::path::Path;

fn main() -> io::Result<()> {
  let dir = env::var("LIBRARY_PATH").unwrap();
  println!("cargo:rerun-if-changed=build.rs");
  println!("cargo:rustc-link-search=native={}", Path::new(&dir).display());
  println!("cargo:rustc-link-lib=luajit");
  Ok(())
}
