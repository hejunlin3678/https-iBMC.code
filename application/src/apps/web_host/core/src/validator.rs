use super::wrap_utils::ErrorInfo;
use bytes::Bytes;
use libc::c_int;
use lua::{Integer, JsonOption, State, ToLua};
use regex::Regex;
use serde_json::Value as JsonValue;
use std::net::{Ipv4Addr, Ipv6Addr};
pub type Result = std::result::Result<(), Error>;
use chrono::Datelike;

#[derive(Debug, Clone)]
pub struct ValidateErrorInfo {
  pub method: &'static str,
  pub value: String,
  pub message: String,
}
to_lua!(ValidateErrorInfo, method, value, message);

#[derive(Debug, Clone)]
pub enum ErrorValue {
  ValidateErr(ValidateErrorInfo),
  ValidateErrs(Vec<Error>),
  ErrInfos(Vec<ErrorInfo>),
}

#[derive(Debug, Clone)]
pub enum ErrorKey {
  Str(String),
  Num(usize),
}

#[derive(Debug, Clone)]
pub struct Error(ErrorKey, ErrorValue);

impl Error {
  pub fn new(method: &'static str, name: &str, value: String, message: String) -> Self {
    Error(
      ErrorKey::Str(name.into()),
      ErrorValue::ValidateErr(ValidateErrorInfo { value, method, message }),
    )
  }

  pub fn new_errs(name: &str, errs: Vec<Error>) -> Self {
    Error(ErrorKey::Str(name.into()), ErrorValue::ValidateErrs(errs))
  }

  pub fn new_errinfos(name: &str, errs: Vec<ErrorInfo>) -> Self {
    Error(ErrorKey::Str(name.into()), ErrorValue::ErrInfos(errs))
  }

  pub fn new_ensure(method: &'static str, name: &str) -> Self {
    Error::new(method, name, String::default(), format!("Ensure {} is valid.", name))
  }
}

impl ToLua for Error {
  fn to_lua(&self, L: &mut State) -> anyhow::Result<()> {
    match self.0 {
      ErrorKey::Str(ref s) => L.push_string(s)?,
      ErrorKey::Num(i) => L.push_integer(i as Integer),
    }
    match &self.1 {
      ErrorValue::ValidateErr(e) => {
        e.to_lua(L)?;
        L.set_table(-3)?;
      }
      ErrorValue::ValidateErrs(es) => {
        L.create_table(0, es.len() as c_int)?;
        for e in es {
          e.to_lua(L)?;
        }
        L.set_table(-3)?;
      }
      ErrorValue::ErrInfos(es) => {
        es.to_lua(L)?;
        L.set_table(-3)?;
      }
    }
    Ok(())
  }
}

impl ToString for Error {
  fn to_string(&self) -> String {
    match &self.1 {
      ErrorValue::ValidateErr(info) => info.message.clone(),
      ErrorValue::ValidateErrs(infos) => infos.iter().map(|i| i.to_string()).collect::<Vec<String>>().join("\n"),
      ErrorValue::ErrInfos(infos) => infos.iter().map(|i| i.to_string()).collect::<Vec<String>>().join("\n"),
    }
  }
}

pub trait OptionValue {
  type Out;
  fn check(&self) -> Option<&Self::Out>;
}

impl<T> OptionValue for Option<T> {
  type Out = T;
  fn check(&self) -> Option<&Self::Out> {
    self.as_ref()
  }
}

impl<T> OptionValue for JsonOption<T> {
  type Out = T;
  fn check(&self) -> Option<&Self::Out> {
    match self {
      JsonOption::Null | JsonOption::None => None,
      JsonOption::Value(ref v) => Some(v),
    }
  }
}

pub trait Validator {
  fn validate(&self) -> std::result::Result<(), Vec<Error>>;
}

impl Validator for JsonValue {
  fn validate(&self) -> std::result::Result<(), Vec<Error>> {
    Ok(())
  }
}

pub fn Range<T>(name: &str, val: &T, min: T, max: T) -> Result
where
  T: std::cmp::Ord + std::fmt::Display,
{
  if *val >= min && *val <= max {
    return Ok(());
  }
  Err(Error::new_ensure("Range", name))
}

pub fn RangeOrNone<T>(name: &str, val: &T, min: T::Out, max: T::Out) -> Result
where
  T: OptionValue,
  T::Out: std::cmp::Ord + std::fmt::Display,
{
  match val.check() {
    Some(v) => Range(name, v, min, max),
    None => Ok(()),
  }
}

pub trait LenLimit {
  fn len(&self) -> usize;
}

impl LenLimit for String {
  fn len(&self) -> usize {
    self.len()
  }
}

impl LenLimit for Bytes {
  fn len(&self) -> usize {
    self.len()
  }
}

impl<T> LenLimit for Vec<T> {
  fn len(&self) -> usize {
    self.len()
  }
}

impl LenLimit for JsonValue {
  fn len(&self) -> usize {
    match self {
      JsonValue::Object(obj) => obj.len(),
      JsonValue::Array(arr) => arr.len(),
      JsonValue::String(s) => s.len(),
      _ => 0,
    }
  }
}

