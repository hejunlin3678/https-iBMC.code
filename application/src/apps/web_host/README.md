# bmc web 服务

- portal：web_host 主程序。
- core：http 路由、http 接口转发到 lua、http 上传、http 下载等功能。
- ldflib：libdflib.so 库 lua 封装。
- lua：rust 封装的 lua 虚拟机。
- lualib：lua 业务代码。
  - redfish_handles：实现 redfish http 接口。
  - webrest_handles：实现 webrest http 接口。
- redfish：redfish 服务路由定义。
- webrest：webrest 服务路由定义。

## Build

构建参数在 platform.mk 中定义。

### x86

```sh
make linux
```

### arm32

```sh
# Ubuntu 下安装 gcc-arm-linux-gnueabi 交叉编译环境
sudo apt-get install gcc-arm-linux-gnueabi

# 安装 rust 交叉编译环境
rustup target add armv7-unknown-linux-gnueabi

make arm-linux

# 或者指定构建参数
make CC="arm32-linux-gnueabi-gcc" \
     STRIP="arm32-linux-gnueabi-strip" \
     CROSS="arm32-linux-gnueabi-" \
     DFLIB_PATH=${G_WORK_DIR}/../../sdk/PME/V100R001C10/lib \
     arm-linux
```

## Test

```sh
make linux
make test
```
