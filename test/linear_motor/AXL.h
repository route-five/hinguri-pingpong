/****************************************************************************
*****************************************************************************
**
** File Name
** ----------
**
** AXL.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Library Header File
** 
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
** 
** (None)
**
**
*****************************************************************************
*****************************************************************************
**
** Website
** ---------------------
**
** http://www.ajinextek.com
**
*****************************************************************************
*****************************************************************************
*/

#ifndef __AXT_AXL_H__
#define __AXT_AXL_H__

#include "AXHS.h"
// #include "AXA.h"
//#include "AXD.h"
//#include "AXM.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//========== ���̺귯�� �ʱ�ȭ =========================================================================

    // ���̺귯�� �ʱ�ȭ
    DWORD   __stdcall AxlOpen(long lIrqNo);

    // ���̺귯�� �ʱ�ȭ�� �ϵ���� Ĩ�� ������ ���� ����.
    DWORD   __stdcall AxlOpenNoReset(long lIrqNo);

    // ���̺귯�� ����� ����
    BOOL    __stdcall AxlClose();

    // ���̺귯���� �ʱ�ȭ �Ǿ� �ִ� �� Ȯ��
    BOOL    __stdcall AxlIsOpened();

    // ���ͷ�Ʈ�� ����Ѵ�.
    DWORD   __stdcall AxlInterruptEnable();

    // ���ͷ�Ʈ�� �����Ѵ�.
    DWORD   __stdcall AxlInterruptDisable();

//========== ���̺귯�� �� ���̽� ���� ���� ============================================================
    // ��ϵ� ���̽� ������ ���� Ȯ��
    DWORD   __stdcall AxlGetBoardCount(long *lpBoardCount);
    // ���̺귯�� ���� Ȯ��, szVersion[64]
    DWORD   __stdcall AxlGetLibVersion(char *szVersion);

    // Network��ǰ�� �� ��⺰ ������¸� Ȯ���ϴ� �Լ�
    DWORD   __stdcall AxlGetModuleNodeStatus(long lBoardNo, long lModulePos);

    // �ش� ���尡 ���� ������ �������� ��ȯ�Ѵ�.
    DWORD   __stdcall AxlGetBoardStatus(long lBoardNo);

    // Network ��ǰ�� Configuration Lock ���¸� ��ȯ�Ѵ�.
    // *wpLockMode  : DISABLE(0), ENABLE(1)
    DWORD __stdcall AxlGetLockMode(long lBoardNo, WORD* wpLockMode);

	// ��ȯ���� ���� ������ ��ȯ�Ѵ�.
	DWORD   __stdcall AxlGetReturnCodeInfo(DWORD dwReturnCode, long lReturnInfoSize, long* lpRecivedSize, char* szReturnInfo);

//========== �α� ���� =================================================================================
    // EzSpy�� ����� �޽��� ���� ����
    // uLevel : 0 - 3 ����
    // LEVEL_NONE(0)    : ��� �޽����� ������� �ʴ´�.
    // LEVEL_ERROR(1)   : ������ �߻��� �޽����� ����Ѵ�.
    // LEVEL_RUNSTOP(2) : ��ǿ��� Run / Stop ���� �޽����� ����Ѵ�.
    // LEVEL_FUNCTION(3): ��� �޽����� ����Ѵ�.
    DWORD   __stdcall AxlSetLogLevel(DWORD uLevel);
    // EzSpy�� ����� �޽��� ���� Ȯ��
    DWORD   __stdcall AxlGetLogLevel(DWORD *upLevel);

//========== MLIII =================================================================================
    // Network��ǰ�� �� ����� �˻��� �����ϴ� �Լ�
    DWORD   __stdcall AxlScanStart(long lBoardNo, long lNet);
    // Network��ǰ �� ������ ��� ����� connect�ϴ� �Լ�
    DWORD   __stdcall AxlBoardConnect(long lBoardNo, long lNet);
    // Network��ǰ �� ������ ��� ����� Disconnect�ϴ� �Լ�
    DWORD   __stdcall AxlBoardDisconnect(long lBoardNo, long lNet);

//========== SIIIH =================================================================================
    // SIIIH ������ ���忡 ����� ��⿡ ���� �˻��� �����ϴ� �Լ�(SIIIH ������ ���� ����)
    DWORD   __stdcall AxlScanStartSIIIH(SCAN_RESULT* pScanResult = NULL);

//========== Fan Control ===============================================================================
    // ���忡 �����Ǿ� �ִ� Fan�� �ӵ�(rpm)�� Ȯ���Ѵ�.
    DWORD   __stdcall AxlReadFanSpeed(long lBoardNo, double *dpFanSpeed);

//========== EzSpy =================================================================================
	// EzSpy User Log(Max length : 200 Bytes)
	DWORD   __stdcall AxlEzSpyUserLog(char* szUserLog);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXL_H__
