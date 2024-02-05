#[macro_export]
macro_rules! struct_param {
  (() -> {($($head:tt)+) ($($body:tt)*) ()}) => {
  #[derive(Clone)] $($head)+ { $($body)* }
  };
  (($id:ident: $ty:ty, $($next:tt)*) -> {($($head:tt)+) ($($body:tt)*) ($($meta:tt)*)}) => {
    struct_param!(($($next)*) -> {($($head)+) ($($body)* $($meta)* pub $id: $ty,) ()});
  };
  ((#[validate $($e:tt)*] $(#[$nfa:meta])* $id:ident:$ty:ty,$($next:tt)*) -> {($($head:tt)+) ($($body:tt)*) ($($meta:tt)*)}) => {
    struct_param!(($(#[$nfa])* $id: $ty, $($next)*) -> {($($head)+) ($($body)*) ($($meta)*)});
  };
  ((#[$fa:meta] $(#[$nfa:meta])* $id:ident: $ty:ty, $($next:tt)*) -> {($($head:tt)+) ($($body:tt)*) ($($meta:tt)*)}) => {
    struct_param!(($(#[$nfa])* $id: $ty, $($next)*) -> {($($head)+) ($($body)*) ($($meta)* #[$fa])});
  };
}

#[macro_export]
macro_rules! validate_param {
  ($my:ident, $errs:ident, ()) => {
    if $errs.len() == 0 {
      Ok(())
    } else {
      Err($errs[..1].to_vec())
    }
  };
  ($my:ident, $errs:ident, ($id:ident: $ty:ty, $($next:tt)*)) => {{
    validate_param!($my, $errs, ($($next)*))
  }};
  ($my:ident, $errs:ident, (#[validate] $(#[$nfa:meta])* $id:ident: $ty:ty, $($next:tt)*)) => {{
    if let Err(e) = $my.$id.validate() {
      $errs.push(core::validator::Error::new_errs(&stringify!($id), e));
    }
    validate_param!($my, $errs, ($(#[$nfa])* $id: $ty, $($next)*))
  }};
  ($my:ident, $errs:ident, (#[validate($($f:ident($($p:expr),*)),+)] $(#[$nfa:meta])* $id:ident: $ty:ty, $($next:tt)*)) => {{
    $(
      if let Err(e) = $f(&stringify!($id), &$my.$id, $($p),*) {
        $errs.push(e);
      }
    )+
    validate_param!($my, $errs, ($(#[$nfa])* $id: $ty, $($next)*))
  }};
  ($my:ident, $errs:ident, (#[$fa:meta] $(#[$nfa:meta])* $id:ident: $ty:ty, $($next:tt)*)) => {{
    validate_param!($my, $errs, ($(#[$nfa])* $id: $ty, $($next)*))
  }};
}

#[macro_export]
macro_rules! tolua_param {
  ($L:ident $my:ident ()) => {
    Ok(())
  };
  ($L:ident $my:ident ($(#[$fa:meta])* $id:ident: $ty:ty, $($next:tt)*)) => {{
    $L.push_string(&stringify!($id))?;
    $my.$id.to_lua($L)?;
    $L.set_table(-3)?;
    tolua_param!($L $my ($($next)*))
  }};
}

#[macro_export]
macro_rules! fromlua_param {
  ($L:ident $index:ident $name:ident () ($($body:tt)*)) => {
    Ok($name { $($body)* })
  };
  ($L:ident $index:ident $name:ident ($(#[$fa:meta])* $id:ident: $ty:ty, $($next:tt)*) ($($body:tt)*)) => {{
    $L.push_string(&stringify!($id));
    $L.get_table($index);
    let $id : $ty = match $L.to_type(-1) {
      Ok(v) => v,
      Err(e) => return Err(crate::anyhow::anyhow!("get field {}.{} failed: {}", &stringify!($name), &stringify!($id), e))
    };
    $L.pop(1);
    fromlua_param!($L $index $name ($($next)*) ($($body)* $id: $id,))
  }};
}

#[macro_export]
macro_rules! define_param {
  (@portal (validate $($flag:ident)*) ($($nfa:tt)*) ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    impl core::Validator for $name {
      fn validate(&self) -> std::result::Result<(), Vec<core::validator::Error>> {
        let mut errs = vec![];
        validate_param!(self, errs, ($($input)*))
      }
    }
    define_param!(@portal ($($flag)*) ($($nfa)*) ($name) ($($head)+) -> {$($input)*} );
  };
  (@portal (ToLua $($flag:ident)*) ($($nfa:tt)*) ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    impl lua::ToLua for $name {
      fn to_lua(&self, L: &mut crate::lua::State) -> crate::anyhow::Result<()> {
        L.new_table()?;
        tolua_param!(L self ($($input)*))
      }
    }
    define_param!(@portal ($($flag)*) ($($nfa)*) ($name) ($($head)+) -> {$($input)*} );
  };
  (@portal (FromLua $($flag:ident)*) ($($nfa:tt)*) ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    impl lua::FromLua for $name {
      fn from_lua(L: &mut crate::lua::State, index: crate::lua::Index) -> crate::anyhow::Result<$name> {
        if !L.is_table(index) {
          return Err(crate::anyhow::anyhow!("expect {}, got {}", stringify!($name), L.typename_of(L.type_of(index)?)));
        }
        let abs_index = L.absindex(index);
        fromlua_param!(L abs_index $name ($($input)*) ())
      }
    }
    define_param!(@portal ($($flag)*) ($($nfa)*) ($name) ($($head)+) -> {$($input)*} );
  };
  (@portal (ToString $($flag:ident)*) ($($nfa:tt)*) ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    impl ToString for $name {
      fn to_string(&self) -> String {
        match serde_json::to_string(&self) {
          Ok(s) => s,
          Err(e) => e.to_string(),
        }
      }
    }
    define_param!(@portal ($($flag)*) ($($nfa)*) ($name) ($($head)+) -> {$($input)*} );
  };
  (@portal () ($($nfa:tt)*) ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    define_param!(@meta ($($nfa)*) ($name) ($($head)+) -> {$($input)*} );
  };

  (@meta (portal($($t:ident),*) $($nfa:tt)*) ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    define_param!(@portal ($($t)*) ($($nfa)*) ($name) ($($head)+) -> {$($input)*});
  };
  (@meta ($fa:meta $($nfa:tt)*) ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    define_param!(@meta ($($nfa)*) ($name) (#[$fa] $($head)+) -> {$($input)*});
  };
  (@meta () ($name:ident) ($($head:tt)+) -> {$($input:tt)*}) => {
    struct_param!(($($input)*) -> {($($head)+) () ()});
  };

  ($(#[$($a:tt)+])* struct $name:ident { $($input:tt)*} ) => {
    define_param!(@meta ($($($a)+)*) ($name) (struct $name) -> { $($input)*});
  };
  ($(#[$($a:tt)+])* pub struct $name:ident { $($input:tt)*} ) => {
    define_param!(@meta ($($($a)+)*) ($name) (pub struct $name) -> { $($input)*});
  }
}

// routes macro

#[macro_export]
macro_rules! and {
  (, $f:expr $(, $next:expr)*) => {{
    let filter = $f;
    $(let filter = filter.and($next);)*
    filter
  }};
}

#[macro_export]
macro_rules! _case_insensitive_path {
  ($($pieces:tt)*) => ({
    __case_insensitive_path!(@start $($pieces)*)
  });
}

#[macro_export]
macro_rules! __case_insensitive_path {
  (@start ..) => ({
    compile_error!("'..' cannot be the only segment")
  });
  (@start $first:tt $(/ $tail:tt)*) => ({
      __case_insensitive_path!(@munch warp::any(); [$first] [$(/ $tail)*])
  });

  (@munch $sum:expr; [$cur:tt] [/ $next:tt $(/ $tail:tt)*]) => ({
      __case_insensitive_path!(@munch and!(, $sum, __case_insensitive_path!(@segment $cur)); [$next] [$(/ $tail)*])
  });
  (@munch $sum:expr; [$cur:tt] []) => ({
      __case_insensitive_path!(@last $sum; $cur)
  });

  (@last $sum:expr; ..) => (
      $sum
  );
  (@last $sum:expr; $end:tt) => (
    and!(,
      and!(, $sum, __case_insensitive_path!(@segment $end)),
      warp::path::end()
    )
  );

  (@segment ..) => (
      compile_error!("'..' must be the last segment")
  );
  (@segment $param:ty) => (
      warp::path::param::<$param>()
  );
  // Constructs a unique ZST so the &'static str pointer doesn't need to
  // be carried around.
  (@segment $s:literal) => ({
      #[derive(Clone, Copy)]
      struct __StaticPath;
      impl crate::core::PathSegmentTrait for __StaticPath {
          fn segment() -> &'static str {
              static S: &str = $s;
              S
          }
      }
      crate::core::case_insensitive_path::<__StaticPath>()
  });
}

#[macro_export]
macro_rules! __path {
  ($($p:tt)+) => {_case_insensitive_path!($($p)+)};
  () => {warp::path::end()};
}

#[macro_export]
macro_rules! __method {
  (GET) => {
    warp::get()
  };
  (PUT) => {
    warp::put()
  };
  (POST) => {
    warp::post()
  };
  (DELETE) => {
    warp::delete()
  };
  (PATCH) => {
    warp::patch()
  };
}

#[macro_export]
macro_rules! routes {
  ($($method:ident)? $([$($p:tt)*])? => {$($b:tt)+} $(; $($nb:tt)+)?) => {{
    let filter = and!($(, __method!($method))? $(, __path!($($p)*))?);
    let filter = filter.and(routes!($($b)+));
    $(let filter = filter.or(routes!($($nb)+)).unify();)?
    filter.boxed()
  }};
  ($($method:ident)? $([$($p:tt)*])? => $(($($aa:tt)+))? $($(.)? $t:ident($f:expr) $(. $with:ident($($withf:expr)?))*)? $(; $($nb:tt)+)?) => {{
    let filter = and!($(, __method!($method))? $(, __path!($($p)*))?);
    $(let filter = routes!(@ll filter, $($aa)+);)?
    $(let filter = filter.$t($f) $(. $with($($withf)?))*;)?
    $(let filter = filter.or(routes!($($nb)+)).unify();)?
    filter.boxed()
  }};

  (@ll $filter:ident, @ $aa:expr $(, $($bb: tt)+)?) => {{
    let filter = $aa.with(core::tuple($filter).and_then()).untuple_one();
    $(let filter = routes!(@ll filter, $($bb)+);)?
    filter
  }};
  (@ll $filter:ident, $aa:expr $(, $($bb: tt)+)?) => {{
    let filter = $filter.and($aa);
    $(let filter = routes!(@ll filter, $($bb)+);)?
    filter
  }};
}
