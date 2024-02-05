#[cfg(test)]
mod test {
  use crate::datas::LoginRule;
  use core::Validator;
  use serde_json::json;

  #[test]
  fn TestLoginRuleTimeValidate() {
    const tests: &[(&'static str, bool)] = &[
      ("12:00", true),
      ("2019-01-01", true),
      ("2019-01-01 12:00", true),
      ("2051-01-01 12:00", false),
      ("1969-01-01 12:00", false),
      ("201-01-01", false),
      ("2019-1-01", false),
      ("2019-01-1", false),
      ("2019-01-01 2:00", false),
      ("2019-01-01 12:1", false),
      ("2019-01-01 12:00:01", false),
    ];
    for test in tests {
      let input = json!({"ID": 0, "StartTime": test.0});
      let d = serde_json::from_value::<LoginRule>(input);
      assert_eq!(d.is_ok(), true);
      assert_eq!(d.unwrap().validate().is_ok(), test.1);
    }
  }

  #[test]
  fn TestLoginRuleIpValidate() {
    const tests: &[(&'static str, bool)] = &[
      ("127.0.0.1", true),
      ("127.0.0.1/99", true),
      ("127.0", false),
      ("a.b.c.d", false),
      ("127.0.0.1/100", false),
      ("127.0.0.1/", false),
      ("127.0.0.1/a", false),
      ("127.1111.0.1", false),
    ];
    for test in tests {
      let input = json!({"ID": 0, "IP": test.0});
      let d = serde_json::from_value::<LoginRule>(input);
      assert_eq!(d.is_ok(), true);
      assert_eq!(d.unwrap().validate().is_ok(), test.1);
    }
  }

  #[test]
  fn TestLoginRuleMacValidate() {
    const tests: &[(&'static str, bool)] = &[
      ("50:5a:ac", true),
      ("50:5a:ac:ed:5d:d4", true),
      ("50:5a:ac:ed:5d:dg", false),
      ("50:5a:ac:ed:5d:2", false),
      ("50:5a:ac:", false),
      ("50:5a", false),
    ];
    for test in tests {
      let input = json!({"ID": 0, "Mac": test.0});
      let d = serde_json::from_value::<LoginRule>(input);
      assert_eq!(d.is_ok(), true);
      assert_eq!(d.unwrap().validate().is_ok(), test.1);
    }
  }
}
