#ifndef __MSCM_TAR_H__
#define __MSCM_TAR_H__
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

extern gint32 tar_bzip2(const guchar * pucdirfilename,const guchar * pucoutfilename);
extern gint32 tar_unzip(const guchar * pucdirfilename,const guchar * pucdirname);
extern gint32 tar_gzip(const guchar * pucdirfilename,const guchar * pucoutfilename);
extern gint32 tar_gzip_pack(const guchar *filepath,const  guchar * pucdirfilename,const guchar * pucoutfilename);
#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */
#endif
