/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  对象私有成员的API
 * Author:       lianjin
 * Notes:        new_obj_priv_data 和 delete_obj_priv_data 成对出现。get_obj_priv_data 和 put_obj_priv_data 成对出现。   
 * History:      2021/2/1 创建
 */

#ifndef __DAL_OBJ_PRIV_DATA_H__
#define __DAL_OBJ_PRIV_DATA_H__

// 在模块初始化时调用一次。
void init_obj_priv_data_lock(void);

/* 
 * 对象创建时使用一次。
 * 私有数据的使用者需保证使用私有数据时的线程安全，包括new_obj_priv_data，get_obj_priv_data 和 put_obj_priv_data。
 * 同一个对象只能绑定一个私有数据，后绑定的数据会替换先绑定的，且后绑定的数据的数据结构不能改变。
 * 同时，如果提供了析构函数，重新绑定时会自动调用替换析构函数释放原来的内存。
 */
gint32 new_obj_priv_data(OBJ_HANDLE obj_handle, gpointer user_data, GDestroyNotify destroy_func);

/*
 * 当私有数据不存在时（如对象被销毁），函数返回RET_ERR，且(*user_data_ptr)置为NULL。
 */
gint32 get_obj_priv_data(OBJ_HANDLE obj_handle, gpointer *user_data_ptr);

/*
 * 当私有数据的引用计数为 0 时，（调用析构函数）释放私有数据内存。
 */
void put_obj_priv_data(OBJ_HANDLE obj_handle, gpointer user_data);

/*
 * 对象销毁时使用一次。
 */
void delete_obj_priv_data(OBJ_HANDLE obj_handle);

#endif // __DAL_OBJ_PRIV_DATA_H__