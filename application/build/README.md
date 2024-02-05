# 开源软件源码中心仓切换指导手册

## 修改内容

一共涉及两个部分的修改：
- 开源软件下载方式改变
- 构建工程适配

### 开源软件下载方式

#### 新增配置文件 opensource_manifest.xml

该配置文件用于记录`开源软件的名称`，`开源软件的版本`，`开源软件的源码仓库地址`，以及`开源软件在工程中的目录存放地址`。

该配置文件的存放目录如下：
```
${iBMC_ROOT}/application/build/dependency_for_hpm/downxml/opensource_manifest.xml
```

#### 新增源码下载脚本 download_from_codehub.sh

该脚本用于解析`opensource_manifest.xml`并下载对应开源软件，脚本位于
```
${iBMC_ROOT}/application/build/download_from_codehub.sh
```


#### 如何新增开源软件
需要新增开源软件时，添加`<project/>`条目即可，如新增libjpeg
```
<?xml version="1.0" encoding="utf-8"?>
<manifest>
  <remote name="code-hub" fetch="https://e_B89vsjtcvxMH9xzKoY:e_B89vsjtcvxMH9xzKoY@codehub-y.huawei.com/" />
  <default remote="code-hub" revision="master"/>

  <project path="open_source/libjpeg/libjpeg" name="OpenSourceCenter/libjpeg.git" revision="refs/tags/9d" />
</manifest>
```

然后删除原始配置文件中的相关条目，即`downxml/dependency_opensourcedown.xml`中相关条目，如：
```
--- a/application/build/dependency_for_hpm/downxml/dependency_opensourcedown.xml
+++ b/application/build/dependency_for_hpm/downxml/dependency_opensourcedown.xml
@@ -527,23 +527,6 @@
                 </copy>
             </copies>
         </dependency>
-        <dependency>
-            <versionType>Opensource</versionType>
-            <repoType>Generic</repoType>
-            <id>
-                <artifactName>jpegsrc.v9d.tar.gz</artifactName>
-                <pdmCode>05833VRC</pdmCode>
-                <dependType></dependType>
-            </id>
-            <copies>
-                <copy>
-                    <source></source>
-                    <dest>../open_source/libjpeg/</dest>
-                    <parameter></parameter>
-                    <clear></clear>
-                </copy>
-            </copies>
-        </dependency>
         <dependency>
             <versionType>Opensource</versionType>
             <repoType>Generic</repoType>
```

#### 如何使用下载脚本
已经集成到了当前下载使用的`download_from_cmc.py`脚本中。

因此通常情况下，不需要修改下载脚本，开源软件的修改在`opensource_manifest.xml`配置文件中进行。

```
--- a/application/build/download_from_cmc.py
+++ b/application/build/download_from_cmc.py
@@ -170,6 +170,13 @@ def download_linux_kernel():

     logging.info('download linux kernel end!')

+def download_open_source_from_codehub():
+    logging.info('download open_source from codehub begin...')
+
+    subprocess.run(f"sh download_from_codehub.sh", shell=True)
+
+    logging.info('download open_source from codehub end!')
+

 def download_npm_open_source():
     logging.info('download npm open_source begin...')
@@ -220,6 +227,7 @@ if __name__ == "__main__":
     if args.npm:
         download_npm_open_source()
     else:
+        download_open_source_from_codehub()
         download_opensource()
         download_dependency()
         download_linux_kernel()
```

### 构建工程适配

以libjpeg为例，删除解压缩、打补丁等动作。如下：
```
$ git diff open_source/build.sh
diff --git a/open_source/build.sh b/open_source/build.sh
index c7f612082c..1b50a39967 100755
--- a/open_source/build.sh
+++ b/open_source/build.sh
@@ -24,7 +24,6 @@ SASL=cyrus-sasl-2.1.27
 SQLITE=sqlite-autoconf-3320300
 LINUX_PAM=Linux-PAM-1.4.0
 LIBEDIT=libedit-20191231-3.1
-LIBJPEG=jpegsrc.v9d
 LIBSSH2=libssh2-1.9.0
 NTP=ntp-4.2.8p15
 SYSLOG_NG=syslog-ng-3.36.1
@@ -191,8 +190,6 @@ function sqlite() {
 # libjpeg
 function libjpeg() {
     cd ${G_WORK_DIR}/libjpeg
-    rm -rf jpeg-9d
-    tar -zxvf $LIBJPEG.tar.gz
-    mv jpeg-9d/* .
+    mv libjpeg/* .
     cp ${G_WORK_DIR}/global.sh .
     open_source_do jpeg
```
