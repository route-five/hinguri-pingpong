#ifndef __AXT_AXDEV_H__
#define __AXT_AXDEV_H__

#include "source/header/AXHD.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

    // Board Number�� �̿��Ͽ� Board Address ã��
    DWORD    __stdcall AxlGetBoardAddress(long lBoardNo, DWORD *upBoardAddress);
    // Board Number�� �̿��Ͽ� Board ID ã��
    DWORD    __stdcall AxlGetBoardID(long lBoardNo, DWORD *upBoardID);
    // Board Number�� �̿��Ͽ� Board Version ã��
    DWORD    __stdcall AxlGetBoardVersion(long lBoardNo, DWORD *upBoardVersion);
    // Board Number�� Module Position�� �̿��Ͽ� Module ID ã��
    DWORD    __stdcall AxlGetModuleID(long lBoardNo, long lModulePos, DWORD *upModuleID);
    // Board Number�� Module Position�� �̿��Ͽ� Module Version ã��
    DWORD    __stdcall AxlGetModuleVersion(long lBoardNo, long lModulePos, DWORD *upModuleVersion);
    // Board Number�� Module Position�� �̿��Ͽ� Network Node ���� Ȯ��
    DWORD    __stdcall AxlGetModuleNodeInfo(long lBoardNo, long lModulePos, long *upNetNo, DWORD *upNodeAddr);

    // Board�� ����� ���� Data Flash Write (PCI-R1604[RTEX master board]����)
    // lPageAddr(0 ~ 199)
    // lByteNum(1 ~ 120)
    // ����) Flash�� ����Ÿ�� ������ ���� ���� �ð�(�ִ� 17mSec)�� �ҿ�Ǳ⶧���� ���� ����� ���� �ð��� �ʿ���.
    DWORD    __stdcall AxlSetDataFlash(long lBoardNo, long lPageAddr, long lBytesNum, BYTE *bpSetData);

    // Board�� ����� ESTOP �ܺ� �Է� ��ȣ�� �̿��� InterLock ��� ��� ���� �� ������ ���� ����� ���� (PCI-Rxx00[MLIII master board]����)
    // 1. ��� ����
    //	  ����: ��� ��� ������ �ܺο��� ESTOP ��ȣ �ΰ��� ���忡 ����� ��� ���� ��忡 ���ؼ� ESTOP ���� ���� ����	
    //    0: ��� ������� ����(�⺻ ������)
    //    1: ��� ���
    // 2. ������ ���� ��
    //      �Է� ���� ��� ���� ���� 1 ~ 40, ���� ��� Cyclic time
	// Board �� dwInterLock, dwDigFilterVal�� �̿��Ͽ� EstopInterLock ��� ����
	DWORD    __stdcall AxlSetEStopInterLock(long lBoardNo, DWORD dwInterLock, DWORD dwDigFilterVal);
	// Board�� ������ dwInterLock, dwDigFilterVal ������ ��������
	DWORD    __stdcall AxlGetEStopInterLock(long lBoardNo, DWORD *dwInterLock, DWORD  *dwDigFilterVal);
	// Board�� �Էµ� EstopInterLock ��ȣ�� �д´�.
	DWORD    __stdcall AxlReadEStopInterLock(long lBoardNo, DWORD *dwInterLock);
	
    // Board�� ����� ���� Data Flash Read(PCI-R1604[RTEX master board]����)
    // lPageAddr(0 ~ 199)
    // lByteNum(1 ~ 120)
    DWORD    __stdcall AxlGetDataFlash(long lBoardNo, long lPageAddr, long lBytesNum, BYTE *bpGetData);

    // Board Number�� Module Position�� �̿��Ͽ� AIO Module Number ã��
    DWORD    __stdcall AxaInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    // Board Number�� Module Position�� �̿��Ͽ� DIO Module Number ã��
    DWORD    __stdcall AxdInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);

    // ���� �࿡ IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommand(long lAxisNo, IPCOMMAND sCommand);
    // ���� �࿡ 8bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData08(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // ���� �࿡ 8bit IPCOMMAND ��������
    DWORD    __stdcall AxmGetCommandData08(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // ���� �࿡ 16bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData16(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // ���� �࿡ 16bit IPCOMMAND ��������
    DWORD    __stdcall AxmGetCommandData16(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // ���� �࿡ 24bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData24(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // ���� �࿡ 24bit IPCOMMAND ��������
    DWORD    __stdcall AxmGetCommandData24(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // ���� �࿡ 32bit IPCOMMAND Setting
    DWORD    __stdcall AxmSetCommandData32(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // ���� �࿡ 32bit IPCOMMAND ��������
    DWORD    __stdcall AxmGetCommandData32(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);

    // ���� �࿡ QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandQi(long lAxisNo, QICOMMAND sCommand);
    // ���� �࿡ 8bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData08Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // ���� �࿡ 8bit QICOMMAND ��������
    DWORD    __stdcall AxmGetCommandData08Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // ���� �࿡ 16bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData16Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // ���� �࿡ 16bit QICOMMAND ��������
    DWORD    __stdcall AxmGetCommandData16Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // ���� �࿡ 24bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData24Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // ���� �࿡ 24bit QICOMMAND ��������
    DWORD    __stdcall AxmGetCommandData24Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // ���� �࿡ 32bit QICOMMAND Setting
    DWORD    __stdcall AxmSetCommandData32Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // ���� �࿡ 32bit QICOMMAND ��������
    DWORD    __stdcall AxmGetCommandData32Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);

    // ���� �࿡ Port Data �������� - IP
    DWORD    __stdcall AxmGetPortData(long lAxisNo,  WORD wOffset, DWORD *upData);
    // ���� �࿡ Port Data Setting - IP
    DWORD    __stdcall AxmSetPortData(long lAxisNo, WORD wOffset, DWORD dwData);
    // ���� �࿡ Port Data �������� - QI
    DWORD    __stdcall AxmGetPortDataQi(long lAxisNo, WORD byOffset, WORD *wData);
    // ���� �࿡ Port Data Setting - QI
    DWORD    __stdcall AxmSetPortDataQi(long lAxisNo, WORD byOffset, WORD wData);

    // ���� �࿡ ��ũ��Ʈ�� �����Ѵ�. - IP
    // sc    : ��ũ��Ʈ ��ȣ (1 - 4)
    // event : �߻��� �̺�Ʈ SCRCON �� �����Ѵ�.
    //         �̺�Ʈ ���� �హ������, �̺�Ʈ �߻��� ��, �̺�Ʈ ���� 1,2 �Ӽ� �����Ѵ�.
    // cmd   : � ������ �ٲܰ����� ���� SCRCMD�� �����Ѵ�.
    // data  : � Data�� �ٲܰ����� ����
    DWORD    __stdcall AxmSetScriptCaptionIp(long lAxisNo, long sc, DWORD event, DWORD data);
    // ���� �࿡ ��ũ��Ʈ�� ��ȯ�Ѵ�. - IP
    DWORD    __stdcall AxmGetScriptCaptionIp(long lAxisNo, long sc, DWORD *event, DWORD *data);

    // ���� �࿡ ��ũ��Ʈ�� �����Ѵ�. - QI
    // sc    : ��ũ��Ʈ ��ȣ (1 - 4)
    // event : �߻��� �̺�Ʈ SCRCON �� �����Ѵ�.
    //         �̺�Ʈ ���� �హ������, �̺�Ʈ �߻��� ��, �̺�Ʈ ���� 1,2 �Ӽ� �����Ѵ�.
    // cmd   : � ������ �ٲܰ����� ���� SCRCMD�� �����Ѵ�.
    // data  : � Data�� �ٲܰ����� ����
    DWORD    __stdcall AxmSetScriptCaptionQi(long lAxisNo, long sc, DWORD event, DWORD cmd, DWORD data);
    // ���� �࿡ ��ũ��Ʈ�� ��ȯ�Ѵ�. - QI
    DWORD    __stdcall AxmGetScriptCaptionQi(long lAxisNo, long sc, DWORD *event, DWORD *cmd, DWORD *data);

    // ���� �࿡ ��ũ��Ʈ ���� Queue Index�� Clear ��Ų��.
    // uSelect IP. 
    // uSelect(0): ��ũ��Ʈ Queue Index �� Clear�Ѵ�.
    //        (1): ĸ�� Queue�� Index Clear�Ѵ�.
    // uSelect QI. 
    // uSelect(0): ��ũ��Ʈ Queue 1 Index �� Clear�Ѵ�.
    //        (1): ��ũ��Ʈ Queue 2 Index �� Clear�Ѵ�.
    DWORD    __stdcall AxmSetScriptCaptionQueueClear(long lAxisNo, DWORD uSelect);

    // ���� �࿡ ��ũ��Ʈ ���� Queue�� Index ��ȯ�Ѵ�. 
    // uSelect IP
    // uSelect(0): ��ũ��Ʈ Queue Index�� �о�´�.
    //        (1): ĸ�� Queue Index�� �о�´�.
    // uSelect QI. 
    // uSelect(0): ��ũ��Ʈ Queue 1 Index�� �о�´�.
    //        (1): ��ũ��Ʈ Queue 2 Index�� �о�´�.
    DWORD    __stdcall AxmGetScriptCaptionQueueCount(long lAxisNo, DWORD *updata, DWORD uSelect);

    // ���� �࿡ ��ũ��Ʈ ���� Queue�� Data���� ��ȯ�Ѵ�. 
    // uSelect IP
    // uSelect(0): ��ũ��Ʈ Queue Data �� �о�´�.
    //        (1): ĸ�� Queue Data�� �о�´�.
    // uSelect QI.
    // uSelect(0): ��ũ��Ʈ Queue 1 Data �о�´�.
    //        (1): ��ũ��Ʈ Queue 2 Data �о�´�.
    DWORD    __stdcall AxmGetScriptCaptionQueueDataCount(long lAxisNo, DWORD *updata, DWORD uSelect);

    // ���� ����Ÿ�� �о�´�.
    DWORD    __stdcall AxmGetOptimizeDriveData(long lAxisNo, double dMinVel, double dVel, double dAccel, double  dDecel, 
            WORD *wRangeData, WORD *wStartStopSpeedData, WORD *wObjectSpeedData, WORD *wAccelRate, WORD *wDecelRate);

    // ���峻�� �������͸� Byte������ ���� �� Ȯ���Ѵ�.
    DWORD    __stdcall AxmBoardWriteByte(long lBoardNo, WORD wOffset, BYTE byData);
    DWORD    __stdcall AxmBoardReadByte(long lBoardNo, WORD wOffset, BYTE *byData);

    // ���峻�� �������͸� Word������ ���� �� Ȯ���Ѵ�.
    DWORD    __stdcall AxmBoardWriteWord(long lBoardNo, WORD wOffset, WORD wData);
    DWORD    __stdcall AxmBoardReadWord(long lBoardNo, WORD wOffset, WORD *wData);

    // ���峻�� �������͸� DWord������ ���� �� Ȯ���Ѵ�.
    DWORD    __stdcall AxmBoardWriteDWord(long lBoardNo, WORD wOffset, DWORD dwData);
    DWORD    __stdcall AxmBoardReadDWord(long lBoardNo, WORD wOffset, DWORD *dwData);

    // ���峻�� ��⿡ �������͸� Byte���� �� Ȯ���Ѵ�.
    DWORD    __stdcall AxmModuleWriteByte(long lBoardNo, long lModulePos, WORD wOffset, BYTE byData);
    DWORD    __stdcall AxmModuleReadByte(long lBoardNo, long lModulePos, WORD wOffset, BYTE *byData);

    // ���峻�� ��⿡ �������͸� Word���� �� Ȯ���Ѵ�.
    DWORD    __stdcall AxmModuleWriteWord(long lBoardNo, long lModulePos, WORD wOffset, WORD wData);
    DWORD    __stdcall AxmModuleReadWord(long lBoardNo, long lModulePos, WORD wOffset, WORD *wData);

    // ���峻�� ��⿡ �������͸� DWord���� �� Ȯ���Ѵ�.
    DWORD    __stdcall AxmModuleWriteDWord(long lBoardNo, long lModulePos, WORD wOffset, DWORD dwData);
    DWORD    __stdcall AxmModuleReadDWord(long lBoardNo, long lModulePos, WORD wOffset, DWORD *dwData);

    // �ܺ� ��ġ �񱳱⿡ ���� �����Ѵ�.(Pos = Unit)
    DWORD    __stdcall AxmStatusSetActComparatorPos(long lAxisNo, double dPos);
    // �ܺ� ��ġ �񱳱⿡ ���� ��ȯ�Ѵ�.(Positon = Unit)
    DWORD    __stdcall AxmStatusGetActComparatorPos(long lAxisNo, double *dpPos);

    // ���� ��ġ �񱳱⿡ ���� �����Ѵ�.(Pos = Unit)
    DWORD    __stdcall AxmStatusSetCmdComparatorPos(long lAxisNo, double dPos);
    // ���� ��ġ �񱳱⿡ ���� ��ȯ�Ѵ�.(Pos = Unit)
    DWORD    __stdcall AxmStatusGetCmdComparatorPos(long lAxisNo, double *dpPos);
	// ABS Position �� Flash �� �����Ѵ�.
    DWORD    __stdcall AxmStatusSetFlashAbsOffset(long lAxisNo, long dPosition);
	// Flash �� ���� �� ABS Position �� ��ȯ�Ѵ�.
	// dReadType  : Value in Flash Memory (0), Real used Value in memory(1)
    DWORD    __stdcall AxmStatusGetFlashAbsOffset(long lAxisNo, long* dPosition, DWORD dReadType);
	// ����ڰ� Flash �� ABS Position ������ �� �ִ� �ɼ��� �����Ѵ�.
	DWORD	 __stdcall AxmStatusSetAbsOffsetWriteEnable(long lAxisNo, bool bStatus);
	// ABS Position ���� �ɼ��� ���¸� ��ȯ�Ѵ�.
	DWORD	 __stdcall AxmStatusGetAbsOffsetWriteEnable(long lAxisNo, bool *bpStatus);	

//========== �߰� �Լ� =========================================================================================================
    // ���� ���� �� �ӵ��� ������ ���Ѵ�� �����Ѵ�.
    // �ӵ� ������� �Ÿ��� �־��־�� �Ѵ�. 
    DWORD    __stdcall AxmLineMoveVel(long lCoord, double dVel, double dAccel, double dDecel);

//========= ���� ��ġ ���� �Լ�( �ʵ�: IP������ , QI���� ��ɾ���)==============================================================
    // ���� ���� Sensor ��ȣ�� ��� ���� �� ��ȣ �Է� ������ �����Ѵ�.
    // ��� ���� LOW(0), HIGH(1), UNUSED(2), USED(3)
    DWORD    __stdcall AxmSensorSetSignal(long lAxisNo, DWORD uLevel);
    // ���� ���� Sensor ��ȣ�� ��� ���� �� ��ȣ �Է� ������ ��ȯ�Ѵ�.
    DWORD    __stdcall AxmSensorGetSignal(long lAxisNo, DWORD *upLevel);
    // ���� ���� Sensor ��ȣ�� �Է� ���¸� ��ȯ�Ѵ�
    DWORD    __stdcall AxmSensorReadSignal(long lAxisNo, DWORD *upStatus);

    // ���� ���� ������ �ӵ��� �������� ���� ��ġ ����̹��� �����Ѵ�.
    // Sensor ��ȣ�� Active level�Է� ���� ��� ��ǥ�� ������ �Ÿ���ŭ ������ �����Ѵ�.
    // �޽��� ��µǴ� �������� �Լ��� �����.
    // lMethod :  0 - �Ϲ� ����, 1 - ���� ��ȣ ���� ���� ���� ����. ��ȣ ���� �� �Ϲ� ����
    //            2 - ���� ����
    DWORD    __stdcall AxmSensorMovePos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lMethod);

    // ���� ���� ������ �ӵ��� �������� ���� ��ġ ����̹��� �����Ѵ�.
    // Sensor ��ȣ�� Active level�Է� ���� ��� ��ǥ�� ������ �Ÿ���ŭ ������ �����Ѵ�.
    // �޽� ����� ����Ǵ� �������� �Լ��� �����.
    DWORD    __stdcall AxmSensorStartMovePos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lMethod);

    // �����˻� ���ེ�� ��ȭ�� ����� ��ȯ�Ѵ�.
    // *lpStepCount      : ��ϵ� Step�� ����
    // *upMainStepNumber : ��ϵ� MainStepNumber ������ �迭����Ʈ
    // *upStepNumber     : ��ϵ� StepNumber ������ �迭����Ʈ
    // *upStepBranch     : ��ϵ� Step�� Branch ������ �迭����Ʈ
    // ����: �迭������ 50���� ����
    DWORD    __stdcall AxmHomeGetStepTrace(long lAxisNo, long *lpStepCount, DWORD *upMainStepNumber, DWORD *upStepNumber, DWORD *upStepBranch);

//=======�߰� Ȩ ��ġ (PI-N804/404���� �ش��.)=================================================================================
    // ����ڰ� ������ ���� Ȩ���� �Ķ��Ÿ�� �����Ѵ�.(QIĨ ���� �������� �̿�).
    // uZphasCount : Ȩ �Ϸ��Ŀ� Z�� ī��Ʈ(0 - 15)
    // lHomeMode   : Ȩ ���� ���( 0 - 12)
    // lClearSet   : ��ġ Ŭ���� , �ܿ��޽� Ŭ���� ��� ���� (0 - 3)
    //               0: ��ġŬ���� ������, �ܿ��޽� Ŭ���� ��� ����
    //                 1: ��ġŬ���� �����, �ܿ��޽� Ŭ���� ��� ����
    //               2: ��ġŬ���� ������, �ܿ��޽� Ŭ���� �����
    //               3: ��ġŬ���� �����, �ܿ��޽� Ŭ���� �����.
    // dOrgVel : Ȩ���� Org  Speed ���� 
    // dLastVel: Ȩ���� Last Speed ���� 
    DWORD    __stdcall AxmHomeSetConfig(long lAxisNo, DWORD uZphasCount, long lHomeMode, long lClearSet, double dOrgVel, double dLastVel, double dLeavePos);
    // ����ڰ� ������ ���� Ȩ���� �Ķ��Ÿ�� ��ȯ�Ѵ�.
    DWORD    __stdcall AxmHomeGetConfig(long lAxisNo, DWORD *upZphasCount, long *lpHomeMode, long *lpClearSet, double *dpOrgVel, double *dpLastVel, double *dpLeavePos); //KKJ(070215)

    // ����ڰ� ������ ���� Ȩ ��ġ�� �����Ѵ�.
    // lHomeMode ���� ���� : 0 - 5 ���� (Move Return�Ŀ� Search��  �����Ѵ�.)
    // lHomeMode -1�� �״�� ���� HomeConfig���� ����Ѵ�� �״�� ������.
    // ��������      : Vel���� ����̸� CW, �����̸� CCW.
    DWORD    __stdcall AxmHomeSetMoveSearch(long lAxisNo, double dVel, double dAccel, double dDecel);

    // ����ڰ� ������ ���� Ȩ ������ �����Ѵ�.
    // lHomeMode ���� ���� : 0 - 12 ���� 
    // lHomeMode -1�� �״�� ���� HomeConfig���� ����Ѵ�� �״�� ������.
    // ��������      : Vel���� ����̸� CW, �����̸� CCW.
    DWORD    __stdcall AxmHomeSetMoveReturn(long lAxisNo, double dVel, double dAccel, double dDecel);
    
    // ����ڰ� ������ ���� Ȩ ��Ż�� �����Ѵ�.
    // ��������      : Vel���� ����̸� CW, �����̸� CCW.
    DWORD    __stdcall AxmHomeSetMoveLeave(long lAxisNo, double dVel, double dAccel, double dDecel);

    // ����ڰ� ������ ������ Ȩ ��ġ�� �����Ѵ�.
    // lHomeMode ���� ���� : 0 - 5 ���� (Move Return�Ŀ� Search��  �����Ѵ�.)
    // lHomeMode -1�� �״�� ���� HomeConfig���� ����Ѵ�� �״�� ������.
    // ��������      : Vel���� ����̸� CW, �����̸� CCW.
    DWORD    __stdcall AxmHomeSetMultiMoveSearch(long lArraySize, long *lpAxesNo, double *dpVel, double *dpAccel, double *dpDecel);

    //������ ��ǥ���� ���� �ӵ� �������� ��带 �����Ѵ�.
    // (������ : �ݵ�� ����� �ϰ� ��밡��)
    // ProfileMode : '0' - ��Ī Trapezode
    //               '1' - ���Ī Trapezode
    //               '2' - ��Ī Quasi-S Curve
    //               '3' - ��Ī S Curve
    //               '4' - ���Ī S Curve
    DWORD    __stdcall AxmContiSetProfileMode(long lCoord, DWORD uProfileMode);
    // ������ ��ǥ���� ���� �ӵ� �������� ��带 ��ȯ�Ѵ�.
    DWORD    __stdcall AxmContiGetProfileMode(long lCoord, DWORD *upProfileMode);

    //========== DIO ���ͷ�Ʈ �÷��� ������Ʈ �б�
    // ������ �Է� ���� ���, Interrupt Flag Register�� Offset ��ġ���� bit ������ ���ͷ�Ʈ �߻� ���� ���� ����
    DWORD    __stdcall AxdiInterruptFlagReadBit(long lModuleNo, long lOffset, DWORD *upValue);
    // ������ �Է� ���� ���, Interrupt Flag Register�� Offset ��ġ���� byte ������ ���ͷ�Ʈ �߻� ���� ���� ����
    DWORD    __stdcall AxdiInterruptFlagReadByte(long lModuleNo, long lOffset, DWORD *upValue);
    // ������ �Է� ���� ���, Interrupt Flag Register�� Offset ��ġ���� word ������ ���ͷ�Ʈ �߻� ���� ���� ����
    DWORD    __stdcall AxdiInterruptFlagReadWord(long lModuleNo, long lOffset, DWORD *upValue);
    // ������ �Է� ���� ���, Interrupt Flag Register�� Offset ��ġ���� double word ������ ���ͷ�Ʈ �߻� ���� ���� ����
    DWORD    __stdcall AxdiInterruptFlagReadDword(long lModuleNo, long lOffset, DWORD *upValue);
    // ��ü �Է� ���� ���, Interrupt Flag Register�� Offset ��ġ���� bit ������ ���ͷ�Ʈ �߻� ���� ���� ����
    DWORD    __stdcall AxdiInterruptFlagRead(long lOffset, DWORD *upValue);

    //========= �α� ���� �Լ� ==========================================================================================
    // ���� �ڵ����� ������.
    // ���� ���� �Լ� ���� ����� EzSpy���� ����͸� �� �� �ֵ��� ���� �Ǵ� �����ϴ� �Լ��̴�.
    // uUse : ��� ���� => DISABLE(0), ENABLE(1)
    DWORD    __stdcall AxmLogSetAxis(long lAxisNo, DWORD uUse);
    
    // EzSpy������ ���� �� �Լ� ���� ��� ����͸� ���θ� Ȯ���ϴ� �Լ��̴�.
    DWORD    __stdcall AxmLogGetAxis(long lAxisNo, DWORD *upUse);

//=========== �α� ��� ���� �Լ�
    //������ �Է� ä���� EzSpy�� �α� ��� ���θ� �����Ѵ�.
    DWORD    __stdcall AxaiLogSetChannel(long lChannelNo, DWORD uUse);
    //������ �Է� ä���� EzSpy�� �α� ��� ���θ� Ȯ���Ѵ�.
    DWORD    __stdcall AxaiLogGetChannel(long lChannelNo, DWORD *upUse);

//==������ ��� ä���� EzSpy �α� ��� 
    //������ ��� ä���� EzSpy�� �α� ��� ���θ� �����Ѵ�.
    DWORD    __stdcall AxaoLogSetChannel(long lChannelNo, DWORD uUse);
    //������ ��� ä���� EzSpy�� �α� ��� ���θ� Ȯ���Ѵ�.
    DWORD    __stdcall AxaoLogGetChannel(long lChannelNo, DWORD *upUse);

//==Log
    // ������ ����� EzSpy�� �α� ��� ���� ����
    DWORD    __stdcall AxdLogSetModule(long lModuleNo, DWORD uUse);
    // ������ ����� EzSpy�� �α� ��� ���� Ȯ��
    DWORD    __stdcall AxdLogGetModule(long lModuleNo, DWORD *upUse);

    // ������ ���尡 RTEX ����� �� �� ������ firmware ������ Ȯ���Ѵ�.
    DWORD    __stdcall AxlGetFirmwareVersion(long lBoardNo, PTCHAR szVersion);
    // ������ ����� Firmware�� ���� �Ѵ�.
    DWORD    __stdcall AxlSetFirmwareCopy(long lBoardNo, WORD *wData, WORD *wCmdData);
    // ������ ����� Firmware Update�� �����Ѵ�.
    DWORD    __stdcall AxlSetFirmwareUpdate(long lBoardNo);
    // ������ ������ ���� RTEX �ʱ�ȭ ���¸� Ȯ�� �Ѵ�.
    DWORD    __stdcall AxlCheckStatus(long lBoardNo, DWORD* dwStatus);
    // ������ �࿡ RTEX Master board�� ���� ������ ���� �մϴ�.
    DWORD    __stdcall AxlRtexUniversalCmd(long lBoardNo, WORD wCmd, WORD wOffset, WORD *wData);
    // ������ ���� RTEX ��� ������ �����Ѵ�.
    DWORD    __stdcall AxmRtexSlaveCmd(long lAxisNo, DWORD dwCmdCode, DWORD dwTypeCode, DWORD dwIndexCode, DWORD dwCmdConfigure, DWORD dwValue);
    // ������ �࿡ ������ RTEX ��� ������ ������� Ȯ���Ѵ�.
    DWORD    __stdcall AxmRtexGetSlaveCmdResult(long lAxisNo, DWORD* dwIndex, DWORD *dwValue);
	// ������ �࿡ ������ RTEX ��� ������ ������� Ȯ���Ѵ�. PCIE-Rxx04-RTEX ����
	DWORD    __stdcall AxmRtexGetSlaveCmdResultEx(long lAxisNo, DWORD* dwpCommand, DWORD* dwpType, DWORD* dwpIndex, DWORD *dwpValue);
    // ������ �࿡ RTEX ���� ������ Ȯ���Ѵ�.
    DWORD   __stdcall AxmRtexGetAxisStatus(long lAxisNo, DWORD *dwStatus);
    // ������ �࿡ RTEX ��� ���� ������ Ȯ���Ѵ�.(Actual position, Velocity, Torque)
    DWORD   __stdcall AxmRtexGetAxisReturnData(long lAxisNo,  DWORD *dwReturn1, DWORD *dwReturn2, DWORD *dwReturn3);
    // ������ �࿡ RTEX Slave ���� ���� ���� ������ Ȯ���Ѵ�.(mechanical, Inposition and etc)
    DWORD   __stdcall AxmRtexGetAxisSlaveStatus(long lAxisNo,  DWORD *dwStatus);
    
    // ������ �࿡ MLII Slave �࿡ ���� ��Ʈ�� ���ɾ �����Ѵ�.
    DWORD   __stdcall AxmSetAxisCmd(long lAxisNo, DWORD* tagCommand);
    // ������ �࿡ MLII Slave �࿡ ���� ��Ʈ�� ������ ����� Ȯ���Ѵ�.
    DWORD   __stdcall AxmGetAxisCmdResult(long lAxisNo, DWORD* tagCommand);

    // ������ SIIIH Slave ��⿡ ��Ʈ�� ������ ����� �����ϰ� ��ȯ �Ѵ�.
    DWORD   __stdcall AxdSetAndGetSlaveCmdResult(long lModuleNo, DWORD* tagSetCommand, DWORD* tagGetCommand);
    DWORD   __stdcall AxaSetAndGetSlaveCmdResult(long lModuleNo, DWORD* tagSetCommand, DWORD* tagGetCommand);
    DWORD   __stdcall AxcSetAndGetSlaveCmdResult(long lModuleNo, DWORD* tagSetCommand, DWORD* tagGetCommand);
    
    // DPRAM �����͸� Ȯ���Ѵ�.
    DWORD   __stdcall AxlGetDpRamData(long lBoardNo, WORD wAddress, DWORD* dwpRdData);
    // DPRAM �����͸� Word������ Ȯ���Ѵ�.
    DWORD   __stdcall AxlBoardReadDpramWord(long lBoardNo, WORD wOffset, DWORD* dwpRdData);
	// DPRAM �����͸� Word������ �����Ѵ�.
	DWORD   __stdcall AxlBoardWriteDpramWord(long lBoardNo, WORD wOffset, DWORD dwWrData);

    // �� ������ �� SLAVE���� ������ �����Ѵ�.
    DWORD   __stdcall AxlSetSendBoardEachCommand(long lBoardNo, DWORD dwCommand, DWORD* dwpSendData, DWORD dwLength);
    // �� ����� ������ �����Ѵ�.
    DWORD   __stdcall AxlSetSendBoardCommand(long lBoardNo, DWORD dwCommand, DWORD* dwpSendData, DWORD dwLength);
    // �� ������ ������ Ȯ���Ѵ�.
    DWORD   __stdcall AxlGetResponseBoardCommand(long lBoardNo, DWORD* dwpReadData);

    // Network Type Master ���忡�� Slave ���� Firmware Version�� �о� ���� �Լ�.
    // ucaFirmwareVersion unsigned char ���� Array�� �����ϰ� ũ�Ⱑ 4�̻��� �ǵ��� ���� �ؾ� �Ѵ�.
    DWORD   __stdcall AxmInfoGetFirmwareVersion(long lAxisNo, unsigned char* ucaFirmwareVersion);
    DWORD   __stdcall AxaInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);
    DWORD   __stdcall AxdInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);
    DWORD   __stdcall AxcInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);
    
    //======== PCI-R1604-MLII ���� �Լ�=========================================================================== 
    // INTERPOLATE and LATCH Command�� Option Field�� Torq Feed Forward�� ���� ���� �ϵ��� �մϴ�.
    // �⺻���� MAX�� �����Ǿ� �ֽ��ϴ�.
    // �������� 0 ~ 4000H���� ���� �� �� �ֽ��ϴ�.
    // �������� 4000H�̻����� �����ϸ� ������ �� �̻����� �����ǳ� ������ 4000H���� ���� �˴ϴ�.
    DWORD   __stdcall AxmSetTorqFeedForward(long lAxisNo, DWORD dwTorqFeedForward);
    
    // INTERPOLATE and LATCH Command�� Option Field�� Torq Feed Forward�� ���� �о���� �Լ� �Դϴ�.
    // �⺻���� MAX�� �����Ǿ� �ֽ��ϴ�.
    DWORD   __stdcall AxmGetTorqFeedForward(long lAxisNo, DWORD* dwpTorqFeedForward);
    
    // INTERPOLATE and LATCH Command�� VFF Field�� Velocity Feed Forward�� ���� ���� �ϵ��� �մϴ�.
    // �⺻���� '0'�� �����Ǿ� �ֽ��ϴ�.
    // �������� 0 ~ FFFFH���� ���� �� �� �ֽ��ϴ�.
    DWORD   __stdcall AxmSetVelocityFeedForward(long lAxisNo, DWORD dwVelocityFeedForward);
    
    // INTERPOLATE and LATCH Command�� VFF Field�� Velocity Feed Forward�� ���� �о���� �Լ� �Դϴ�.
    DWORD   __stdcall AxmGetVelocityFeedForward(long lAxisNo, DWORD* dwpVelocityFeedForward);

    // Encoder type�� �����Ѵ�.
    // �⺻���� 0(TYPE_INCREMENTAL)�� �����Ǿ� �ֽ��ϴ�.
    // �������� 0 ~ 1���� ���� �� �� �ֽ��ϴ�.
    // ������ : 0(TYPE_INCREMENTAL), 1(TYPE_ABSOLUTE).
    DWORD   __stdcall AxmSignalSetEncoderType(long lAxisNo, DWORD dwEncoderType);

    // Encoder type�� Ȯ���Ѵ�.
    DWORD   __stdcall AxmSignalGetEncoderType(long lAxisNo, DWORD* dwpEncoderType);
    //======================================================================================================== 

    // Slave Firmware Update�� ���� �߰�
    //DWORD   __stdcall AxmSetSendAxisCommand(long lAxisNo, WORD wCommand, WORD* wpSendData, WORD wLength);

    //======== PCI-R1604-RTEX, RTEX-PM ���� �Լ�============================================================== 
    // ���� �Է� 2,3�� �Է½� JOG ���� �ӵ��� �����Ѵ�. 
    // ������ ���õ� ��� ����(Ex, PulseOutMethod, MoveUnitPerPulse ��)���� �Ϸ�� ���� �ѹ��� �����Ͽ��� �Ѵ�.
    DWORD    __stdcall AxmMotSetUserMotion(long lAxisNo, double dVelocity, double dAccel, double dDecel);

    // ���� �Է� 2,3�� �Է½� JOG ���� ���� ��� ���θ� �����Ѵ�.
    // ������ :  0(DISABLE), 1(ENABLE)
    DWORD    __stdcall AxmMotSetUserMotionUsage(long lAxisNo, DWORD dwUsage);

    // MPGP �Է��� ����Ͽ� Load/UnLoad ��ġ�� �ڵ����� �̵��ϴ� ��� ����. 
    DWORD    __stdcall  AxmMotSetUserPosMotion(long lAxisNo, double dVelocity, double dAccel, double dDecel, double dLoadPos, double dUnLoadPos, DWORD dwFilter, DWORD dwDelay);

    // MPGP �Է��� ����Ͽ� Load/UnLoad ��ġ�� �ڵ����� �̵��ϴ� ��� ����. 
    // ������ :  0(DISABLE), 1(Position ��� A ���), 2(Position ��� B ���)
    DWORD    __stdcall  AxmMotSetUserPosMotionUsage(long lAxisNo, DWORD dwUsage);
    //======================================================================================================== 

    //======== SIO-CN2CH/HPC4, ���� ��ġ Ʈ���� ��� ��� ���� �Լ�================================================ 
    // �޸� ������ ���� �Լ�
    DWORD  __stdcall AxcKeWriteRamDataAddr(long lChannelNo, DWORD dwAddr, DWORD dwData);
    // �޸� ������ �б� �Լ�
    DWORD  __stdcall AxcKeReadRamDataAddr(long lChannelNo, DWORD dwAddr, DWORD* dwpData);
    // �޸� �ʱ�ȭ �Լ�
    DWORD  __stdcall AxcKeResetRamDataAll(long lModuleNo, DWORD dwData);
    // Ʈ���� Ÿ�� �ƿ� ���� �Լ�
    DWORD  __stdcall AxcTriggerSetTimeout(long lChannelNo, DWORD dwTimeout);
    // Ʈ���� Ÿ�� �ƿ� Ȯ�� �Լ�
    DWORD  __stdcall AxcTriggerGetTimeout(long lChannelNo, DWORD* dwpTimeout);
    // Ʈ���� ��� ���� Ȯ�� �Լ�
    DWORD  __stdcall AxcStatusGetWaitState(long lChannelNo, DWORD* dwpState);
    // Ʈ���� ��� ���� ���� �Լ�
    DWORD  __stdcall AxcStatusSetWaitState(long lChannelNo, DWORD dwState);

    // ���� ä�ο� ���ɾ� ����.
    DWORD  __stdcall AxcKeSetCommandData32(long lChannelNo, DWORD dwCommand, DWORD dwData);
    // ���� ä�ο� ���ɾ� ����.
    DWORD  __stdcall AxcKeSetCommandData16(long lChannelNo, DWORD dwCommand, WORD wData);
    // ���� ä���� �������� Ȯ��.
    DWORD  __stdcall AxcKeGetCommandData32(long lChannelNo, DWORD dwCommand, DWORD *dwpData);
    // ���� ä���� �������� Ȯ��.
    DWORD  __stdcall AxcKeGetCommandData16(long lChannelNo, DWORD dwCommand, WORD *wpData);
    //======================================================================================================== 

    //======== PCI-N804/N404 ����, Sequence Motion ===================================================================
    // Sequence Motion�� �� ������ ���� �մϴ�. (�ּ� 1��)
    // lSeqMapNo : �� ��ȣ ������ ��� Sequence Motion Index Point
    // lSeqMapSize : �� ��ȣ ����
    // long* LSeqAxesNo : �� ��ȣ �迭
    DWORD   __stdcall AxmSeqSetAxisMap(long lSeqMapNo, long lSeqMapSize, long* lSeqAxesNo);
    DWORD   __stdcall AxmSeqGetAxisMap(long lSeqMapNo, long* lSeqMapSize, long* lSeqAxesNo);

    // Sequence Motion�� ����(Master) ���� ���� �մϴ�. 
    // �ݵ�� AxmSeqSetAxisMap(...) �� ������ �� ������ �����Ͽ��� �մϴ�.
    DWORD   __stdcall AxmSeqSetMasterAxisNo(long lSeqMapNo, long lMasterAxisNo);

    // Sequence Motion�� Node ���� ������ ���̺귯���� �˸��ϴ�.
    DWORD   __stdcall AxmSeqBeginNode(long lSeqMapNo);

    // Sequence Motion�� Node ���� ���Ḧ ���̺귯���� �˸��ϴ�.
    DWORD   __stdcall AxmSeqEndNode(long lSeqMapNo);

    // Sequence Motion�� ������ ���� �մϴ�.
    DWORD   __stdcall AxmSeqStart(long lSeqMapNo, DWORD dwStartOption);

    // Sequence Motion�� �� Profile Node ������ ���̺귯���� �Է� �մϴ�.
    // ���� 1�� Sequence Motion�� ����ϴ���, *dPosition�� 1���� Array�� �����Ͽ� �ֽñ� �ٶ��ϴ�.
    DWORD   __stdcall AxmSeqAddNode(long lSeqMapNo, double* dPosition, double dVelocity, double dAcceleration, double dDeceleration, double dNextVelocity);

    // Sequence Motion�� ���� �� ���� ���� ���� Node Index�� �˷� �ݴϴ�.
    DWORD   __stdcall AxmSeqGetNodeNum(long lSeqMapNo, long* lCurNodeNo);

    // Sequence Motion�� �� Node Count�� Ȯ�� �մϴ�.
    DWORD   __stdcall AxmSeqGetTotalNodeNum(long lSeqMapNo, long* lTotalNodeCnt);

    // Sequence Motion�� ���� ���� ������ Ȯ�� �մϴ�.
    // dwInMotion : 0(���� ����), 1(���� ��)
    DWORD   __stdcall AxmSeqIsMotion(long lSeqMapNo, DWORD* dwInMotion);

    // Sequence Motion�� Memory�� Clear �մϴ�.
    // AxmSeqSetAxisMap(...), AxmSeqSetMasterAxisNo(...) ���� ������ ���� �����˴ϴ�.
    DWORD   __stdcall AxmSeqWriteClear(long lSeqMapNo);

    // Sequence Motion�� ������ ���� �մϴ�.
    // dwStopMode : 0(EMERGENCY_STOP), 1(SLOWDOWN_STOP) 
    DWORD   __stdcall AxmSeqStop(long lSeqMapNo, DWORD dwStopMode);
    //======================================================================================================== 


	//======== PCIe-Rxx04-SIIIH ���� �Լ�==========================================================================
	// (SIIIH, MR_J4_xxB, Para : 0 ~ 8) ==
	//     [0] : Command Position
	//     [1] : Actual Position
	//     [2] : Actual Velocity
	//     [3] : Mechanical Signal
	//     [4] : Regeneration load factor(%)
	//     [5] : Effective load factor(%)
	//     [6] : Peak load factor(%)
	//     [7] : Current Feedback
	//     [8] : Command Velocity
	DWORD	__stdcall AxmStatusSetMon(long lAxisNo, DWORD dwParaNo1, DWORD dwParaNo2, DWORD dwParaNo3, DWORD dwParaNo4, DWORD dwUse);
	DWORD	__stdcall AxmStatusGetMon(long lAxisNo, DWORD *dwpParaNo1, DWORD *dwpParaNo2, DWORD *dwpParaNo3, DWORD *dwpParaNo4, DWORD *dwpUse);
	DWORD	__stdcall AxmStatusReadMon(long lAxisNo, DWORD *dwpParaNo1, DWORD *dwpParaNo2, DWORD *dwpParaNo3, DWORD *dwpParaNo4, DWORD *dwDataVaild);
	DWORD	__stdcall AxmStatusReadMonEx(long lAxisNo, long *lpDataCnt, DWORD *dwpReadData);
	//=============================================================================================================
		
    //======== PCI-R32IOEV-RTEX ���� �Լ�=========================================================================== 
    // I/O ��Ʈ�� �Ҵ�� HPI register �� �а� �������� API �Լ�.
    // I/O Registers for HOST interface.
    // I/O 00h Host status register (HSR)
    // I/O 04h Host-to-DSP control register (HDCR)
    // I/O 08h DSP page register (DSPP)
    // I/O 0Ch Reserved
    DWORD   __stdcall AxlSetIoPort(long lBoardNo, DWORD dwAddr, DWORD dwData);
    DWORD   __stdcall AxlGetIoPort(long lBoardNo, DWORD dwAddr, DWORD* dwpData);

    //======== PCI-R3200-MLIII ���� �Լ�=========================================================================== 
