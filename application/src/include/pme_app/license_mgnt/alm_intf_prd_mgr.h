/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: ��Ʒ�����������ɾ���Լ���ѯ��Ʒ״̬�ȡ�
 * Author: AdaptiveLM team
 * Create: 2015-07-08
 */
#ifndef ALM_INTF_PRD_MGR_H
#define ALM_INTF_PRD_MGR_H

#include "alm_intf_def.h"
#include "alm_intf_lic_mgr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#if defined(ALM_PACK_4)
#pragma pack(4)
#elif defined(ALM_PACK_8)
#pragma pack(8)
#endif

/*
 * ����: ��Ӳ�Ʒ��ͬʱ����BBOM���־û������ļ��л��¼
 * ��Ʒ��ż��汾������Ӳ�Ʒ�ı�Ŵ����ڳ־��ļ��У�
 * �Ұ汾����ͬʱ�����汾������������������á�
 * Ϊ���ٳ־û��ļ���д�����������һ������Ӷ����Ʒ��
 * ��ӵĲ�Ʒ����Ŀ���������־û����õ�����Ʒ��Ŀ��
 * ����:
 * pstPrds - �������Ӳ�Ʒ�������Ʒ��ӽ����
 * uiPrdNum - ������Ӳ�Ʒ����
 * puiFailNum - ���ٸ���Ʒδ�ܳɹ���ӵ��ڴ��С�
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrAdd(INOUT ALM_PRD_STRU *pstPrds, IN ALM_UINT32 uiPrdNum, OUT ALM_UINT32 *puiFailNum);

/*
 * ����: ����Licenseʱ����Licene�汾�Ͳ�Ʒ�汾ƥ�䣬���Ҳ�Ʒ��˫��Կ���Ƿ����������㷨ǩ����License
 * Ĭ�����������Ե��øú�������ֹ��
 * ��ǰ�����㷨�ǣ�RSA1024_SHA1��RSA2048_SHA1��ECC163_SHA1��ECC233_SHA1��
 * ��ǰ��ǿ�㷨�ǣ�RSA3072_SHA256��ECC283_SHA256��
 * ע�⣺���㷨�᲻�ϵ����ӣ���һ��ʱ��󣬵�ǰ��ǿ�㷨Ҳ�������㷨�����㷨�ο���Ʒ��ȫ����3.0Ҫ��
 * ���øú���ʱ����Ҫ�ȺͲ�ƷSEȷ�ϣ�(1)�Ƿ��б�Ҫ��(2)��ƷLicense��ǩ���㷨��ʲô��
 * ����:
 * iPrdId - ���õĲ�Ʒid��
 * bAllow - �Ƿ�����ֻ�ܴ�ALM_TRUE��ALM_FALSE������ֵ��
 * ALM_TRUE������Ĭ��Ҳ������ģ�
 * ALM_FALSE����������Ҫ���ء�
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrAllowVerMatWeakAlgLic(IN ALM_INT32 iPrdId, IN ALM_BOOL bAllow);

/* �Ƴ�����Ӳ�Ʒ����������á� */
LIC_EXPORT ALM_INT32 ALM_PrdMgrDel(IN ALM_INT32 iPrdId);

/* �Ƴ��־û��ڴ��Ʒ��Ϣ,û��ִ��ɾ����Ʒ֮ǰ,  ����ִ��ɾ����Ʒ�ı�����Ϣ */
LIC_EXPORT ALM_INT32 ALM_PrdMgrPsDel(IN ALM_INT32 iPrdId);

/*
 * ����: �ָ������Ѽ��ص�License����������Stick�ȡ�
 * ��û��ִ�лָ����������������������Stick�򼤻�License�ļ���Ϊ�ָ����̡�
 * ��:����������������������֮ǰ״̬����(��ο���ǰʱ�估��������ˢ��)��
 * ����������������̡���Ӳ�Ʒ����ɽ�����Stick��������ú���á�
 * ���ѻָ��Ĳ�Ʒ�ٴε��ò���Ӱ�졣
 * ����:
 * piPrdId - ָ����Ҫ�ָ��Ĳ�Ʒ����ΪNULL�ָ�ȫ����Ʒ��
 * pstLic - ָ����ҪĬ�ϼ��ص�License�ļ�����ΪNULL�����ء�
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrRecover(IN CONST ALM_INT32 *piPrdId, INOUT ALM_LIC_ACTIVE_STRU *pstLics);

/* ʹ��Ʒ��Ĭ��ֵ���� */
LIC_EXPORT ALM_INT32 ALM_PrdMgrDisable(IN ALM_INT32 iPrdId);

/* ��ѯָ��ID�Ĳ�Ʒ���ƺͰ汾��Ϣ����չ��Ϣ */
LIC_EXPORT ALM_INT32 ALM_PrdMgrQuery(IN CONST ALM_INT32 iPrdId, OUT ALM_PRD_BASE_STRU *pstPrdInfo,
                                     INOUT ALM_PRD_EXTEND_INFO_STRU *pstPrdExtend);

/**
 * @ingroup AdaptiveLM
 * @brief �����Ƿ�������Ӳ�Ʒ�������Կ��ϢУ��ʧ��ʱ��ʹ�õڶ�KeyFile��У�顣
 *
 * @attention �ù���Ĭ�ϲ���������Ҫ�����ù��ܣ���ͨ����API��ALM_TRUE����API��Ҫ��ALM_Start֮����á�
 *
 * @li Memory operation: alloc��free��size:
 * -# ���ڴ����</br>
 * @li Thread safe:
 * -# ���̰߳�ȫ</br>
 * @li OS/CPU difference:
 * -# ��OS����</br>
 * @li Time consuming:
 * -# �Ǻ�ʱ�ӿ�</br>
 *
 * @param bTurnOn [IN] ��������#ALM_BOOL�����ÿ�������
 *
 * @retval #ALM_OK 0 �ɹ���
 * @retval #��������ֵ ʧ�� - ��Ӧ���������API�ֲᡣ
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrLoadSecondKeySwitch(IN ALM_BOOL bTurnOn);

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* ALM_INTF_PRD_MGR_H */
