

#ifndef __JSON_PATCH_UTIL_H__
#define __JSON_PATCH_UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "json_object_private.h"
#include "printbuf.h"
#include "glib.h"
#include "pme_app/common/public_redfish.h"

#ifdef _cplusplus
extern "C" {
#endif

int redfish_object_object_to_json_string(struct json_object *jso, struct printbuf *pb, int level, int flags);
json_bool get_property_obj_from_segments(json_object *obj, char *segment_info[], int segment_num,
    json_object **property_jso);

extern json_bool json_object_has_key(json_object *jso, const char *json_key);
void json_object_object_del_ex(json_object *jso, const char *json_pointer_path);
json_bool ex_json_object_object_get_keyval(json_object *jso, const gchar **key, json_object **val);
gint ex_json_object_array_get_object_item_nth(json_object *jso_array, const char *item_name);

extern void free_property_extra_info(json_object *jso, void *userdata);

void json_object_set_user_data(struct json_object *jso, void *user_data, json_object_delete_fn *user_delete);

extern json_bool json_object_get_user_data(struct json_object *jso, void **user_data);

extern void json_object_array_clean(json_object *jso);

void ex_json_object_join(json_object *jso_to, json_object *jso_from);

const char *ex_json_object_object_get_key(json_object *jso);

struct json_object *ex_json_object_object_get_val(json_object *jso);


extern json_bool is_json_object_array_element_exist(json_object *array_jso, json_object *element_jso);


extern int json_object_array_element_occur_number(json_object *jso, json_object *element_jso);


extern json_bool json_object_array_delete_idx(json_object *jso, int idx);

extern json_bool get_element_int(json_object *jso, const char *element_name, gint32 *int_value);

extern json_bool get_element_boolean(json_object *jso, const char *element_name, json_bool *bool_value);

extern void standardize_req_json_data(json_object *pattern_jso, json_object *req_jso);

extern json_bool json_object_object_case_get(struct json_object *jso, const char *case_key, const char **key,
    struct json_object **value);

extern int json_object_key_index(json_object *jso, const char *name);

extern json_bool json_object_object_add_rdf(struct json_object *obj, const char *objectkey, struct json_object *val);

extern void json_object_patch(json_object *origin_jso, json_object *patch_body_jso);

extern json_object *jso_object_object_dup(json_object *jso);

extern void json_object_set_object(json_object *jso, json_object *jso_data);

extern gint32 json_array_duplicate_check(json_object *jso_array, gint32 *duplicate_index);

extern int ex_case_get_jso_from_path(json_object *jso, const char *prop_path, const char **real_prop_name,
    json_object **prop_jso);

extern json_object *ex_json_object_new_double(double d, gchar *format);

#ifdef _cplusplus
}
#endif 

#endif 
