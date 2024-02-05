


#ifndef __REDFISH_CLASS_H__
#define __REDFISH_CLASS_H__

#include <json.h>
#include "redfish_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define MAX_CLASS_NAME_LEN 256
#define MAX_VALUE_NAME_LEN 256
#define MAX_ARRAY_INDEX_LEN 16

#define SCHEMA_FILE_SUFFIX ".json"

#define REF_REGEX_LOCAL_FILE "^#/definitions/(.+)"
#define REF_REGEX_OTHER_FILE ".+/(.+)\\.json#/definitions/(.+)"


#define ACTION_SCHEMA_PREFIX "actions-schema"
#define JSON_SCHMEA_OPTIONAL_FLAG "optionalPara"

#define REGEX_PATTERN_URL "(?:https?://)?(?:[\\w]+.)([a-zA-Z.]{2,6})([\\w.-]*)*/?"
// 匹配 dateTime 的正则表达式 格式参见 http://www.w3.org/TR/xmlschema-2/#dateTime
#define REGEX_PATTERN_DATETIME                                                                                         \
    "(\\d{4})-(((0[13578]|1[02])-(0[1-9]|[12][0-9]|3[01]))|((0[469]|11)-(0[1-9]|[12][0-9]|30))|(02-(0[1-9]|[1][0-"     \
        "9]|2[0-9])))T(([0-1][0-9])|([2][0-3])):([0-5][0-9]):([0-5][0-9])(.[0-9]+)?(Z|(([-+])([0][0-9]|[1][0-2]):([0-" \
        "5][0-9])))?"




#define JSON_SCHEMA_FILE_PATH \
    REDFISH_PATH_DEFINE("/conf/redfish/v1/schemastore/en/") // ("/redfish_cfg_dir/json-schema/")
#define ACTION_SCHEMA_PATH REDFISH_PATH_DEFINE("/conf/redfish/v1/redfish_template/actions-schema.json")

typedef struct tag_wrong_prop_info {
    
    char *prop_name;

    
    char *json_pointer_path;
} wrong_prop_info_s;


extern json_bool redfish_post_format_check(const gchar *redfish_class, const gchar *jso_body_str,
    const gchar *action_name, 
    json_object **jso_body_parsed, json_object *jso_err_message);



extern json_bool redfish_patch_format_check(const gchar *redfish_class, const gchar *jso_body_str,
    json_object **jso_body_parsed, json_object *jso_extend_arr);



extern void redfish_class_dump(const char *dump_path);

extern json_object *get_class_schema(const char *class_name);

extern void format_rsc_json(json_object *pattern_jso, json_object *rsc_jso);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 
