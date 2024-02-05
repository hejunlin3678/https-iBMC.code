# 封装 libdflib.so 库给 lua 访问，并简化属性访问逻辑

## build

```sh
git clone ssh://git@isource-dg.huawei.com:2222/z00559457/ldflib.git
cd ldflib
git submodule init
git submodule update

# 编译 linux 版本
make linux

# 编译 arm 32 位 linux 版本
make arm-linux

# LUA_INCLUDE_PATH 环境变量指定 lua 头文件路径，默认为 3rd/lua53。
# 可以手动指定为其他版本 lua，比如 luajit。
make LUA_INCLUDE_PATH=/usr/local/LuaJIT-2.0.5/src linux

# 默认不编译 3rd/lua53，如果需要编译可以设置 WITH_LUA=1。
make WITH_LUA=1 linux
```

具体编译参数请查看 platform.mk 文件

## test

``` sh
➜  ldflib git:(master) ✗ make WITH_LUA=1 linux
➜  ldflib git:(master) ✗ ./3rd/lua53/lua test/test.lua
............loop 10000 times: pormise 0.097 s, raw 0.051447 s
......
Ran 18 tests in 0.156 seconds, 18 successes, 0 failures
OK
```

## C API

### dflib.so：封装部分 libdflib.so 函数到 lua 的 c 库

| lua 函数 | 返回值 | 说明 |
| -- | -- | -- |
| init(module_name) | bool | dfl_init |
| get_object_handle(object_name) | (DFL错误码, handle) | dfl_get_object_handle |
| get_object_handles(class_name) | (DFL错误码, handle 数组) | dfl_get_object_list |
| get_object_handle_nth(class_name, n) | (DFL错误码, handle) | 获取第n个类对象 |
| get_property_value(handle, property_name) | (DFL错误码, 属性值) | dfl_get_property_value |
| get_object_name(handle) | 对象名 | dfl_get_object_name |
| get_class_name(handle) | (DFL错误码, 对象类名) | dfl_get_class_name |
| get_object_count(class_name) | (DFL错误码, 对象总数) | dfl_get_object_count |
| get_timestamp() | 整数 | vos_get_cur_time_stamp |
| get_timestamp_offset() | 整数 | 获取时区 | |
| get_datetime() | 字符串 | 格式化后的当前时间 |
| gettime() | double | 获取当前毫秒时间，测试用 |
| get_referenced_property(handle, property_name) | (DFL错误码, 引用对象名, 引用属性名) | dfl_get_referenced_property |
| get_referenced_object(handle, property_name) | (DFL错误码, 引用对象 handle) | dfl_get_referenced_object |
| set_property_value(handle, property_name, gvariant_val, opt) | DFL错误码 | dfl_set_property_value |
| call_class_method(handle, method_name, user_data_list, input_list) | (DFL错误码, gvariant 数组) | dfl_call_class_method |
| call_remote_class_method2_async(ip, port, obj_name, method_name, user_data_list, input_list) | (DFL错误码, gvariant 数组) | dfl_call_remote_class_method2 |
| GVariant | 返回 GVariant 库 | 导出 glib2 GVariant 部分功能到 lua |
| EVENT_WORK_STATE_S() | 返回 EVENT_WORK_STATE_S 位域对象 | 导出 EVENT_WORK_STATE_S 位域到 lua |
| SENSOR_READINGMASK_BIT | 返回 SENSOR_READINGMASK_BIT 位域对象 | 导出 SENSOR_READINGMASK_BIT 位域到 lua |
| SENSOR_CAPABILITY_S | 返回 SENSOR_CAPABILITY_S 位域对象 | 导出 SENSOR_CAPABILITY_S 位域到 lua |

```lua
local c = require "dflib.core" -- 导入 dflib.so c 库

local ok, handle = c.get_object_handle("EthGroup0")
lu.assertEquals(ok, DFL_OK)
local ok, prop = c.get_property_value(handle, "IpAddr")
lu.assertEquals(ok, DFL_OK)
```

### GVariant 相关函数

| lua 函数 | 对应 glib2 函数 |
| -- | -- |
| GVariant.new_byte | g_variant_new_byte |
| GVariant.new_int16 | g_variant_new_int16 |
| GVariant.new_uint16 | g_variant_new_uint16 |
| GVariant.new_int32 | g_variant_new_int32 |
| GVariant.new_uint32 | g_variant_new_uint32 |
| GVariant.new_double | g_variant_new_double |
| GVariant.new_string | g_variant_new_string |
| GVariant.new_vstring | g_variant_new_vstring |
| GVariant.new_bool | g_variant_new_boolean |

所有这些函数返回 gvariant 类对象，包含两个类方法：
type 方法获取象类型
value: 方法获取对象的值

```lua
local c = require "dflib.core" -- 导入 dflib.so c 库
local GVariant = c.GVariant

local v = GVariant.new_byte(1)
lu.assertEquals(v:type(), "y")
lu.assertEquals(v:value(), 1)

local v = GVariant.new_double(1.13)
lu.assertEquals(v:type(), "d")
lu.assertEquals(v:value(), 1.13)

local v = GVariant.new_string("abc")
lu.assertEquals(v:type(), "s")
lu.assertEquals(v:value(), "abc")
```