/*
    // M-III Master ���� �߿��� ������Ʈ �⺻ ���� ���� �Լ�
    DWORD   __stdcall AxlM3SetFWUpdateInit(long lBoardNo, DWORD dwTotalPacketSize);
    // M-III Master ���� �߿��� ������Ʈ �⺻ ���� ���� ��� Ȯ�� �Լ�
    DWORD   __stdcall AxlM3GetFWUpdateInit(long lBoardNo, DWORD *dwTotalPacketSize);
    // M-III Master ���� �߿��� ������Ʈ �ڷ� ���� �Լ�
    DWORD   __stdcall AxlM3SetFWUpdateCopy(long lBoardNo, DWORD *lFWUpdataData, DWORD dwLength);
    // M-III Master ���� �߿��� ������Ʈ �ڷ� ���� ��� Ȯ�� �Լ�
    DWORD   __stdcall AxlM3GetFWUpdateCopy(long lBoardNo, BYTE bCrcData, DWORD *lFWUpdataResult);
    // M-III Master ���� �߿��� ������Ʈ ����
    DWORD   __stdcall AxlM3SetFWUpdate(long lBoardNo, DWORD dwSectorNo);
    // M-III Master ���� �߿��� ������Ʈ ���� ��� Ȯ��
    DWORD   __stdcall AxlM3GetFWUpdate(long lBoardNo, DWORD *dwSectorNo, DWORD *dwIsDone);
*/
    // M-III Master ���� �߿��� ������Ʈ �⺻ ���� ���� �Լ�
    DWORD   __stdcall AxlM3SetFWUpdateInit(long lBoardNo, DWORD dwTotalPacketSize, DWORD dwProcTotalStepNo);
    // M-III Master ���� �߿��� ������Ʈ �⺻ ���� ���� ��� Ȯ�� �Լ�
    DWORD   __stdcall AxlM3GetFWUpdateInit(long lBoardNo, DWORD *dwTotalPacketSize, DWORD *dwProcTotalStepNo);

    // M-III Master ���� �߿��� ������Ʈ �ڷ� ���� �Լ�
    DWORD   __stdcall AxlM3SetFWUpdateCopy(long lBoardNo, DWORD *pdwPacketData, DWORD dwPacketSize);
    // M-III Master ���� �߿��� ������Ʈ �ڷ� ���� ��� Ȯ�� �Լ�
    DWORD   __stdcall AxlM3GetFWUpdateCopy(long lBoardNo, DWORD *dwPacketSize);

    // M-III Master ���� �߿��� ������Ʈ ����
    DWORD   __stdcall AxlM3SetFWUpdate(long lBoardNo, DWORD dwFlashBurnStepNo);
    // M-III Master ���� �߿��� ������Ʈ ���� ��� Ȯ��
    DWORD   __stdcall AxlM3GetFWUpdate(long lBoardNo, DWORD *dwFlashBurnStepNo, DWORD *dwIsFlashBurnDone);

    // M-III Master ���� EEPROM ������ ���� �Լ�
    DWORD   __stdcall AxlM3SetCFGData(long lBoardNo, DWORD *pCmdData, DWORD CmdDataSize);
    // M-III Master ���� EEPROM ������ �������� �Լ�
    DWORD   __stdcall AxlM3GetCFGData(long lBoardNo, DWORD *pCmdData, DWORD CmdDataSize);

    // M-III Master ���� CONNECT PARAMETER �⺻ ���� ���� �Լ�
    DWORD   __stdcall AxlM3SetMCParaUpdateInit(long lBoardNo, WORD wCh0Slaves, WORD wCh1Slaves, DWORD dwCh0CycTime, DWORD dwCh1CycTime, DWORD dwChInfoMaxRetry);
    // M-III Master ���� CONNECT PARAMETER �⺻ ���� ���� ��� Ȯ�� �Լ�
    DWORD   __stdcall AxlM3GetMCParaUpdateInit(long lBoardNo, WORD *wCh0Slaves, WORD *wCh1Slaves, DWORD *dwCh0CycTime, DWORD *dwCh1CycTime, DWORD *dwChInfoMaxRetry);
    // M-III Master ���� CONNECT PARAMETER �⺻ ���� ���� �Լ�
    DWORD   __stdcall AxlM3SetMCParaUpdateCopy(long lBoardNo, WORD wIdx, WORD wChannel, WORD wSlaveAddr, DWORD dwProtoCalType, DWORD dwTransBytes, DWORD dwDeviceCode);
    // M-III Master ���� CONNECT PARAMETER �⺻ ���� ���� ��� Ȯ�� �Լ�
    DWORD   __stdcall AxlM3GetMCParaUpdateCopy(long lBoardNo, WORD wIdx, WORD *wChannel, WORD *wSlaveAddr, DWORD *dwProtoCalType, DWORD *dwTransBytes, DWORD *dwDeviceCode);

    // M-III Master ���峻�� �������͸� DWord������ Ȯ�� �Լ�
    DWORD   __stdcall AxlBoardReadDWord(long lBoardNo, WORD wOffset, DWORD *dwData);
    // M-III Master ���峻�� �������͸� DWord������ ���� �Լ�
    DWORD   __stdcall AxlBoardWriteDWord(long lBoardNo, WORD wOffset, DWORD dwData);

    // ���峻�� Ȯ�� �������͸� DWord������ ���� �� Ȯ���Ѵ�.
    DWORD    __stdcall AxlBoardReadDWordEx(long lBoardNo, DWORD dwOffset, DWORD *dwData);
    DWORD    __stdcall AxlBoardWriteDWordEx(long lBoardNo, DWORD dwOffset, DWORD dwData);

    // ������ ���� ���� ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetCtrlStopMode(long lAxisNo, BYTE bStopMode);
    // ������ Lt ���� ���·� ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetCtrlLtSel(long lAxisNo, BYTE bLtSel1, BYTE bLtSel2);
    // ������ IO �Է� ���¸� Ȯ�� �Լ�
    DWORD   __stdcall AxmStatusReadServoCmdIOInput(long lAxisNo, DWORD *upStatus);    
    // ������ ���� ���� �Լ�
    DWORD   __stdcall AxmM3ServoExInterpolate(long lAxisNo, DWORD dwTPOS, DWORD dwVFF, DWORD dwTFF, DWORD dwTLIM, DWORD dwExSig1, DWORD dwExSig2);
    // ���� �������� ���̾ ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetExpoAccBias(long lAxisNo, WORD wBias);
    // ���� �������� �ð� ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetExpoAccTime(long lAxisNo, WORD wTime);
    // ������ �̵� �ð��� ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetMoveAvrTime(long lAxisNo, WORD wTime);
    // ������ Acc ���� ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetAccFilter(long lAxisNo, BYTE bAccFil);
    // ������ ���� �����1 ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetCprmMonitor1(long lAxisNo, BYTE bMonSel);
    // ������ ���� �����2 ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetCprmMonitor2(long lAxisNo, BYTE bMonSel);
    // ������ ���� �����1 Ȯ�� �Լ�
    DWORD   __stdcall AxmM3ServoStatusReadCprmMonitor1(long lAxisNo, DWORD *upStatus);
    // ������ ���� �����2 Ȯ�� �Լ�
    DWORD   __stdcall AxmM3ServoStatusReadCprmMonitor2(long lAxisNo, DWORD *upStatus);
    // ���� �������� Dec ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetAccDec(long lAxisNo, WORD wAcc1, WORD wAcc2, WORD wAccSW, WORD wDec1, WORD wDec2, WORD wDecSW);
    // ���� ���� ���� �Լ�
    DWORD   __stdcall AxmM3ServoSetStop(long lAxisNo, long lMaxDecel);

