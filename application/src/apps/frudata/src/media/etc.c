

#include <sys/file.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"
#include "media/etc.h"


gint32 frudev_write_to_file(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return SYN_OPEN_FILE_FAIL;
    }

    gchar file_name[FILE_NAME_BUFFER_SIZE] = {0};
    (void)dal_get_property_value_string(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, file_name, sizeof(file_name));

    if (strlen(file_name) == 0) {
        debug_log(DLOG_ERROR, "filename is NULL");
        return SYN_OPEN_FILE_FAIL;
    }

    FILE *fp = dal_fopen_check_realpath(file_name, "w", file_name);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "[%s] fopen check real path failed", __FUNCTION__);
        return SYN_OPEN_FILE_FAIL;
    }

    (void)fchmod(fileno(fp), S_IRUSR | S_IWUSR);

    size_t fwrite_back = fwrite(fru_file, FRU_AREA_MAX_LEN + sizeof(FRU_FILE_HEADER_S), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "write file %s failed.", file_name);
        (void)fclose(fp);
        return SYN_WRITE_E2P_FAIL;
    }

    (void)fflush(fp);
    (void)fsync(fileno(fp));
    (void)fclose(fp);
    return RET_OK;
}

gint32 frudev_read_from_file(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    gchar file_name[64] = {0};
    (void)dal_get_property_value_string(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, file_name, sizeof(file_name));

    if (strlen(file_name) == 0) {
        debug_log(DLOG_ERROR, "filename is NULL");
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    if (dal_check_realpath_before_open(file_name) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: path is invalid", __FUNCTION__);
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    gint32 fd = open(file_name, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        debug_log(DLOG_ERROR, "open file failed, %s.", g_strerror(errno));
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    gint32 read_len = read(fd, fru_file, FRU_AREA_MAX_LEN + sizeof(FRU_FILE_HEADER_S));
    if (read_len < 0) {
        debug_log(DLOG_ERROR, "read file failed, %s.", g_strerror(errno));
        (void)close(fd);
        return ERROR_CODE_FILE_WRITE_FAIL;
    }

    (void)close(fd);
    return RET_OK;
}

gint32 frudev_read_file_withoffset(OBJ_HANDLE object_handle, guint32 offset, gpointer read_buf, guint32 length)
{
    if (read_buf == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter", __FUNCTION__);
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }
    gchar file_name[64] = {0};
    (void)dal_get_property_value_string(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, file_name, sizeof(file_name));

    if (strlen(file_name) == 0) {
        debug_log(DLOG_ERROR, "[%s] filename is NULL", __FUNCTION__);
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    FILE* fp = dal_fopen_check_realpath(file_name, "r", file_name);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "[%s] fopen check real path failed", __FUNCTION__);
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    gint32 handle = fileno(fp);
    (void)fchmod(handle, S_IRUSR | S_IWUSR);
    (void)flock(handle, LOCK_EX);
    (void)fseek(fp, offset, SEEK_SET);

    size_t fread_back = fread(read_buf, length, 1, fp);
    (void)flock(handle, LOCK_UN);
    (void)fclose(fp);
    if (fread_back != 1) {
        debug_log(DLOG_ERROR, "[%s] read file %s failed.", __FUNCTION__, file_name);
        return ERROR_CODE_FILE_READ_FAIL;
    }
    return RET_OK;
}

gint32 frudev_write_file_withoffset(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf, guint32 length)
{
    gchar file_name[64] = {0};
    (void)dal_get_property_value_string(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, file_name, sizeof(file_name));
    if (strlen(file_name) == 0) {
        debug_log(DLOG_ERROR, "[%s] filename is NULL", __FUNCTION__);
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    FILE *fp = NULL;
    if (vos_get_file_accessible(file_name) == FALSE) {
        fp = dal_fopen_check_realpath(file_name, "w+", file_name);
    } else {
        fp = dal_fopen_check_realpath(file_name, "r+", file_name);
    }
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "[%s] fopen check real path failed", __FUNCTION__);
        return ERROR_CODE_CANNOT_OPEN_FILE;
    }

    gint32 handle = fileno(fp);
    (void)fchmod(handle, S_IRUSR | S_IWUSR);
    (void)flock(handle, LOCK_EX);
    (void)fseek(fp, offset, SEEK_SET);

    size_t fwrite_back = fwrite(write_buf, length, 1, fp);
    (void)flock(handle, LOCK_UN);
    (void)fclose(fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "[%s] write file %s failed.", __FUNCTION__, file_name);
        return ERROR_CODE_FILE_WRITE_FAIL;
    }
    return RET_OK;
}