---

### LUA API

#### 用 LUA 对 C API 再做一次封装

#### dflib.class

- dflib.class(class_name)
  - 创建 dflib.class，代表指定类名的所有 DFL 对象
- dflib.class(class_name, 属性名, 属性值)
  - 创建 dflib.class，代表指定类名，指定属性为指定值的 DFL 对象
- dflib.class.class_name()
  - 简写，等同于 dflib.class(class_name)
- dflib.class.class_name(属性名, 属性值)
  - 简写，等同于 dflib.class(class_name, 属性名, 属性值)
- dflib.class.class_name[n]
  - 创建 dflib.class，代表指定类名的第n个 DFL 对象

  ```lua
  local dflib = require "dflib"
  local C = dflib.class

  local AllEthGroup1 = C("EthGroup")
  local AllEthGroup2 = C.EthGroup() -- 简写
  lu.assertEquals(AllEthGroup1.__handles, AllEthGroup2.__handles)

  local SpecificEthGroup1 = C("EthGroup", "NetMode", 1)
  local SpecificEthGroup2 = C.EthGroup("NetMode", 1) -- 简写
  lu.assertEquals(SpecificEthGroup1.__handles, SpecificEthGroup2.__handles)

  local FirstEthGroup = C.EthGroup[0] -- 代表 EthGroup 类的第 0 个 DFL 对象的类
  lu.assertEquals(#FirstEthGroup.__handles, 1)
  lu.assertEquals(FirstEthGroup.__handles[1], AllEthGroup1.__handles[1])
  ```

##### dflib.class 方法

- fold(callback, init_acc)
  - 说明：遍历满足条件的所有对象
  - 参数：
    - callback：function(obj, acc)
      - 说明：回调函数，每个满足条件的对象回调一次
      - 参数：
        - obj：dflib.object
        - acc：迭代参数，第一次为 fold 函数的 init_acc 值，之后为前一次回调的返回值
      - 返回值：
        - 下一次迭代的输入参数
    - init_acc：迭代参数初始值
  - 返回值：
    - 用 promise 包装的最后一次迭代返回值

  ```lua
  local dflib = require "dflib"
  local C = dflib.class

  -- 获取 EthGroup 类所有对象的对象名
  local EthGroup = C.EthGroup()
  local p_obj_names = EthGroup:fold(function(obj, acc)
    table.insert(acc, dflib.object_name(obj))
    return acc
  end, {})

  -- p_obj_names 是对象名数组的 promise 包装，需要 fetch 才能拿到结果
  local obj_names = p_obj_names:fetch()
  for _, v in pairs(obj_names) do print(v) end
  ```

- next(callback)
  - 说明：回调满足条件的第一个对象
  - 参数：
    - callback：function(obj)
      - obj：dflib.object
  - 返回值：
    - 用 promise 包装的回调函数返回值

  ```lua
  local dflib = require "dflib"
  local C = dflib.class

  -- 获取 EthGroup 类第一个对象的对象名
  local EthGroup = C.EthGroup()
  local p_obj_name = EthGroup:next(function(obj)
    return dflib.object_name(obj)
  end)

  -- p_obj_name 是回调返回值的 promise 包装，需要 fetch 才能拿到结果
  local obj_name = p_obj_name:fetch()
  print(obj_name)
  ```

---

#### dflib.object

- dflib.object(ObjectName)
  - 创建 dflib.object，代表指定对象名的 DFL 对象
- dflib.object.ObjectName
  - 简写，等同于 dflib.object(ObjectName)

  ```lua
  local dflib = require "dflib"
  local O = dflib.object

  -- 获取对象名为 EthGroup0 的对象
  local Obj1_EthGroup0 = O("EthGroup0")
  local Obj2_EthGroup0 = O.EthGroup0
  lu.assertEquals(Obj1_EthGroup0.__handle, Obj2_EthGroup0.__handle)
  ```

##### dflib.object 方法

- next(callback)
  - 说明：调用回调函数并将返回值用 promise 包装后返回
  - 参数：
    - callback：function(obj)
      - obj：当前 dflib.object 自己
  - 返回值：
    - 用 promise 包装的回调函数返回值

  ```lua
  local dflib = require "dflib"
  local O = dflib.object

  local Obj1_EthGroup0 = O("EthGroup0")
  local p_obj_name = Obj1_EthGroup0:next(function(obj)
    return dflib.object_name(obj)
  end)

  -- p_obj_name 是回调返回值的 promise 包装，需要 fetch 才能拿到结果
  local obj_name = p_obj_name:fetch()
  print(obj_name)
  ```