pub fn Length<T: LenLimit>(name: &str, val: &T, min: usize, max: usize) -> Result {
  if val.len() >= min && val.len() <= max {
    return Ok(());
  }
  Err(Error::new_ensure("Length", name))
}

pub fn LengthOrNone<T>(name: &str, val: &T, min: usize, max: usize) -> Result
where
  T: OptionValue,
  T::Out: LenLimit,
{
  match val.check() {
    Some(v) => Length(name, v, min, max),
    None => Ok(()),
  }
}

pub fn InOrNone<T>(name: &str, val: &T, strs: &'static [&'static str]) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    Some(v) => In(name, v, strs),
    None => Ok(()),
  }
}

pub fn In(name: &str, val: &str, strs: &'static [&'static str]) -> Result {
  if strs.contains(&val) {
    return Ok(());
  }
  let mut tmp = String::default();
  for s in strs.iter() {
    if tmp.len() > 10 {
      tmp += &" ...";
      break;
    }
    if tmp.len() > 0 {
      tmp += ", ";
    }
    tmp += s;
  }
  Err(Error::new_ensure("In", name))
}

impl<T: Validator> Validator for Vec<T> {
  fn validate(&self) -> std::result::Result<(), Vec<Error>> {
    let mut errs = vec![];
    for (i, v) in self.iter().enumerate() {
      match v.validate() {
        Ok(()) => {}
        Err(e) => errs.push(Error(ErrorKey::Num(i + 1), ErrorValue::ValidateErrs(e))),
      }
    }

    if errs.is_empty() {
      Ok(())
    } else {
      Err(errs)
    }
  }
}

impl<T: Validator> Validator for Option<T> {
  fn validate(&self) -> std::result::Result<(), Vec<Error>> {
    match self {
      Some(ref v) => v.validate(),
      None => Ok(()),
    }
  }
}

pub fn Unique<T: Eq + std::hash::Hash + ToString>(name: &str, val: &Vec<T>) -> Result {
  let mut uniq = std::collections::HashSet::new();
  if val.iter().all(move |v| uniq.insert(v)) {
    Ok(())
  } else {
    Err(Error::new_ensure("Unique", name))
  }
}

pub fn UniqueOrNone<T, U>(name: &str, val: &T) -> Result
where
  T: OptionValue<Out = Vec<U>>,
  U: Eq + std::hash::Hash + ToString,
{
  match val.check() {
    Some(v) => Unique(name, v),
    None => Ok(()),
  }
}

pub fn AllIn<T: ToString, U: ToString>(name: &str, val: &Vec<T>, arr: &'static [U]) -> Result
where
  T: PartialEq<U>,
  U: PartialEq<T>,
{
  let vv = val
    .iter()
    .filter(|v| arr.iter().all(|x| *x != **v))
    .map(|v| v.to_string())
    .collect::<Vec<String>>();
  if vv.len() > 0 {
    Err(Error::new_ensure("AllIn", name))
  } else {
    Ok(())
  }
}

pub fn AllInOrNone<O, T: ToString, U: ToString>(name: &str, val: &O, arr: &'static [U]) -> Result
where
  O: OptionValue<Out = Vec<T>>,
  T: PartialEq<U>,
  U: PartialEq<T>,
{
  match val.check() {
    Some(v) => AllIn(name, v, arr),
    None => Ok(()),
  }
}

pub fn DateTime(name: &str, val: &String, fmt: &'static str) -> Result {
  match chrono::NaiveDateTime::parse_from_str(&val, fmt) {
    Ok(_) => Ok(()),
    Err(_) => Err(Error::new_ensure("DateTime", name)),
  }
}

pub fn DateTimeOrNone<T>(name: &str, val: &T, fmt: &'static str) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    Some(v) => DateTime(name, v, fmt),
    None => Ok(()),
  }
}

pub fn Ranges<T: ToString>(name: &str, val: &T, ranges: &'static [(T, T)]) -> Result
where
  T: std::cmp::Ord + std::fmt::Display,
{
  for range in ranges {
    if *val >= range.0 && *val <= range.1 {
      return Ok(());
    }
  }
  Err(Error::new_ensure("Ranges", name))
}

pub fn RangesOrNone<O, T>(name: &str, val: &O, ranges: &'static [(T, T)]) -> Result
where
  O: OptionValue<Out = T>,
  T: std::cmp::Ord + std::fmt::Display + ToString,
{
  match val.check() {
    Some(v) => Ranges::<T>(name, v, ranges),
    None => Ok(()),
  }
}

pub fn Regex(name: &str, val: &String, rx: &'static str) -> Result {
  match Regex::new(rx) {
    Err(e) => error!("new regex failed: {}", e),
    Ok(re) if re.is_match(val) => return Ok(()),
    _ => {}
  }

  Err(Error::new_ensure("Regex", name))
}

pub fn RegexOrNone<T>(name: &str, val: &T, rx: &'static str) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    Some(v) => Regex(name, v, rx),
    None => Ok(()),
  }
}

pub fn RegexSet(name: &str, val: &String, rx: &[&'static str]) -> Result {
  for v in rx {
    if Regex(name, val, v).is_ok() {
      return Ok(());
    }
  }

  Err(Error::new_ensure("RegexSet", name))
}

pub fn RegexSetOrNone<T>(name: &str, val: &T, rx: &[&'static str]) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    Some(v) => RegexSet(name, v, rx),
    None => Ok(()),
  }
}

pub fn CheckRegexOrEmpty<T>(name: &str, val: &T, rx: &[&'static str]) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    None => Ok(()),
    Some(v) => match v.len() {
      0 => Ok(()),
      _ => RegexSet(name, v, rx),
    },
  }
}

pub fn CheckIpv4(name: &str, val: &String) -> Result {
  if val.parse::<Ipv4Addr>().is_ok() {
    return Ok(());
  }

  Err(Error::new_ensure("CheckIpv4", name))
}

pub fn CheckIpv4OrNone<T>(name: &str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    None => Ok(()),
    Some(v) => CheckIpv4(name, v),
  }
}

//两种需要trim的场景：
//1).[::ffff:192.1.56.10] 会将"["和"]"去掉
//2).::ffff:192.1.56.10/98 会将"/数字"去掉
fn trim_ipv6<'a>(name: &str, s: &'a str) -> std::result::Result<&'a str, Error> {
  let v = s.trim_matches(|c| c == '[' || c == ']');
  match v.rfind("/") {
    Some(idx) => match v[idx + 1..].parse::<u32>() {
      Ok(_) => Ok(&v[..idx]),
      Err(_) => Err(Error::new_ensure("CheckIpv6", name)),
    },
    None => Ok(v),
  }
}