//========== ǥ�� I/O ��� ���� Ŀ�ǵ� =========================================================================
    // Network��ǰ �� �����̺� ����� �Ķ���� ���� ���� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network��ǰ �� �����̺� ����� �Ķ���� ���� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network��ǰ �� �����̺� ����� ID���� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationIdRd(long lBoardNo, long lModuleNo, BYTE bIdCode, BYTE bOffset, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network��ǰ �� �����̺� ����� ��ȿ Ŀ�ǵ�� ����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationNop(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network��ǰ �� �����̺� ����� �¾��� �ǽ��ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationConfig(long lBoardNo, long lModuleNo, BYTE bConfigMode, BYTE bModuleType);
    // Network��ǰ �� �����̺� ����� �˶� �� ��� ���� ���� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationAlarm(long lBoardNo, long lModuleNo, WORD wAlarmRdMod, WORD wAlarmIndex, BYTE bModuleType, WORD *pwAlarmData);
    // Network��ǰ �� �����̺� ����� �˶� �� ��� ���¸� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationAlarmClear(long lBoardNo, long lModuleNo, WORD wAlarmClrMod, BYTE bModuleType);
    // Network��ǰ �� �����̺� ������ ��������� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationSyncSet(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network��ǰ �� �����̺� ������ ������ �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationConnect(long lBoardNo, long lModuleNo, BYTE bVer, BYTE bComMode, BYTE bComTime, BYTE bProfileType, BYTE bModuleType);
    // Network��ǰ �� �����̺� ������ ���� ������ �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationDisConnect(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network��ǰ �� �����̺� ����� ���ֹ߼� �Ķ���� ���� ���� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationStoredParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network��ǰ �� �����̺� ����� ���ֹ߼� �Ķ���� ���� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationStoredParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    // Network��ǰ �� �����̺� ����� �޸� ���� ���� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationMemory(long lBoardNo, long lModuleNo, WORD wSize, DWORD dwAddress, BYTE bModuleType, BYTE bMode, BYTE bDataType, BYTE *pbData);
    // Network��ǰ �� �����̺� ����� �޸� ���� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationMemory(long lBoardNo, long lModuleNo, WORD wSize, DWORD dwAddress, BYTE bModuleType, BYTE bMode, BYTE bDataType, BYTE *pbData);

