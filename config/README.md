### buildimg 操作配置文件指导：
1、基于每个板子各有一个配置文件 img.json
2、对应目录 ：
- 			src_tree = V2R2_trunk
- 			build_tree = V2R2_trunk/temp

3、标签的操作顺序为：dos2unix -> mkdir -> cp -> rm -> mv -> touch -> sed -> ln -> chown -> chmod
4、特殊说明：
- 			之前的 rtos_with_driver/apps 目录已不存在 现对应目录为 rtos_with_driver/rootfs/opt/pme
- 			大家修改权限时注意下先后顺序 以及 应该直接在现有基础上更改 而不是直接在后面新增 
- 			目前公共部分的配置 只能每个板子配一遍 后续会进行公共配置抽取