- ref(prop_name)
  - 说明：获取该 DFL 对象指定属性的引用属性信息
  - 参数：
    - prop_name：属性名称
  - 返回值：
    - 用 promise 包装的引用对象信息 {obj_name = 引用对象名, prop_name = 引用属性名}

  ```lua
  local c = require "dflib.core"
  local dflib = require "dflib"
  local O = dflib.object

  local ref_info = O.Object3:ref("Property1"):fetch()

  local ok, handle = c.get_object_handle("Object3")
  lu.assertEquals(ok, DFL_OK)
  local ok, obj_name1, prop_name1 = c.get_referenced_property(handle, "Property1")
  lu.assertEquals(ok, DFL_OK)
  lu.assertEquals(ref_info, {obj_name = obj_name1, prop_name = prop_name1})
  ```

- ref_obj(prop_name)
  - 说明：获取该 DFL 对象指定属性的引用对象
  - 参数：
    - prop_name：属性名称
  - 返回值：
    - 用 promise 包装的引用对象

  ```lua
  local c = require "dflib.core"
  local dflib = require "dflib"
  local O = dflib.object

  local ref_obj = O.Object3:ref_obj("Property2"):fetch()

  local ok, handle = c.get_object_handle("Object3")
  lu.assertEquals(ok, DFL_OK)
  local ok, expected = c.get_referenced_object(handle, "Property2")
  lu.assertEquals(ok, DFL_OK)
  lu.assertEquals(ref_obj.__handle, expected)
  ```

- obj[prop_name]
  - 说明：获取该 DFL 对象指定属性的值
  - 参数：
    - prop_name：属性名称
  - 返回值：
    - 用 promise 包装的属性值

  ```lua
  local c = require "dflib.core"
  local dflib = require "dflib"
  local O = dflib.object

  local obj2 = O.Object2
  local prop = obj2["Property1"]

  local ok, expected = c.get_property_value(obj2.__handle, "Property1")
  lu.assertEquals(ok, DFL_OK)
  lu.assertEquals(prop:fetch(), expected)

  local prop_1 = obj2.Property1 -- 简写
  lu.assertEquals(prop_1:fetch(), expected)
  ```

- obj(method_name, user_data_list, input_list)
  - 说明：调用对象方法
  - 参数：
    - method_name：方法名称
    - user_data_list：用户数据数组，数组每一项必须是 GVariant 对象
    - input_list：输入数据数组，数组每一项必须是 GVariant 对象
  - 返回值：
    数组，具体意义看不同方法的定义

  ```lua
  local dflib = require "dflib"
  local O = dflib.object

  local obj = O.Object2
  local ok, ret = obj:call("Method1", nil, {GVariant.new_byte(1), GVariant.new_byte(2), GVariant.new_byte(3)})
  lu.assertEquals(ok, DFL_OK);
  for k, v in ipairs(ret) do print(k, v) end
  ```

---

## example

### 根据对象名获取属性

```lua
function TestGetObjectHandleNth()
  -- 获取对象名为 EthGroup0 的对象 IpAddr 属性值
  local value = O.EthGroup0.IpAddr:fetch()

  -- 直接调用底层接口访问
  local ok, handle = c.get_object_handle("EthGroup0")
  lu.assertEquals(ok, DFL_OK)
  local ok, prop = c.get_property_value(handle, "IpAddr")
  lu.assertEquals(ok, DFL_OK)
  local expected = prop:value()

  lu.assertEquals(value, expected)
end
```

### 指定类名和对象索引获取属性

```lua
function TestGetObjectHandleNth()
  -- 获取类名为 EthGroup 的第 0 个对象的 IpAddr 属性值
  local value = C.EthGroup[0].IpAddr:fetch()

  -- 直接调用底层接口访问
  local ok, handle = c.get_object_handle_nth("EthGroup", 0)
  lu.assertEquals(ok, DFL_OK)
  local ok, prop = c.get_property_value(handle, "IpAddr")
  lu.assertEquals(ok, DFL_OK)
  local expected = prop:value()

  lu.assertEquals(value, expected)
end
```

### 指定类名和对象给定属性的值访问对象属性

```lua
function TestGetObjectHandleSearch()
  -- 获取类名为 EthGroup 并且 NetMode 属性等于 1 的对象的 IpAddr 属性值
  local value = C.EthGroup("NetMode", 1).IpAddr:fetch()
  
  -- 直接调用底层接口访问
  local handle
  local ok, handles = c.get_object_handles("EthGroup")
  lu.assertEquals(ok, DFL_OK)
  for _, h in ipairs(handles) do
    local ok, prop = c.get_property_value(h, "NetMode")
    lu.assertEquals(ok, DFL_OK)
    if ok == DFL_OK and prop:value() == 1 then
      handle = h
      break
    end
  end
  local ok, prop = c.get_property_value(handle, "IpAddr")
  lu.assertEquals(ok, DFL_OK)
  local expected = prop:value()

  lu.assertEquals(value, expected)
end
```

### 属性访问异常处理

```lua
  -- 链式调用嵌套函数内部产生的异常也可以被外部 catch 捕获
  O.Object2.Property2:next(function()
    return O.UnknowClass.Property1:fetch() == 1
  end):catch(1)
```