//========== ǥ�� I/O ��� Ŀ�ؼ� Ŀ�ǵ� =========================================================================
    // Network��ǰ �� �������� �����̺� ����� �ڵ� �＼�� ��� ���� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationAccessMode(long lBoardNo, long lModuleNo, BYTE bModuleType, BYTE bRWSMode);
    // Network��ǰ �� �������� �����̺� ����� �ڵ� �＼�� ��� �������� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationAccessMode(long lBoardNo, long lModuleNo, BYTE bModuleType, BYTE *bRWSMode);
    // Network��ǰ �� �����̺� ����� ���� �ڵ� ���� ��带 �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetAutoSyncConnectMode(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD dwAutoSyncConnectMode);
    // Network��ǰ �� �����̺� ����� ���� �ڵ� ���� ��� �������� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetAutoSyncConnectMode(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *dwpAutoSyncConnectMode);
    // Network��ǰ �� �����̺� ��⿡ ���� ���� ����ȭ ������ �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SyncConnectSingle(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network��ǰ �� �����̺� ��⿡ ���� ���� ����ȭ ���� ������ �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SyncDisconnectSingle(long lBoardNo, long lModuleNo, BYTE bModuleType);
    // Network��ǰ �� �����̺� ������ ���� ���¸� Ȯ���ϴ� �Լ�
    DWORD   __stdcall AxlM3IsOnLine(long lBoardNo, long lModuleNo, DWORD *dwData);