pub fn CheckIpv6(name: &str, val: &String) -> Result {
  if trim_ipv6(name, val)?.parse::<Ipv6Addr>().is_ok() {
    return Ok(());
  }

  Err(Error::new_ensure("CheckIpv6", name))
}

pub fn CheckIpv6OrNone<T>(name: &str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  match val.check() {
    None => Ok(()),
    Some(v) => CheckIpv6(name, v),
  }
}

pub fn CheckServerAddressOrNone<T>(name: &str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  //check domain
  if CheckRegexOrEmpty(name, val, &[
    r"^(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63}))(\.(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63})))+$"
  ]).is_ok() {
    return Ok(());
  }

  if CheckIpv4OrNone(name, val).is_ok() {
    return Ok(());
  }

  return CheckIpv6OrNone(name, val);
}

pub fn CheckIPAddressOrNone<T>(name: &str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  if CheckIpv4OrNone(name, val).is_ok() {
    return Ok(());
  }

  return CheckIpv6OrNone(name, val);
}

pub fn CheckDNSAddressOrNone<T>(name: &str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  //check domain
  if CheckRegexOrEmpty(name, val, &[
    r"^(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63}))(\.(([a-zA-Z0-9][-a-zA-Z0-9]{0,61}[a-zA-Z0-9])|([a-zA-Z0-9]{1,63})))+\.?$"
  ]).is_ok() {
    return Ok(());
  }

  if CheckIpv4OrNone(name, val).is_ok() {
    return Ok(());
  }

  return CheckIpv6OrNone(name, val);
}

// 检查时间段范围
pub fn CheckDateRange(name: &str, val: &String, fmt: &'static str, minDay: i32, maxDay: i32) -> Result {
    match chrono::NaiveDate::parse_from_str(val, fmt) {
      Ok(d) if d.year() >= minDay && d.year() <= maxDay => Ok(()),
    _ => Err(Error::new_ensure("CheckDateRange", name)),
  }
}

// 检查时间段范围(允许 None)
pub fn CheckDateRangeOrNone(
  name: &str,
  val: &Option<String>,
  fmt: &'static str,
  minDay: i32,
  maxDay: i32,
) -> Result {
  match val {
    Some(ref v) => CheckDateRange(name, v, fmt, minDay, maxDay),
    None => Ok(()),
  }
}

// 检查整数型字符串是否在合法范围内
pub fn CheckStringRange<T>(name: &str, val: &String, min: T, max: T) -> Result
where
  T: std::cmp::Ord + std::fmt::Display + std::str::FromStr,
{
  match val.parse::<T>() {
    Ok(v) if v >= min && v <= max => return Ok(()),
    _ => Err(Error::new_ensure("CheckStringRange", name)),
  }
}

pub fn CheckStringRangeOrNone<T>(name: &str, val: &Option<String>, min: T, max: T) -> Result
where
  T: std::cmp::Ord + std::fmt::Display + std::str::FromStr,
{
  match val.check() {
    None => Ok(()),
    Some(v) => CheckStringRange(name, v, min, max),
  }
}

pub fn CheckBase64OrNone<T>(name: &str, val: &T) -> Result
where
  T: OptionValue<Out = String>,
{
  //check base64
  if CheckRegexOrEmpty(name, val, &[r"^[a-zA-Z0-9+/=]*$"]).is_ok() {
    return Ok(());
  }
  Err(Error::new_ensure("CheckBase64", name))
}