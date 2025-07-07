/****************************************************************************
*****************************************************************************
**
** File Name
** ----------
**
** AXC.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Counter Library Header File
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

#ifndef __AXT_AXC_H__
#define __AXT_AXC_H__

#include "source/header/AXHD.h"
#include "source/header/AXHS.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//========== ���� �� ��� ���� 
    // CNT ����� �ִ��� Ȯ��
    DWORD __stdcall AxcInfoIsCNTModule(DWORD *upStatus);
    
    // CNT ��� No Ȯ��
    DWORD __stdcall AxcInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    
    // CNT ����� ����� ���� Ȯ��
    DWORD __stdcall AxcInfoGetModuleCount(long *lpModuleCount);

    // ������ ����� ī���� �Է� ä�� ���� Ȯ��
    DWORD __stdcall AxcInfoGetChannelCount(long lModuleNo, long *lpCount);
    
    // �ý��ۿ� ������ ī������ �� ä�� ���� Ȯ��
    DWORD __stdcall AxcInfoGetTotalChannelCount(long *lpChannelCount);    

    // ������ ��� ��ȣ�� ���̽� ���� ��ȣ, ��� ��ġ, ��� ID Ȯ��
    DWORD __stdcall AxcInfoGetModule(long lModuleNo, long *lpBoardNo, long *lpModulePos, DWORD *upModuleID);

    // �ش� ����� ��� ������ �������� ��ȯ�Ѵ�.
    DWORD __stdcall AxcInfoGetModuleStatus(long lModuleNo);

    DWORD __stdcall AxcInfoGetFirstChannelNoOfModuleNo(long lModuleNo, long *lpChannelNo);
	DWORD __stdcall AxcInfoGetModuleNoOfChannelNo(long lChannelNo, long *lpModuleNo);

    // ī���� ����� Encoder �Է� ����� ���� �մϴ�.
    // dwMethod --> 0x00 : Sign and pulse, x1 multiplication
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // dwMethod --> 0x08 : Sign and pulse, x2 multiplication
    // dwMethod --> 0x09 : Increment and decrement pulses, x1 multiplication
    // dwMethod --> 0x0A : Increment and decrement pulses, x2 multiplication
    // SIO-CN2CH/HPC4�� ���
    // dwMethod --> 0x00 : Up/Down ���, A phase : �޽�, B phase : ����
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // SIO-LCM4�� ���
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication    
    DWORD __stdcall AxcSignalSetEncInputMethod(long lChannelNo, DWORD dwMethod);

    // ī���� ����� Encoder �Է� ����� ���� �մϴ�.
    // *dwpUpMethod --> 0x00 : Sign and pulse, x1 multiplication
    // *dwpUpMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // *dwpUpMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // *dwpUpMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // *dwpUpMethod --> 0x08 : Sign and pulse, x2 multiplication
    // *dwpUpMethod --> 0x09 : Increment and decrement pulses, x1 multiplication
    // *dwpUpMethod --> 0x0A : Increment and decrement pulses, x2 multiplication
    // SIO-CN2CH/HPC4�� ���
    // dwMethod --> 0x00 : Up/Down ���, A phase : �޽�, B phase : ����
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication
    // SIO-LCM4�� ���
    // dwMethod --> 0x01 : Phase-A and phase-B pulses, x1 multiplication
    // dwMethod --> 0x02 : Phase-A and phase-B pulses, x2 multiplication
    // dwMethod --> 0x03 : Phase-A and phase-B pulses, x4 multiplication       
    DWORD __stdcall AxcSignalGetEncInputMethod(long lChannelNo, DWORD* dwpUpMethod);

    // ī���� ����� Ʈ���Ÿ� ���� �մϴ�.
    // dwMode -->  0x00 : Latch
    // dwMode -->  0x01 : State
    // dwMode -->  0x02 : Special State    --> SIO-CN2CH ����
    // SIO-CN2CH�� ���
    // dwMode -->  0x00 : ���� ��ġ Ʈ���� �Ǵ� �ֱ� ��ġ Ʈ����.
    // ���� : ��ǰ���� ����� ���� �ٸ��� ������ �����Ͽ� ��� �ʿ�.
    // dwMode -->  0x01 : �ð� �ֱ� Ʈ����(AxcTriggerSetFreq�� ����)
    // SIO-HPC4�� ���
    // dwMode -->  0x00 : timer mode with counter & frequncy.
    // dwMode -->  0x01 : timer mode.
    // dwMode -->  0x02 : absolute mode[with fifo].
    // dwMode -->  0x03 : periodic mode.[Default]
    DWORD __stdcall AxcTriggerSetFunction(long lChannelNo, DWORD dwMode);

    // ī���� ����� Ʈ���� ������ Ȯ�� �մϴ�.
    // *dwMode -->  0x00 : Latch
    // *dwMode -->  0x01 : State
    // *dwMode -->  0x02 : Special State
    // SIO-CN2CH�� ���
    // *dwMode -->  0x00 : ���� ��ġ Ʈ���� �Ǵ� �ֱ� ��ġ Ʈ����.
    // ���� : ��ǰ���� ����� ���� �ٸ��� ������ �����Ͽ� ��� �ʿ�.
    // *dwMode -->  0x01 : �ð� �ֱ� Ʈ����(AxcTriggerSetFreq�� ����)
    // SIO-HPC4�� ���
    // *dwMode -->  0x00 : timer mode with counter & frequncy.
    // *dwMode -->  0x01 : timer mode.
    // *dwMode -->  0x02 : absolute mode[with fifo].
    // *dwMode -->  0x03 : periodic mode.[Default]
    DWORD __stdcall AxcTriggerGetFunction(long lChannelNo, DWORD* dwpMode);

    // dwUsage --> 0x00 : Trigger Not use
    // dwUsage --> 0x01 : Trigger use
    DWORD __stdcall AxcTriggerSetNotchEnable(long lChannelNo, DWORD dwUsage);

    // *dwUsage --> 0x00 : Trigger Not use
    // *dwUsage --> 0x01 : Trigger use
    DWORD __stdcall AxcTriggerGetNotchEnable(long lChannelNo, DWORD* dwpUsage);
    

    // ī���� ����� Capture �ؼ��� ���� �մϴ�.(External latch input polarity)
    // dwCapturePol --> 0x00 : Signal OFF -> ON
    // dwCapturePol --> 0x01 : Signal ON -> OFF
    DWORD __stdcall AxcSignalSetCaptureFunction(long lChannelNo, DWORD dwCapturePol);

    // ī���� ����� Capture �ؼ� ������ Ȯ�� �մϴ�.(External latch input polarity)
    // *dwCapturePol --> 0x00 : Signal OFF -> ON
    // *dwCapturePol --> 0x01 : Signal ON -> OFF
    DWORD __stdcall AxcSignalGetCaptureFunction(long lChannelNo, DWORD* dwpCapturePol);

    // ī���� ����� Capture ��ġ�� Ȯ�� �մϴ�.(External latch)
    // *dbpCapturePos --> Capture position ��ġ
    DWORD __stdcall AxcSignalGetCapturePos(long lChannelNo, double* dbpCapturePos);

    // ī���� ����� ī���� ���� Ȯ�� �մϴ�.
    // *dbpActPos --> ī���� ��
    DWORD __stdcall AxcStatusGetActPos(long lChannelNo, double* dbpActPos);

    // ī���� ����� ī���� ���� dbActPos ������ ���� �մϴ�.
    DWORD __stdcall AxcStatusSetActPos(long lChannelNo, double dbActPos);

    // ī���� ����� Ʈ���� ��ġ�� �����մϴ�.
    // ī���� ����� Ʈ���� ��ġ�� 2�������� ���� �� �� �ֽ��ϴ�.
    DWORD __stdcall AxcTriggerSetNotchPos(long lChannelNo, double dbLowerPos, double dbUpperPos);

    // ī���� ����� ������ Ʈ���� ��ġ�� Ȯ�� �մϴ�.
    DWORD __stdcall AxcTriggerGetNotchPos(long lChannelNo, double* dbpLowerPos, double* dbpUpperPos);

    // ī���� ����� Ʈ���� ����� ������ �մϴ�.
    // dwOutVal --> 0x00 : Ʈ���� ��� '0'
    // dwOutVal --> 0x01 : Ʈ���� ��� '1'
    DWORD __stdcall AxcTriggerSetOutput(long lChannelNo, DWORD dwOutVal);

    // ī���� ����� ���¸� Ȯ���մϴ�.
    // Bit '0' : Carry (ī���� ����ġ�� ���� �޽��� ���� ī���� ����ġ�� �Ѿ� 0�� �ٲ���� �� 1��ĵ�� ON���� �մϴ�.)
    // Bit '1' : Borrow (ī���� ����ġ�� ���� �޽��� ���� 0�� �Ѿ� ī���� ����ġ�� �ٲ���� �� 1��ĵ�� ON���� �մϴ�.)
    // Bit '2' : Trigger output status
    // Bit '3' : Latch input status
    DWORD __stdcall AxcStatusGetChannel(long lChannelNo, DWORD* dwpChannelStatus);


    // SIO-CN2CH ���� �Լ���
    //
    // ī���� ����� ��ġ ������ �����Ѵ�.
    // ���� ��ġ �̵����� ���� �޽� ������ �����ϴµ�,
    // ex) 1mm �̵��� 1000�� �ʿ��ϴٸ�dMoveUnitPerPulse = 0.001�� �Է��ϰ�,
    //     ���� ��� �Լ��� ��ġ�� ���õ� ���� mm ������ �����ϸ� �ȴ�.
    DWORD __stdcall AxcMotSetMoveUnitPerPulse(long lChannelNo, double dMoveUnitPerPulse);

    // ī���� ����� ��ġ ������ Ȯ���Ѵ�.
    DWORD __stdcall AxcMotGetMoveUnitPerPulse(long lChannelNo, double* dpMoveUnitPerPuls);

    // ī���� ����� ���ڴ� �Է� ī���͸� ���� ����� �����Ѵ�.
    // dwReverse --> 0x00 : �������� ����.
    // dwReverse --> 0x01 : ����.
    DWORD __stdcall AxcSignalSetEncReverse(long lChannelNo, DWORD dwReverse);

    // ī���� ����� ���ڴ� �Է� ī���͸� ���� ����� ������ Ȯ���Ѵ�.
    DWORD __stdcall AxcSignalGetEncReverse(long lChannelNo, DWORD* dwpReverse);

    // ī���� ����� Encoder �Է� ��ȣ�� �����Ѵ�.
    // dwSource -->  0x00 : 2(A/B)-Phase ��ȣ.
    // dwSource -->  0x01 : Z-Phase ��ȣ.(���⼺ ����.)
    DWORD  __stdcall AxcSignalSetEncSource(long lChannelNo, DWORD dwSource);

    // ī���� ����� Encoder �Է� ��ȣ ���� ������ Ȯ���Ѵ�.
    DWORD  __stdcall AxcSignalGetEncSource(long lChannelNo, DWORD* dwpSource);

    // ī���� ����� Ʈ���� ��� ���� �� ���� ���� �����Ѵ�.
    // ��ġ �ֱ� Ʈ���� ��ǰ�� ��� ��ġ �ֱ�� Ʈ���� ����� �߻���ų ���� �� ���� ���� �����Ѵ�.
    // ���� ��ġ Ʈ���� ��ǰ�� ��� Ram ���� ������ Ʈ���� ������ ���� ���� ��ġ�� �����Ѵ�.
    // ���� : AxcMotSetMoveUnitPerPulse�� ������ ����.
    // Note : ���Ѱ��� ���Ѱ����� �������� �����Ͽ��� �մϴ�.
    DWORD  __stdcall AxcTriggerSetBlockLowerPos(long lChannelNo, double dLowerPosition);

    // ī���� ����� Ʈ���� ��� ���� �� ���� ���� Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetBlockLowerPos(long lChannelNo, double* dpLowerPosition);

    // ī���� ����� Ʈ���� ��� ���� �� ���� ���� �����Ѵ�.
    // ��ġ �ֱ� Ʈ���� ��ǰ�� ��� ��ġ �ֱ�� Ʈ���� ����� �߻���ų ���� �� ���� ���� �����Ѵ�.
    // ���� ��ġ Ʈ���� ��ǰ�� ��� Ʈ���� ������ ������ Ram �� ������ ������ Ʈ���� ������ ����Ǵ� ��ġ�� ���ȴ�.
    // ���� : AxcMotSetMoveUnitPerPulse�� ������ ����.
    // Note : ���Ѱ��� ���Ѱ����� ū���� �����Ͽ��� �մϴ�.
    DWORD  __stdcall AxcTriggerSetBlockUpperPos(long lChannelNo, double dUpperPosition);
    // ī���� ����� Ʈ���� ��� ���� �� ���� ���� �����Ѵ�.
    DWORD  __stdcall AxcTriggerGetBlockUpperPos(long lChannelNo, double* dpUpperrPosition);

    // ī���� ����� ��ġ �ֱ� ��� Ʈ���ſ� ���Ǵ� ��ġ �ֱ⸦ �����Ѵ�.
    // ���� : AxcMotSetMoveUnitPerPulse�� ������ ����.
    DWORD  __stdcall AxcTriggerSetPosPeriod(long lChannelNo, double dPeriod);

    // ī���� ����� ��ġ �ֱ� ��� Ʈ���ſ� ���Ǵ� ��ġ �ֱ⸦ Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetPosPeriod(long lChannelNo, double* dpPeriod);

    // ī���� ����� ��ġ �ֱ� ��� Ʈ���� ���� ��ġ ������ ���� ��ȿ����� �����Ѵ�.
    // dwDirection -->  0x00 : ī������ ��/���� ���Ͽ� Ʈ���� �ֱ� ���� ���.
    // dwDirection -->  0x01 : ī���Ͱ� ���� �Ҷ��� Ʈ���� �ֱ� ���� ���.
    // dwDirection -->  0x01 : ī���Ͱ� ���� �Ҷ��� Ʈ���� �ֱ� ���� ���.
    DWORD  __stdcall AxcTriggerSetDirectionCheck(long lChannelNo, DWORD dwDirection);
    // ī���� ����� ��ġ �ֱ� ��� Ʈ���� ���� ��ġ ������ ���� ��ȿ��� ������ Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetDirectionCheck(long lChannelNo, DWORD* dwpDirection);

    // ī���� ����� ��ġ �ֱ� ��� Ʈ���� ����� ����, ��ġ �ֱ⸦ �ѹ��� �����Ѵ�.
    // ��ġ ���� ���� :  AxcMotSetMoveUnitPerPulse�� ������ ����.
    DWORD  __stdcall AxcTriggerSetBlock(long lChannelNo, double dLower, double dUpper, double dABSod);

    // ī���� ����� ��ġ �ֱ� ��� Ʈ���� ����� ����, ��ġ �ֱ⸦ ������ �ѹ��� Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetBlock(long lChannelNo, double *dpLower, double *dpUpper, double *dpABSod);

    // ī���� ����� Ʈ���� ��� �޽� ���� �����Ѵ�.
    // ���� : uSec
    DWORD  __stdcall AxcTriggerSetTime(long lChannelNo, double dTrigTime);

    // ī���� ����� Ʈ���� ��� �޽� �� ������ Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetTime(long lChannelNo, double* dpTrigTime);

    // ī���� ����� Ʈ���� ��� �޽��� ��� ������ �����Ѵ�.
    // dwLevel -->  0x00 : Ʈ���� ��½� 'Low' ���� ���.
    // dwLevel -->  0x01 : Ʈ���� ��½� 'High' ���� ���.
    DWORD  __stdcall AxcTriggerSetLevel(long lChannelNo, DWORD dwLevel);
    // ī���� ����� Ʈ���� ��� �޽��� ��� ���� ������ Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetLevel(long lChannelNo, DWORD* dwpLevel);

    // ī���� ����� ���ļ� Ʈ���� ��� ��ɿ� �ʿ��� ���ļ��� �����Ѵ�.
    // ���� : Hz, ���� : 1Hz ~ 500 kHz
    DWORD  __stdcall AxcTriggerSetFreq(long lChannelNo, DWORD dwFreqency);
    // ī���� ����� ���ļ� Ʈ���� ��� ��ɿ� �ʿ��� ���ļ��� ������ Ȯ���Ѵ�.
    DWORD  __stdcall AxcTriggerGetFreq(long lChannelNo, DWORD* dwpFreqency);

    // ī���� ����� ���� ä�ο� ���� ���� ��� ���� �����Ѵ�.
    // dwOutput ���� : 0x00 ~ 0x0F, �� ä�δ� 4���� ���� ���
    DWORD  __stdcall AxcSignalWriteOutput(long lChannelNo, DWORD dwOutput);

    // ī���� ����� ���� ä�ο� ���� ���� ��� ���� Ȯ���Ѵ�.
    DWORD  __stdcall AxcSignalReadOutput(long lChannelNo, DWORD* dwpOutput);

    // ī���� ����� ���� ä�ο� ���� ���� ��� ���� ��Ʈ ���� �����Ѵ�.
    // lBitNo ���� : 0 ~ 3, �� ä�δ� 4���� ���� ���
    DWORD  __stdcall AxcSignalWriteOutputBit(long lChannelNo, long lBitNo, DWORD uOnOff);
    // ī���� ����� ���� ä�ο� ���� ���� ��� ���� ��Ʈ ���� Ȯ�� �Ѵ�.
    // lBitNo ���� : 0 ~ 3
    DWORD  __stdcall AxcSignalReadOutputBit(long lChannelNo,long lBitNo, DWORD *upOnOff);

    // ī���� ����� ���� ä�ο� ���� ���� �Է� ���� Ȯ���Ѵ�.
    DWORD  __stdcall AxcSignalReadInput(long lChannelNo, DWORD* dwpInput);

    // ī���� ����� ���� ä�ο� ���� ���� �Է� ���� ��Ʈ ���� Ȯ�� �Ѵ�.
    // lBitNo ���� : 0 ~ 3
    DWORD  __stdcall AxcSignalReadInputBit(long lChannelNo, long lBitNo, DWORD *upOnOff);

    // ī���� ����� Ʈ���� ����� Ȱ��ȭ �Ѵ�.
    // ���� ������ ��ɿ� ���� Ʈ���� ����� ���������� ����� ������ �����Ѵ�.
    DWORD  __stdcall AxcTriggerSetEnable(long lChannelNo, DWORD dwUsage);

    // ī���� ����� Ʈ���� ��� Ȱ��ȭ ���� ������ Ȯ���ϴ�.
    DWORD  __stdcall AxcTriggerGetEnable(long lChannelNo, DWORD* dwpUsage);

    // ī���� ����� ������ġ Ʈ���� ����� ���� ������ RAM ������ Ȯ���Ѵ�.
    // dwAddr ���� : 0x0000 ~ 0x1FFFF;
    DWORD  __stdcall AxcTriggerReadAbsRamData(long lChannelNo, DWORD dwAddr, DWORD* dwpData);

    // ī���� ����� ������ġ Ʈ���� ����� ���� �ʿ��� RAM ������ �����Ѵ�.
    // dwAddr ���� : 0x0000 ~ 0x1FFFF;
    DWORD  __stdcall AxcTriggerWriteAbsRamData(long lChannelNo, DWORD dwAddr, DWORD dwData);

	// ���� CNT ä���� ���� ��ġ Ʈ���� ����� ���� DWORD�� ��ġ ������ �����Ѵ�.
	//----------------------------------------------------------------------------------------------------------------------------------
	// 1. AXT_SIO_CN2CH�� ���
	// dwTrigNum	--> 131072(=0x20000) ������ ���� ����
	// dwTrigPos	--> DWORD�� Data �Է� ����
	// dwDirection	--> 0x0(default) : dwTrigPos[0], dwTrigPos[1] ..., dwTrigPos[dwTrigNum - 1] ������ Data�� Write �Ѵ�.
	//					0x1			 : dwTrigPos[dwTrigNum - 1], dwTrigPos[dwTrigNum - 2], ..., dwTrigPos[0] ������ Data�� Write �Ѵ�.
	// *����* 1) dwDirection: Data Write ������ �ٸ� �� ��ɻ��� ���� ����
	//		  2) AXC Manual�� AxcTriggerSetAbs - Description�� �����Ͽ� data�� ���� �� ����ؾ� ��
	//----------------------------------------------------------------------------------------------------------------------------------
	// 2. AXT_SIO_HPC4�� ���
	// dwTrigNum	--> 500 ������ ���� ����
	// dwTrigPos	--> DWORD�� Data �Է� ����
	// dwDirection	--> 0x0(default) : ������ �ʴ� ������, �Է����� �ʾƵ� �ȴ�.
	//----------------------------------------------------------------------------------------------------------------------------------
	// 3. AXT_SIO_RCNT2RTEX, AXT_SIO_RCNT2MLIII, AXT_SIO_RCNT2SIIIH, AXT_SIO_RCNT2SIIIH_R�� ���
	// dwTrigNum	--> 0x200(=512) ������ ���� ����
	// dwTrigPos	--> DWORD�� data �Է� ����
	// dwDirection	--> 0x0(default) : ������ �ʴ� ������, �Է����� �ʾƵ� �ȴ�.
	//----------------------------------------------------------------------------------------------------------------------------------
    DWORD  __stdcall AxcTriggerSetAbs(long lChannelNo, DWORD dwTrigNum, DWORD* dwTrigPos, DWORD dwDirection = 0);


	// ���� CNT ä���� ���� ��ġ Ʈ���� ����� ���� double�� ��ġ ������ �����Ѵ�.
	//----------------------------------------------------------------------------------------------------------------------------------
	// 1. AXT_SIO_CN2CH�� ���
	// dwTrigNum	--> 4194304(=0x20000*32) ������ ���� ����
	// dTrigPos		--> double�� data �Է� ����
	// dwDirection	--> 0x0(default) : dTrigPos[0], dTrigPos[1] ..., dTrigPos[dwTrigNum - 1] ������ Data�� Write �Ѵ�.
	//					0x1			 : dTrigPos[dwTrigNum - 1], dTrigPos[dwTrigNum - 2], ..., dTrigPos[0] ������ Data�� Write �Ѵ�.
	// *����* 1) dwDirection: Data Write ������ �ٸ� �� ��ɻ��� ���� ����
	//----------------------------------------------------------------------------------------------------------------------------------
	// 2. AXT_SIO_RCNT2RTEX, AXT_SIO_RCNT2MLIII, AXT_SIO_RCNT2SIIIH_R�� ���
	// dwTrigNum	--> 0x200(=512) ������ ���� ����
	// dTrigPos		--> double�� data �Է� ����
	// dwDirection	--> 0x0(default) : ������ �ʴ� ������, �Է����� �ʾƵ� �ȴ�.
	//----------------------------------------------------------------------------------------------------------------------------------
    DWORD  __stdcall AxcTriggerSetAbsDouble(long lChannelNo, DWORD dwTrigNum, double* dTrigPos, DWORD dwDirection = 0);

	////////////////// LCM4_10_Version/////////////////////////////////////////////////////////////

	// ī���� ����� PWM ����� Ȱ��ȭ�Ѵ�.
	DWORD __stdcall AxcTriggerSetPwmEnable(long lChannelNo, bool bEnable);
	// ī���� ����� PWM ��� Ȱ��ȭ ���¸� Ȯ���Ѵ�.
	DWORD __stdcall AxcTriggerGetPwmEnable(long lChannelNo, bool *bEnable);
	// ī���� ����� PWM ��¸�带 �����Ѵ�.
	// dwMode : PWM ��¸��
	// [0] : Manual (Manual�� ������ PWM Data)
	// [1] : Auto (�ӵ� Table)
	DWORD __stdcall AxcTriggerSetPwmOutMode(long lChannelNo, DWORD dwMode); 
	// ī���� ����� PWM ��¸�带 Ȯ���Ѵ�.
	// dwMode : PWM ��¸��
	// [0] : Manual (Manual�� ������ PWM Data)
	// [1] : Auto (�ӵ� Table)
	DWORD __stdcall AxcTriggerGetPwmOutMode(long lChannelNo, DWORD* dwpMode);	

	// ī���� ����� �� ���̺��� 2-D ����ӵ����� PWM ��ȣ�� ����ϱ� ���� �ʿ��� �ӵ� ������ �����Ѵ�.
	// dMinVel : dMinVel
	// dMaxVel : dMaxVel 
	// dVelInterval : �ӵ� ���̺������� ������ �ӵ� Interval
	// ������ : dMinVel���� dVelInterval �������� �ִ� 5000���� �ӵα����� ������.  
	//          (((dMaxVel-dMinVel) / dVelInterval) <= 5000)�� �����Ͽ��� �Ѵ�. 
	DWORD __stdcall AxcTriggerSetPwmVelInfo(long lChannelNo, double dMinVel, double dMaxVel, double dVelInterval);
	// ī���� ����� �� ���̺��� 2-D ����ӵ����� PWM ��ȣ�� ����ϱ� ���� �ʿ��� �ӵ� ������ Ȯ���Ѵ�.
	DWORD __stdcall AxcTriggerGetPwmVelInfo(long lChannelNo, double* dpMinVel, double* dpMaxVel, double* dpVelInterval);
	//	ī���� ����� PWM ��¿��� Pulse �� �������� �����Ѵ�.
	// dwMode : Pulse �� ���� 
	// [0] : DutyRatio
	// [1] : PulseWidth
	DWORD __stdcall AxcTriggerSetPwmPulseControl(long lChannelNo, DWORD dwMode);
	// 	ī���� ����� PWM ��¿��� Pulse ��������� Ȯ�δ�.
	// dwpMode : Pulse �� ���� 
	// [0] : DutyRatio
	// [1] : PulseWidth	
	DWORD __stdcall AxcTriggerGetPwmPulseControl(long lChannelNo, DWORD *dwpMode);

	// ī���� ����� �� ���̺��� 2-D ����ӵ����� PWM ��ȣ�� ����ϱ� ���� �ʿ��� ������ �����Ѵ�.
	// pwm ��¸�尡 Manual �� ��쿡�� ��ȿ�ϴ�
	// dFrequency :  (0.017 ~ 1M) ���� ������ �����ϴ�. (Hz ����)
	// dData : Pulse �� ���� Data �� �Է��ϸ� Pulse Control ��Ŀ� ���� Data ������ �ٸ���.
	// Pulse �� �������� DutyRatio�� ��� DutyRatio 
	// Pulse �� �������� PulseWidth �� ��� PulseWidth (us����) 
	DWORD __stdcall AxcTriggerSetPwmManualData(long lChannelNo,double dFrequency, double dData);
	// ī���� ����� �� ���̺��� 2-D ����ӵ����� PWM ��ȣ�� ����ϱ� ���� �ʿ��� ������ Ȯ���Ѵ�.
	DWORD __stdcall AxcTriggerGetPwmManualData(long lChannelNo,double* dpFrequency, double* dpData);
	// ī���� ����� �� ���̺��� 2-D ����ӵ����� PWM ��ȣ�� ����ϱ� ���� �ʿ��� ������ �����Ѵ�.
	// lDataCnt : ���� �� Ʈ���� ������ ��ü ����
	// dpVel : dpVel[0],dpVel[1]....dpVel[DataCnt -1] ������ �Է� ����
	// dwpFrequency : dwpFrequency[0],dwpFrequency[1]....dwpFrequency[DataCnt-1] ������ �Է� ����(0.017 ~ 1M) ���� ������ �����ϴ�.
	// dData : Pulse �� ���� Data �� �Է��ϸ� Pulse Control ��Ŀ� ���� Data ������ �ٸ���.
	// Pulse �� �������� DutyRatio�� ��� DutyRatio 
	// Pulse �� �������� PulseWidth �� ��� PulseWidth (us����) 
	// ������ : 
	//    1) dpVel, dwpFrequency, dwpDutyRatio �� �迭 ������ �����Ͽ� ����ؾ��Ѵ�.
	//		- �ӵ��� 0�� ���������� PWM ����� �Ұ��ϴ�.
	//    3) PWM Enable ���¿����� ����� �� ����.
	DWORD __stdcall AxcTriggerSetPwmPatternData(long lChannelNo, long lDataCnt,  double *dpVel, double *dpFrequency, double *dpData);
	// ī���� ����� �� ���̺��� 2-D ����ӵ����� PWM ��ȣ�� ����ϱ� ���� �ʿ��� ������ �����Ѵ�.
	DWORD __stdcall AxcTriggerSetPwmData(long lChannelNo, double dVel, double dFrequency, double dData);
	// ī���� ����� �� ���̺��� 2-D ����ӵ����� PWM ��ȣ�� ����ϱ� ���� �ʿ��� ������ Ȯ���Ѵ�.
	DWORD __stdcall AxcTriggerGetPwmData(long lChannelNo, double dVel, double *dpFrequency, double *dpData);
	// ī���� ����� �ӵ� ���� Ȯ�� �մϴ�.
	DWORD __stdcall AxcStatusReadActVel(long lChannelNo, double* dpActVel);
	// ī���� ����� 2D �ӵ� ���� Ȯ�� �մϴ�.
	DWORD __stdcall AxcStatusRead2DActVel(long lChannelNo, double* dpActVel);
	// ī���� ����� Position ���� �ʱ�ȭ �Ѵ�.
	DWORD __stdcall AxcStatusSetActPosClear(long lChannelNo);	
	////////////////// HPC4_30_Version
	// ī���� ����� �� ���̺��� �Ҵ�� Ʈ���� ����� ������ �����Ѵ�.
	// uLevel : Ʈ���� ��� ��ȣ�� Active Level
	//   [0]  : Ʈ���� ��½� 'Low' ���� ���.
	//   [1]  : Ʈ���� ��½� 'High' ���� ���.
    DWORD  __stdcall AxcTableSetTriggerLevel(long lModuleNo, long lTablePos, DWORD uLevel);
	// ī���� ����� �� ���̺��� ������ Ʈ���� ����� ���� �������� Ȯ���Ѵ�.
    DWORD  __stdcall AxcTableGetTriggerLevel(long lModuleNo, long lTablePos, DWORD* upLevel);

	// ī���� ����� �� ���̺��� �Ҵ�� Ʈ���� ����� �޽� ���� �����Ѵ�.
	// dTriggerTimeUSec : [Default 500ms], us������ ����
	DWORD  __stdcall AxcTableSetTriggerTime(long lModuleNo, long lTablePos, double dTriggerTimeUSec);
	// ī���� ����� �� ���̺��� ������ Ʈ���� ����� �޽� �� �������� Ȯ���Ѵ�.
    DWORD  __stdcall AxcTableGetTriggerTime(long lModuleNo, long lTablePos, double* dpTriggerTimeUSec);

	// ī���� ����� �� ���̺��� �Ҵ� �� 2���� ���ڴ� �Է� ��ȣ�� �����Ѵ�.
	// uEncoderInput1 [0-3]: ī���� ��⿡ �ԷµǴ� 4���� ���ڴ� ��ȣ�� �ϳ�
	// uEncoderInput2 [0-3]: ī���� ��⿡ �ԷµǴ� 4���� ���ڴ� ��ȣ�� �ϳ�
	DWORD  __stdcall AxcTableSetEncoderInput(long lModuleNo, long lTablePos, DWORD uEncoderInput1, DWORD uEncoderInput2);
	// ī���� ����� �� ���̺��� �Ҵ� �� 2���� ���ڴ� �Է� ��ȣ�� Ȯ���Ѵ�.
	DWORD  __stdcall AxcTableGetEncoderInput(long lModuleNo, long lTablePos, DWORD* upEncoderInput1, DWORD* upEncoderInput2);

	// ī���� ����� �� ���̺��� �Ҵ� �� Ʈ���� ��� ��Ʈ�� �����Ѵ�.
	// uTriggerOutport [0x0-0xF]: Bit0: Ʈ���� ��� 0, Bit1: Ʈ���� ��� 1, Bit2: Ʈ���� ��� 2, Bit3: Ʈ���� ��� 3 
	// Ex) 0x3(3)   : ��� 0, 1�� Ʈ���� ��ȣ�� ����ϴ� ���
	//     0xF(255) : ��� 0, 1, 2, 3�� Ʈ���� ��ȣ�� ����ϴ� ���
	DWORD  __stdcall AxcTableSetTriggerOutport(long lModuleNo, long lTablePos, DWORD uTriggerOutport);
	// ī���� ����� �� ���̺��� �Ҵ� �� Ʈ���� ��� ��Ʈ�� Ȯ���Ѵ�.
	DWORD  __stdcall AxcTableGetTriggerOutport(long lModuleNo, long lTablePos, DWORD* upTriggerOutport);

	// ī���� ����� �� ���̺��� ������ Ʈ���� ��ġ�� ���� ��� ���� ������ �����Ѵ�.
	// dErrorRange  : ���� ���� Unit������ Ʈ���� ��ġ�� ���� ��� ���� ������ ����
	DWORD  __stdcall AxcTableSetErrorRange(long lModuleNo, long lTablePos, double dErrorRange);
	// ī���� ����� �� ���̺��� ������ Ʈ���� ��ġ�� ���� ��� ���� ������ Ȯ���Ѵ�.
	DWORD  __stdcall AxcTableGetErrorRange(long lModuleNo, long lTablePos, double* dpErrorRange);

	// ī���� ����� �� ���̺��� ������ ������(Ʈ���� ��� Port, Ʈ���� �޽� ��) Ʈ���Ÿ� 1�� �߻���Ų��.
	// �� ���� : 1) Ʈ���Ű� Disable�Ǿ� ������ �� �Լ��� �ڵ����� Enable���� Ʈ���Ÿ� �߻���Ŵ 
	//           2) Trigger Mode�� HPC4_PATTERN_TRIGGER ����� ��� �� �Լ��� �ڵ����� Ʈ���� ��带 HPC4_RANGE_TRIGGER�� ���� ��(�ϳ��� Ʈ���Ÿ� �߻���Ű�� ����)
	DWORD  __stdcall AxcTableTriggerOneShot(long lModuleNo, long lTablePos);

	// ī���� ����� �� ���̺��� ������ ������(Ʈ���� ��� Port, Ʈ���� �޽� ��), ������ ������ŭ ������ ���ļ��� Ʈ���Ÿ� �߻���Ų��.
    // lTriggerCount     : ������ ���ļ��� �����ϸ� �߻���ų Ʈ���� ��� ����
	// uTriggerFrequency : Ʈ���Ÿ� �߻���ų ���ļ�	
	// �� ���� : 1) Ʈ���Ű� Disable�Ǿ� ������ �� �Լ��� �ڵ����� Enable���� ������ ���� Ʈ���Ÿ� �߻���Ŵ 
	//           2) Trigger Mode�� HPC4_PATTERN_TRIGGER ��尡 �ƴ� ��� �� �Լ��� �ڵ����� Ʈ���� ��带 HPC4_PATTERN_TRIGGER�� ���� ��
	DWORD  __stdcall AxcTableTriggerPatternShot(long lModuleNo, long lTablePos, long lTriggerCount, DWORD uTriggerFrequency);
	// ī���� ����� �� ���̺��� ������ ���� Ʈ���� ���� ������(���ļ�, ī����) Ȯ���Ѵ�.
	DWORD  __stdcall AxcTableGetPatternShotData(long lModuleNo, long lTablePos, long* lpTriggerCount, DWORD* upTriggerFrequency);

	// ī���� ����� �� ���̺��� Ʈ���Ÿ� ����ϴ� ����� �����Ѵ�.
	// uTrigMode : Ʈ���Ÿ� ����ϴ� ����� �����Ѵ�.
	//   [0] HPC4_RANGE_TRIGGER   : ������ Ʈ���� ��ġ�� ������ ��� �����ȿ� ��ġ�� �� Ʈ���Ÿ� ����ϴ� ���
	//   [1] HPC4_VECTOR_TRIGGER  : ���� Ʈ���� ��ġ�� ������ ��� ������ ���� ������ ��ġ�� �� Ʈ���Ÿ� ����ϴ� ���
	//   [3] HPC4_PATTERN_TRIGGER : ��ġ�� �����ϰ� ������ ������ŭ ������ ���ļ��� Ʈ���Ÿ� ����ϴ� ���
	DWORD  __stdcall AxcTableSetTriggerMode(long lModuleNo, long lTablePos, DWORD uTrigMode);
	// ī���� ����� �� ���̺��� ������ Ʈ���Ÿ� ����ϴ� ����� Ȯ���Ѵ�
	DWORD  __stdcall AxcTableGetTriggerMode(long lModuleNo, long lTablePos, DWORD* upTrigMode);		
	// ī���� ����� �� ���̺� ���� ��µ� ���� Ʈ���� ������ �ʱ�ȭ �Ѵ�.
	DWORD  __stdcall AxcTableSetTriggerCountClear(long lModuleNo, long lTablePos);

	// ī���� ����� �� ���̺��� 2-D ������ġ���� Ʈ���� ��ȣ�� ����ϱ� ���� �ʿ��� ������ �����Ѵ�.
	// lTriggerDataCount : ���� �� Ʈ���� ������ ��ü ����
    //   [-1, 0]         : ��ϵ� Ʈ���� ���� ����Ÿ �ʱ�ȭ
	// dpTriggerData     : 2-D ������ġ Ʈ���� ����(�ش� �迭�� ������ lTriggerDataCount * 2�� �Ǿ�ߵ�)
	//   *[0, 1]         : X[0], Y[0] 
	// lpTriggerCount    : �Է��� 2-D ���� Ʈ���� ��ġ���� Ʈ���� ���� ���� �� �߻���ų Ʈ���� ������ �迭�� ����(�ش� �迭�� ������ lTriggerDataCount)
	// dpTriggerInterval : TriggerCount ��ŭ �����ؼ� Ʈ���Ÿ� �߻���ų�� ���� �� ������ ���ļ� ������ ����(�ش� �迭�� ������ lTriggerDataCount)
	// ������ : 
	//    1) �� ���������� �迭 ������ �����Ͽ� ����ؾߵ˴ϴ�. ���ο��� ���Ǵ� ���� ���� ���� �迭�� �����ϸ� �޸� ���� ������ �߻� �� �� ����.
	//    2) Trigger Mode�� HPC4_RANGE_TRIGGER�� �ڵ� �����
	//    3) �Լ� ���ο��� Trigger�� Disable�� �� ��� ������ �����ϸ� �Ϸ� �� �ٽ� Enable ��Ŵ
	DWORD  __stdcall AxcTableSetTriggerData(long lModuleNo, long lTablePos, long lTriggerDataCount, double* dpTriggerData, long* lpTriggerCount, double* dpTriggerInterval);
	// ī���� ����� �� ���̺��� Ʈ���� ��ȣ�� ����ϱ� ���� ������ Ʈ���� ���� ������ Ȯ���Ѵ�.
	// �� ���� : �� ���̺��� ��ϵ� �ִ� Ʈ���� ����Ÿ ������ �� ���� �Ʒ��� ���� Ʈ���� ����Ÿ ������ �̸� �ľ��� �� ����Ͻʽÿ�.
	// Ex)      1) AxcTableGetTriggerData(lModuleNo, lTablePos, &lTriggerDataCount, NULL, NULL, NULL);
	//          2) dpTriggerData     = new double[lTriggerDataCount * 2];
	//          3) lpTriggerCount    = new long[lTriggerDataCount];
	//          4) dpTriggerInterval = new double[lTriggerDataCount];  
	DWORD  __stdcall AxcTableGetTriggerData(long lModuleNo, long lTablePos, long* lpTriggerDataCount, double* dpTriggerData, long* lpTriggerCount, double* dpTriggerInterval);
	
	// ī���� ����� �� ���̺��� 2-D ������ġ���� Ʈ���� ��ȣ�� ����ϱ� ���� �ʿ��� ������ AxcTableSetTriggerData�Լ��� �ٸ� ������� �����Ѵ�.
	// lTriggerDataCount : ���� �� Ʈ���� ������ ��ü ����
	// uOption : dpTriggerData �迭�� ����Ÿ �Է� ����� ���� 
	//   [0]   : dpTriggerData �迭�� X Pos[0], Y Pos[0], X Pos[1], Y Pos[1] ������ �Է�
	//   [1]   : dpTriggerData �迭�� X Pos[0], Y Pos[0], Count, Inteval, X Pos[1], Y Pos[1], Count, Inteval ������ �Է�
	// ������ : 
	//    1) dpTriggerData�� �迭 ������ �����Ͽ� ����ؾߵ˴ϴ�. ���ο��� ���Ǵ� ���� ���� ���� �迭�� �����ϸ� �޸� ���� ������ �߻� �� �� ����.
	//    2) Trigger Mode�� HPC4_RANGE_TRIGGER�� �ڵ� �����
	//    3) �Լ� ���ο��� Trigger�� Disable�� �� ��� ������ �����ϸ� �Ϸ� �� �ٽ� Enable ��Ŵ
    DWORD  __stdcall AxcTableSetTriggerDataEx(long lModuleNo, long lTablePos, long lTriggerDataCount, DWORD uOption, double* dpTriggerData);
	// ī���� ����� �� ���̺��� Ʈ���� ��ȣ�� ����ϱ� ���� ������ Ʈ���� ���� ������ Ȯ���Ѵ�.
	// �� ���� : �� ���̺��� ��ϵ� �ִ� Ʈ���� ����Ÿ ������ �� ���� �Ʒ��� ���� Ʈ���� ����Ÿ ������ �̸� �ľ��� �� ���.
	// Ex)      1) AxcTableGetTriggerDataEx(lModuleNo, lTablePos, &lTriggerDataCount, &uOption, NULL);
	//          2) if(uOption == 0) : dpTriggerData     = new double[lTriggerDataCount * 2];
	//          3) if(uOption == 1) : dpTriggerData     = new double[lTriggerDataCount * 4];
	//          4) dpTriggerInterval = new double[lTriggerDataCount];  
    DWORD  __stdcall AxcTableGetTriggerDataEx(long lModuleNo, long lTablePos, long* lpTriggerDataCount,  DWORD* upOption, double* dpTriggerData);
  
	// ī���� ����� ������ ���̺��� ������ ��� Ʈ���� ����Ÿ�� H/W FIFO�� ����Ÿ�� ��� �����Ѵ�.
	DWORD  __stdcall AxcTableSetTriggerDataClear(long lModuleNo, long lTablePos);

	// ī���� ����� ������ ���̺��� Ʈ���� ��� ����� ���۽�Ŵ.
	// uEnable : Ʈ���Ÿ� ��� ����� ��뿩�θ� ����
	// �� ���� : 1) Ʈ���� ��� �� DISABLE�ϸ� ����� �ٷ� ����
	//           2) AxcTableTriggerOneShot, AxcTableGetPatternShotData,AxcTableSetTriggerData, AxcTableGetTriggerDataEx �Լ� ȣ��� �ڵ����� ENABLE��
	DWORD  __stdcall AxcTableSetEnable(long lModuleNo, long lTablePos, DWORD uEnable);
	// ī���� ����� ������ ���̺��� Ʈ���� ��� ����� ���� ���θ� Ȯ����.
	DWORD  __stdcall AxcTableGetEnable(long lModuleNo, long lTablePos, DWORD* upEnable);		

	// ī���� ����� ������ ���̺��� �̿��� �߻��� Ʈ���� ������ Ȯ��.
	// lpTriggerCount : ������� ��µ� Ʈ���� ��� ������ ��ȯ, AxcTableSetTriggerCountClear �Լ��� �ʱ�ȭ
	DWORD  __stdcall AxcTableReadTriggerCount(long lModuleNo, long lTablePos, long* lpTriggerCount);	

	// ī���� ����� ������ ���̺��� �Ҵ�� H/W Ʈ���� ����Ÿ FIFO�� ���¸� Ȯ��.
	// lpCount1[0~500] : 2D Ʈ���� ��ġ ����Ÿ �� ù��°(X) ��ġ�� �����ϰ� �ִ� H/W FIFO�� �Էµ� ����Ÿ ����
	// upStatus1 : 2D Ʈ���� ��ġ ����Ÿ �� ù��°(X) ��ġ�� �����ϰ� �ִ� H/W FIFO�� ����
	//   [Bit 0] : Data Empty
	//   [Bit 1] : Data Full
	//   [Bit 2] : Data Valid
	// lpCount2[0~500] : 2D Ʈ���� ��ġ ����Ÿ �� �ι�°(Y) ��ġ�� �����ϰ� �ִ� H/W FIFO�� �Էµ� ����Ÿ ����
	// upStatus2 : 2D Ʈ���� ��ġ ����Ÿ �� �ι�°(Y) ��ġ�� �����ϰ� �ִ� H/W FIFO�� ����
	//   [Bit 0] : Data Empty
	//   [Bit 1] : Data Full
	//   [Bit 2] : Data Valid
	DWORD  __stdcall AxcTableReadFifoStatus(long lModuleNo, long lTablePos, long* lpCount1, DWORD* upStatus1, long* lpCount2, DWORD* upStatus2);

	// ī���� ����� ������ ���̺��� �Ҵ�� H/W Ʈ���� ����Ÿ FIFO�� ���� ��ġ ����Ÿ���� Ȯ��.
	// dpTopData1 : 2D H/W Ʈ���� ����Ÿ FIFO�� ���� ����Ÿ �� ù��°(X) ��ġ ����Ÿ�� Ȯ�� �� 
	// dpTopData1 : 2D H/W Ʈ���� ����Ÿ FIFO�� ���� ����Ÿ �� �ι�°(Y) ��ġ ����Ÿ�� Ȯ�� �� 
	DWORD  __stdcall AxcTableReadFifoData(long lModuleNo, long lTablePos, double* dpTopData1, double* dpTopData2);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXC_H__