//========== ǥ�� I/O �������� Ŀ�ǵ� =========================================================================
    // Network��ǰ �� ����ȭ ������ �����̺� I/O ��⿡ ���� ������ �������� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationRWS(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    // Network��ǰ �� ����ȭ ������ �����̺� I/O ��⿡ ���� �����Ͱ��� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationRWS(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    // Network��ǰ �� �񵿱�ȭ ������ �����̺� I/O ��⿡ ���� ������ �������� ��ȯ�ϴ� �Լ�
    DWORD   __stdcall AxlM3GetStationRWA(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    // Network��ǰ �� �񵿱�ȭ ������ �����̺� I/O ��⿡ ���� �����Ͱ��� �����ϴ� �Լ�
    DWORD   __stdcall AxlM3SetStationRWA(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);

    // MLIII adjustment operation�� ���� �Ѵ�.
    // dwReqCode == 0x1005 : parameter initialization : 20sec
    // dwReqCode == 0x1008 : absolute encoder reset   : 5sec
    // dwReqCode == 0x100E : automatic offset adjustment of motor current detection signals  : 5sec
    // dwReqCode == 0x1013 : Multiturn limit setting  : 5sec
    DWORD   __stdcall AxmM3AdjustmentOperation(long lAxisNo, DWORD dwReqCode);

    // M3 ���� ���� �˻� ���� ���� ���ܿ� �Լ��̴�.
    DWORD    __stdcall AxmHomeGetM3FWRealRate(long lAxisNo, DWORD *upHomeMainStepNumber, DWORD *upHomeSubStepNumber, DWORD *upHomeLastMainStepNumber, DWORD *upHomeLastSubStepNumber);
	// M3 ���� ���� �˻��� ���������� Ż��� �����Ǵ� ��ġ ���� ��ȯ�ϴ� �Լ��̴�.
    DWORD   __stdcall AxmHomeGetM3OffsetAvoideSenArea(long lAxisNo, double *dPos);
    // M3 ���� ���� �˻��� ���������� Ż��� �����Ǵ� ��ġ ���� �����ϴ� �Լ��̴�.
	// dPos ���� ���� 0�̸� �ڵ����� Ż��� �����Ǵ� ��ġ ���� �ڵ����� �����ȴ�.
	// dPos ���� ���� ����� ���� �Է��Ѵ�.
	DWORD   __stdcall AxmHomeSetM3OffsetAvoideSenArea(long lAxisNo, double dPos);

	// M3 ����, ����ġ ���ڴ� ��� ����, �����˻� �Ϸ� �� CMD/ACT POS �ʱ�ȭ ���� ����
	// dwSel: 0, ���� �˻��� CMD/ACTPOS 0���� ������.[�ʱⰪ]
	// dwSel: 1, ���� �˻��� CMD/ACTPOS ���� �������� ����.
	DWORD   __stdcall AxmM3SetAbsEncOrgResetDisable(long lAxisNo, DWORD dwSel);

	// M3 ����, ����ġ ���ڴ� ��� ����, �����˻� �Ϸ� �� CMD/ACT POS �ʱ�ȭ ���� ������ ��������
	// upSel: 0, ���� �˻��� CMD/ACTPOS 0���� ������.[�ʱⰪ]
	// upSel: 1, ���� �˻��� CMD/ACTPOS ���� �������� ����.
	DWORD   __stdcall AxmM3GetAbsEncOrgResetDisable(long lAxisNo, DWORD *upSel);

	// M3 ����, �����̺� OFFLINE ��ȯ�� �˶� ���� ��� ��� ���� ����
	// dwSel: 0, ML3 �����̺� ONLINE->OFFLINE �˶� ó�� ������� ����.[�ʱⰪ]
	// dwSel: 1, ML3 �����̺� ONLINE->OFFLINE �˶� ó�� ���

	DWORD   __stdcall AxmM3SetOfflineAlarmEnable(long lAxisNo, DWORD dwSel);
	// M3 ����, �����̺� OFFLINE ��ȯ�� �˶� ���� ��� ��� ���� ���� �� ��������
	// upSel: 0, ML3 �����̺� ONLINE->OFFLINE �˶� ó�� ������� ����.[�ʱⰪ]
	// upSel: 1, ML3 �����̺� ONLINE->OFFLINE �˶� ó�� ���

	DWORD   __stdcall AxmM3GetOfflineAlarmEnable(long lAxisNo, DWORD *upSel);

	// M3 ����, �����̺� OFFLINE ��ȯ ���� ���� �� ��������
	// upSel: 0, ML3 �����̺� ONLINE->OFFLINE ��ȯ���� ����
	// upSel: 1, ML3 �����̺� ONLINE->OFFLINE ��ȯ�Ǿ���.
	DWORD   __stdcall AxmM3ReadOnlineToOfflineStatus(long lAxisNo, DWORD *upStatus);

    // Network ��ǰ�� Configuration Lock ���¸� �����Ѵ�.
    // wLockMode  : DISABLE(0), ENABLE(1)
    DWORD   __stdcall AxlSetLockMode(long lBoardNo, WORD wLockMode);

    // Lock ������ ����
    DWORD   __stdcall AxlSetLockData(long lBoardNo, DWORD dwTotalNodeNum, DWORD *dwpNodeNo, DWORD *dwpNodeID, DWORD *dwpLockData);

	DWORD   __stdcall AxmMoveStartPosWithAVC(long lAxisNo, double dPosition, double dMaxVelocity, double dMaxAccel, double dMinJerk, double *dpMoveVelocity, double *dpMoveAccel, double *dpMoveJerk);
    // ī���� ����� 2-D ������ġ Ʈ���� ����� ���� �ʿ��� Ʈ���� ��ġ ������ �����Ѵ�.
    // lChannelNo : 0,1 channel �� ��� 0, 2,3 channel �� ��� 2 �� ����. 
    // nDataCnt : 
    //  nDataCnt > 0 : ������ ���, nDataCnt <= 0 : ��ϵ� ������ �ʱ�ȭ.
    // dwOption : Reserved.
    // dpPatternData : (X1, Y1)
    DWORD  __stdcall AxcTriggerSetPatternData(long lChannelNo, long nDataCnt,  DWORD dwOption, double* dpPatternData);
    // ī���� ����� 2-D ������ġ Ʈ���� ����� ���� �ʿ��� Ʈ���� ��ġ ������ Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetPatternData(long lChannelNo, long* npDataCnt,  DWORD* dwpOption, double* dpPatternData);

	//���� ���� �����Ͽ� AxmContiEndNode �Լ������� ������������ Node �� Data Queue �� �̸� ä������ �� �ֵ����ϴ� ����� Ȱ��ȭ �Ѵ�.
	//bPushPrevContiQueue : 1 // �ش� ��� Ȱ��ȭ 
	//bPushPrevContiQueue : 0 // �ش� ��� ��Ȱ��ȭ 	
	DWORD __stdcall AxmContiSetPushPrevContiQueueEnable(long lCoordinate,BOOL bPushPrevContiQueue);
	//�����ص� AxmContiSetPushPrevContiQueueEnable Flag���� ��ȯ�Ѵ�.	
	DWORD __stdcall AxmContiGetPushPrevContiQueueEnable(long lCoordinate,BOOL *bPushPrevContiQueue);

	// ���Ӻ��� ���� �� Data Queue �� Node ������ ����Ǿ����� ���¸� ��ȯ�Ѵ�.
	// AxmContiSetPushPrevContiQueueEnable(long lCoordinate, 1) �� �����Ǿ����� ��츸 ��ȿ
	// bPushPrevContiQueueComplete : 1 // Node Data ���� �Ϸ�
	// bPushPrevContiQueueComplete : 0 // Node Data ���� �Ǿ���������
	DWORD __stdcall AxmContiGetPushPrevContiQueueComplete(long lCoordinate, BOOL * bPushPrevContiQueueComplete);
	
    // ���Ӻ��� ���� �� ù ��� ���� �� ������ ��� ���� �� �����ð� ���� ������ ��ǥ���� ������ ���� OutputBit On/Off ����
	// AxmContiBeginNode �տ� ȣ���ؾ� �Ѵ�. �ѹ� �����ϸ� Flag�� �ʱ�ȭ�Ǿ� �ٽ� ȣ���ؾ� ����� �� �ִ�.
    // StartTime/EndTime ������ [Sec]�̸�, 0 ~ 6.5�ʱ��� ���� �����ϴ�.
    // uOnoff	: 0 - ���� ��ġ���� Bit On ���� ��ġ���� Bit Off
    //          : 1 - ���� ��ġ���� Bit Off ���� ��ġ���� Bit On
    // lEndMode : 0 - ������ ��� ���� ���� �� ��� OutputBit Off/On
    //			: 1 - ������ ��� ���� ���� �� �Է��� EndTime ���� OutputBit Off/On
    //			: 2 - ���� ���� �� OutputBit On/Off �� �Է��� EndTime ���� OutputBit Off/On
	DWORD __stdcall AxmContiSetWriteOutputBit(long lCoordinate, double dStartTime, double dEndTime, long lBitNo, long uOnoff, long lEndMode);

	// AxmContiSetWriteOutputBit�� ������ ������ ��ȯ�Ѵ�.
	DWORD __stdcall	AxmContiGetWriteOutputBit(long lCoordinate, double* dpStartTime, double* dpEndTime, long* lpBitNo, long* lpOnoff, long* lpEndMode);

	// AxmContiSetWriteOutputBit�� ������ ������ �����Ѵ�.
	DWORD __stdcall	AxmContiResetWriteOutputBit(long lCoordinate);

	// AxmMoveTorqueStop �Լ��� ��ũ ���� ���� �� CmdPos ���� ActPos ���� ��ġ��Ű�� ���������� ��� �ð��� �����Ѵ�.
	// dwSettlingTime
	//  1) ����: [msec]
	//  2) �Է� ���� ����: 0 ~ 10000
	//  *����* AxmMoveTorqueSetStopSettlingTime �Լ��� ��� �ð��� �������� ������, dafault ���� 10[msec]�� ����ȴ�.
	DWORD __stdcall AxmMoveTorqueSetStopSettlingTime(long lAxisNo, DWORD dwSettlingTime);
	// AxmMoveTorqueStop �Լ��� ��ũ ���� ���� �� CmdPos ���� ActPos ���� ��ġ��Ű�� ���������� ��� �ð��� ��ȯ�Ѵ�.
	DWORD __stdcall AxmMoveTorqueGetStopSettlingTime(long lAxisNo, DWORD* dwpSettlingTime);

	//////////////////////////////////////////////////////////////////////////
	// Monitor  
	// �����͸� ������ ������ �׸��� �߰��մϴ�.
	DWORD  __stdcall AxlMonitorSetItem(long lBoardNo, long lItemIndex, DWORD dwSignalType, long lSignalNo, long lSubSignalNo);

	// ������ ������ ������ �׸�鿡 ���� ������ �����ɴϴ�.
	DWORD  __stdcall AxlMonitorGetIndexInfo(long lBoardNo, long* lpItemSize, long* lpItemIndex);

	// ������ ������ ������ �� �׸��� ���� ������ �����ɴϴ�.
	DWORD  __stdcall AxlMonitorGetItemInfo(long lBoardNo, long lItemIndex, DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo);

	// ��� ������ ���� �׸��� ������ �ʱ�ȭ�մϴ�.
	DWORD  __stdcall AxlMonitorResetAllItem(long lBoardNo);

	// ���õ� ������ ���� �׸��� ������ �ʱ�ȭ�մϴ�.
	DWORD  __stdcall AxlMonitorResetItem(long lBoardNo, long lItemIndex);

	// ������ ������ Ʈ���� ������ �����մϴ�.
	DWORD  __stdcall AxlMonitorSetTriggerOption(long lBoardNo, DWORD dwSignalType, long lSignalNo, long lSubSignalNo, DWORD dwOperatorType, double dValue1, double dValue2);

	// ������ ������ Ʈ���� ������ �����ɴϴ�.
	//DWORD  __stdcall AxlMonitorGetTriggerOption(DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo, DWORD* dwpOperatorType, double* dpValue1, double* dpValue2);

	// ������ ������ Ʈ���� ������ �ʱ�ȭ�մϴ�.
	DWORD  __stdcall AxlMonitorResetTriggerOption(long lBoardNo);

	// ������ ������ �����մϴ�.
	DWORD  __stdcall AxlMonitorStart(long lBoardNo, DWORD dwStartOption, DWORD dwOverflowOption);

	// ������ ������ �����մϴ�.
	DWORD  __stdcall AxlMonitorStop(long lBoardNo);

	// ������ �����͸� �����ɴϴ�.
	DWORD  __stdcall AxlMonitorReadData(long lBoardNo, long* lpItemSize, long* lpDataCount, double* dpReadData);

	// ������ ������ �ֱ⸦ �����ɴϴ�.
	DWORD  __stdcall AxlMonitorReadPeriod(long lBoardNo, DWORD* dwpPeriod);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// MonitorEx  
	// �����͸� ������ ������ �׸��� �߰��մϴ�.
	DWORD  __stdcall AxlMonitorExSetItem(long lItemIndex, DWORD dwSignalType, long lSignalNo, long lSubSignalNo);

	// ������ ������ ������ �׸�鿡 ���� ������ �����ɴϴ�.
	DWORD  __stdcall AxlMonitorExGetIndexInfo(long* lpItemSize, long* lpItemIndex);

	// ������ ������ ������ �� �׸��� ���� ������ �����ɴϴ�.
	DWORD  __stdcall AxlMonitorExGetItemInfo(long lItemIndex, DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo);

	// ��� ������ ���� �׸��� ������ �ʱ�ȭ�մϴ�.
	DWORD  __stdcall AxlMonitorExResetAllItem();

	// ���õ� ������ ���� �׸��� ������ �ʱ�ȭ�մϴ�.
	DWORD  __stdcall AxlMonitorExResetItem(long lItemIndex);

	// ������ ������ Ʈ���� ������ �����մϴ�.
	DWORD  __stdcall AxlMonitorExSetTriggerOption(DWORD dwSignalType, long lSignalNo, long lSubSignalNo, DWORD dwOperatorType, double dValue1, double dValue2);

	// ������ ������ Ʈ���� ������ �����ɴϴ�.
	//DWORD  __stdcall AxlMonitorExGetTriggerOption(DWORD* dwpSignalType, long* lpSignalNo, long* lpSubSignalNo, DWORD* dwpOperatorType, double* dpValue1, double* dpValue2);

	// ������ ������ Ʈ���� ������ �ʱ�ȭ�մϴ�.
	DWORD  __stdcall AxlMonitorExResetTriggerOption();

	// ������ ������ �����մϴ�.
	DWORD  __stdcall AxlMonitorExStart(DWORD dwStartOption, DWORD dwOverflowOption);

	// ������ ������ �����մϴ�.
	DWORD  __stdcall AxlMonitorExStop();

	// ������ �����͸� �����ɴϴ�.
	DWORD  __stdcall AxlMonitorExReadData(long* lpItemSize, long* lpDataCount, double* dpReadData);

	// ������ ������ �ֱ⸦ �����ɴϴ�.
	DWORD  __stdcall AxlMonitorExReadPeriod(DWORD* dwpPeriod);
	//////////////////////////////////////////////////////////////////////////

	// X2, Y2 �࿡ ���� Offset ��ġ ������ ������ 2�� ���� ���� #01.
	DWORD  __stdcall AxmLineMoveDual01(long lCoordNo, double* dpEndPosition, double dVelocity, double dAccel, double dDecel, double dOffsetLength, double dTotalLength, double* dpStartOffsetPosition, double* dpEndOffsetPosition);
	// X2, Y2 �࿡ ���� Offset ��ġ ������ ������ 2�� ��ȣ ���� #01.
	DWORD  __stdcall AxmCircleCenterMoveDual01(long lCoordNo, long* lpAxes, double* dpCenterPosition, double* dpEndPosition, double dVelocity, double dAccel, double dDecel, DWORD dwCWDir, double dOffsetLength, double dTotalLength, double* dpStartOffsetPosition, double* dpEndOffsetPosition);

	// �ش纸���� connect mode �� ��ȯ�Ѵ�.
	// dpMode : 1 Auto Connect Mode
	// dpMode : 0 Manual Connect Mode
	DWORD	__stdcall AxlGetBoardConnectMode(long lBoardNo, DWORD *dwpMode);
	// �ش纸���� connect mode �� �����Ѵ�.
	// dMode : 1 Auto Connect Mode
	// dMode : 0 Manual Connect Mode	
	DWORD	__stdcall AxlSetBoardConnectMode(long lBoardNo, DWORD dwMode);
	
	//������ ���� Command Queue �� �ʱ�ȭ �Ѵ�.
	DWORD	__stdcall AxmStatusSetCmdQueueClear(long lAxisNo);
	
	// ������ ���� ��� �������ݰ��� Data �� Ȯ���Ѵ�.	
	DWORD	__stdcall AxmStatusGetControlBits(long lAxisNo, DWORD* dwpTxData, DWORD* dwpRxData);

	// ��� ���� AXL�� �ִ��� Ȯ��(Shared Memory�� �����ϴ��� Ȯ��)
	BOOL	__stdcall AxlIsUsing();
	DWORD	__stdcall AxlRescanExternalDevice();
	DWORD	__stdcall AxlGetExternalDeviceInfo(long lBoardNo, void* devInfo);  //void* -> ExternalDeviceInfo*

	// EzConfig�� �����ϴ� Ȯ�� ��� ����� ���� �Լ�
	// (PCIe-Rxx05-MLIII, dwFunction: 0) ==
	//		[0]: ActOffset ���� ��� Ȱ��ȭ ����(dwValue: [0] Disable, [1] Enable)
	DWORD	__stdcall AxlSetExFunction(DWORD dwFunction, DWORD dwValue);
	DWORD	__stdcall AxlGetExFunction(DWORD dwFunction, DWORD *dwpValue);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXDEV_H__
