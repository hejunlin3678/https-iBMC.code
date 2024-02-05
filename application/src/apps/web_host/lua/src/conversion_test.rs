#[cfg(test)]
mod test {
  use crate::state::State;
  use serde_json::{Number as JsonNumber, Value as JsonValue};

  #[test]
  fn TestJsonNumber() {
    let mut L = State::new();
    let top = L.get_top();

    // lua -> json: 1.0 被当做整数
    L.push_number(1.0);
    let val: JsonValue = L.to_type(-1).unwrap();
    assert_eq!(val, JsonValue::Number(JsonNumber::from(1)));
    L.pop(1);
    assert_eq!(L.get_top(), top);

    // lua -> json: -1.0 被当做整数
    L.push_number(-1.0);
    let val: JsonValue = L.to_type(-1).unwrap();
    assert_eq!(val, JsonValue::Number(JsonNumber::from(-1)));
    L.pop(1);
    assert_eq!(L.get_top(), top);

    // lua -> json: 1.1 被当做浮点数
    L.push_number(1.1);
    let val: JsonValue = L.to_type(-1).unwrap();
    assert_eq!(val, JsonValue::Number(JsonNumber::from_f64(1.1).unwrap()));
    L.pop(1);
    assert_eq!(L.get_top(), top);

    // lua -> json: 空表当做数组
    L.new_table().unwrap();
    let val: JsonValue = L.to_type(-1).unwrap();
    assert_eq!(val, JsonValue::Array(vec![]));
    L.pop(1);
    assert_eq!(L.get_top(), top);
  }
}
