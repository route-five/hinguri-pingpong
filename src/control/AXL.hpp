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

#include "AXHS.hpp"
// #include "AXA.h"
//#include "AXD.h"
//#include "AXM.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

    DWORD   __stdcall AxlOpen(long lIrqNo);
    DWORD   __stdcall AxlOpenNoReset(long lIrqNo);
    BOOL    __stdcall AxlClose();
    BOOL    __stdcall AxlIsOpened();
    DWORD   __stdcall AxlInterruptEnable();
    DWORD   __stdcall AxlInterruptDisable();
//==========
    DWORD   __stdcall AxlGetBoardCount(long *lpBoardCount);
    DWORD   __stdcall AxlGetLibVersion(char *szVersion);
    DWORD   __stdcall AxlGetModuleNodeStatus(long lBoardNo, long lModulePos);
    DWORD   __stdcall AxlGetBoardStatus(long lBoardNo);
    DWORD __stdcall AxlGetLockMode(long lBoardNo, WORD* wpLockMode);
	DWORD   __stdcall AxlGetReturnCodeInfo(DWORD dwReturnCode, long lReturnInfoSize, long* lpRecivedSize, char* szReturnInfo);
//==========
    DWORD   __stdcall AxlSetLogLevel(DWORD uLevel);
    DWORD   __stdcall AxlGetLogLevel(DWORD *upLevel);
//==========
    DWORD   __stdcall AxlScanStart(long lBoardNo, long lNet);
    DWORD   __stdcall AxlBoardConnect(long lBoardNo, long lNet);
    DWORD   __stdcall AxlBoardDisconnect(long lBoardNo, long lNet);
//==========
    DWORD   __stdcall AxlScanStartSIIIH(SCAN_RESULT* pScanResult = NULL);
//========== Fan Control ===============================================================================
    DWORD   __stdcall AxlReadFanSpeed(long lBoardNo, double *dpFanSpeed);
//========== EzSpy =================================================================================
	DWORD   __stdcall AxlEzSpyUserLog(char* szUserLog);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXL_H__
