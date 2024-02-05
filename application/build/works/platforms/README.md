组件统一放在该目录下维护，每种组件一个文件夹。
software.xml，表示从cmc获取固件方式的配置，内容来源于cmc的“配置xml"
srcbaseline.xml，表示从源码下载编译的方式，内容来源于cmc的“配置xml”
参考各自组件的构建指导书，编译3个脚本：
prebuild.sh，源码下载
build.sh，编译
postbuild.sh，打包到目标目录