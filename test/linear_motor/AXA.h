/****************************************************************************
*****************************************************************************
**
** File Name
** ----------
**
** AXA.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Analog Library Header File
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

#ifndef __AXT_AXA_H__
#define __AXT_AXA_H__

#include "source/header/AXHS.h"

#ifdef __cplusplus
extern "C" {

#endif    //__cplusplus
//========== ���� �� ��� ���� Ȯ�� �Լ� 
    //AIO ����� �ִ��� Ȯ���Ѵ�
    DWORD    __stdcall AxaInfoIsAIOModule(DWORD *upStatus);
    
    //��� No�� Ȯ���Ѵ�.
    DWORD    __stdcall AxaInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    
    //AIO ����� ������ Ȯ���Ѵ�.
    DWORD    __stdcall AxaInfoGetModuleCount(long *lpModuleCount);

    //������ ����� �Է� ä�� ���� Ȯ���Ѵ�
    DWORD    __stdcall AxaInfoGetInputCount(long lModuleNo, long *lpCount);
    
    //������ ����� ��� ä�� ���� Ȯ���Ѵ�.
    DWORD    __stdcall AxaInfoGetOutputCount(long lModuleNo, long *lpCount);
    
    //������ ����� ù ��° ä�� ��ȣ�� Ȯ���Ѵ�.(�Է� ����,��� ���� ����)
    DWORD    __stdcall AxaInfoGetChannelNoOfModuleNo(long lModuleNo, long *lpChannelNo);

    //������ ����� ù ��° �Է� ä�� ��ȣ�� Ȯ���Ѵ�.(�Է� ���, �Է�/��� ���� ����)
    DWORD    __stdcall AxaInfoGetChannelNoAdcOfModuleNo(long lModuleNo, long *lpChannelNo);

    //������ ����� ù ��° ��� ä�� ��ȣ�� Ȯ���Ѵ�.(��� ���, �Է�/��� ���� ����)
    DWORD    __stdcall AxaInfoGetChannelNoDacOfModuleNo(long lModuleNo, long *lpChannelNo);

    //������ ��� ��ȣ�� ���̽� ���� ��ȣ, ��� ��ġ, ��� ID�� Ȯ���Ѵ�.
    DWORD    __stdcall AxaInfoGetModule (long lModuleNo, long *lpBoardNo, long *lpModulePos, DWORD *upModuleID);

    // �ش� ����� ��� ������ �������� ��ȯ�Ѵ�.
    DWORD    __stdcall AxaInfoGetModuleStatus(long lModuleNo);
    
//========== �Է� ��� ���� �˻� �Լ�
      //������ �Է� ä�� ��ȣ�� ��� ��ȣ�� Ȯ���Ѵ�.
    DWORD    __stdcall AxaiInfoGetModuleNoOfChannelNo(long lChannelNo, long *lpModuleNo);
    
       //�Ƴ��α� �Է� ����� ��ü ä�� ������ Ȯ���Ѵ�.
    DWORD    __stdcall AxaiInfoGetChannelCount(long *lpChannelCount);

//========== �Է� ��� ���ͷ�Ʈ/ä�� �̺�Ʈ ���� �� Ȯ�� �Լ�
    //������ ä�ο� �̺�Ʈ �޽����� �޾ƿ��� ���Ͽ� ������ �޽���, �ݹ� �Լ� �Ǵ� �̺�Ʈ ����� ����Ѵ�. H/W Ÿ�̸�(Timer Trigger Mode, External Trigger Mode)�� �̿�, ������ ������ ���� ���۽�(AxaStartMultiChannelAdc ����)�� ����Ѵ�.
    // ������ ��⿡ ���ͷ�Ʈ �޽����� �޾ƿ��� ���Ͽ� ������ �޽���, �ݹ� �Լ� �Ǵ� �̺�Ʈ ����� ���
    //========= ���ͷ�Ʈ ���� �Լ� ======================================================================================
    // �ݹ� �Լ� ����� �̺�Ʈ �߻� ������ ��� �ݹ� �Լ��� ȣ�� ������ ���� ������ �̺�Ʈ�� �������� �� �ִ� ������ ������
    // �ݹ� �Լ��� ������ ���� �� ������ ���� ���μ����� ��ü�Ǿ� �ְ� �ȴ�.
    // ��, �ݹ� �Լ� ���� ���ϰ� �ɸ��� �۾��� ���� ��쿡�� ��뿡 ���Ǹ� ���Ѵ�. 
    // �̺�Ʈ ����� ��������� �̿��Ͽ� ���ͷ�Ʈ �߻����θ� ���������� �����ϰ� �ִٰ� ���ͷ�Ʈ�� �߻��ϸ� 
    // ó�����ִ� �������, ������ ������ ���� �ý��� �ڿ��� �����ϰ� �ִ� ������ ������
    // ���� ������ ���ͷ�Ʈ�� �����ϰ� ó������ �� �ִ� ������ �ִ�.
    // �Ϲ������δ� ���� ������ ������, ���ͷ�Ʈ�� ����ó���� �ֿ� ���ɻ��� ��쿡 ���ȴ�. 
    // �̺�Ʈ ����� �̺�Ʈ�� �߻� ���θ� �����ϴ� Ư�� �����带 ����Ͽ� ���� ���μ����� ������ ���۵ǹǷ�
    // MultiProcessor �ý��۵�� �ڿ��� ���� ȿ�������� ����� �� �ְ� �Ǿ� Ư�� �����ϴ� ����̴�.
    // ���ͷ�Ʈ �޽����� �޾ƿ��� ���Ͽ� ������ �޽��� �Ǵ� �ݹ� �Լ��� ����Ѵ�.
    // (�޽��� �ڵ�, �޽��� ID, �ݹ��Լ�, ���ͷ�Ʈ �̺�Ʈ)
    //    hWnd        : ������ �ڵ�, ������ �޼����� ������ ���. ������� ������ NULL�� �Է�.
    //    uMessage    : ������ �ڵ��� �޼���, ������� �ʰų� ����Ʈ���� ����Ϸ��� 0�� �Է�.
    //    proc        : ���ͷ�Ʈ �߻��� ȣ��� �Լ��� ������, ������� ������ NULL�� �Է�.
    //    pEvent      : �̺�Ʈ ������� �̺�Ʈ �ڵ�
    DWORD    __stdcall AxaiEventSetChannel(long lChannelNo, HWND hWnd, DWORD uMesssage, AXT_EVENT_PROC pProc, HANDLE *pEvent);
    
    // ������ �Է� ä�ο� �̺�Ʈ ��� ������ �����Ѵ�.
    //======================================================//
    // uUse        : DISABLE(0)        // �̺�Ʈ ����
    //             : ENABLE(1)         // �̺�Ʈ ����
    //======================================================//
    DWORD    __stdcall AxaiEventSetChannelEnable(long lChannelNo, DWORD uUse);
    
    //������ �Է� ä���� �̺�Ʈ ��� ������ Ȯ���Ѵ�.
    //======================================================//
    // *upUse      : DISABLE(0)        // �̺�Ʈ ����
    //             : ENABLE(1)         // �̺�Ʈ ����
    //======================================================//
    DWORD    __stdcall AxaiEventGetChannelEnable(long lChannelNo, DWORD *upUse);
    
    //������ ���� �Է� ä�ο� �̺�Ʈ ��� ������ �����Ѵ�.
    //======================================================//
    // lSize       : ��� �� �Է� ä���� ����
    // lpChannelNo : ����� ä�� ��ȣ�� �迭
    // uUse        : DISABLE(0)        // �̺�Ʈ ����
    //             : ENABLE(1)         // �̺�Ʈ ����
    //======================================================//
    DWORD    __stdcall AxaiEventSetMultiChannelEnable(long lSize, long *lpChannelNo, DWORD uUse);
    
    //������ �Է� ä�ο� �̺�Ʈ ������ �����Ѵ�.
    //======================================================//
    // uMask       : DATA_EMPTY(1) --> ���ۿ� �����Ͱ� ���� ��
    //             : DATA_MANY(2)  --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : DATA_SMALL(3) --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : DATA_FULL(4)  --> ���ۿ� �����Ͱ� �� á�� ��
    //======================================================//
    DWORD    __stdcall AxaiEventSetChannelMask(long lChannelNo, DWORD uMask);
    
    //������ �Է� ä�ο� �̺�Ʈ ������ Ȯ���Ѵ�.
    //======================================================//
    // *upMask     : DATA_EMPTY(1) --> ���ۿ� �����Ͱ� ���� ��
    //             : DATA_MANY(2)  --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : DATA_SMALL(3) --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : DATA_FULL(4)  --> ���ۿ� �����Ͱ� �� á�� ��
    //======================================================//
    DWORD    __stdcall AxaiEventGetChannelMask(long lChannelNo, DWORD *upMask);
    
    //������ ���� �Է� ä�ο� �̺�Ʈ ������ �����Ѵ�.
    //==============================================================================//
    // lSize       : ��� �� �Է� ä���� ����
    // lpChannelNo : ����� ä�� ��ȣ�� �迭
    // uMask       : DATA_EMPTY(1) --> ���ۿ� �����Ͱ� ���� ��
    //             : DATA_MANY(2)  --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : DATA_SMALL(3) --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : DATA_FULL(4)  --> ���ۿ� �����Ͱ� �� á�� ��
    //==============================================================================//
    DWORD    __stdcall AxaiEventSetMultiChannelMask(long lSize, long *lpChannelNo, DWORD uMask);
    
    //�̺�Ʈ �߻� ��ġ�� Ȯ���Ѵ�.
    //==============================================================================//
    // *upMode     : AIO_EVENT_DATA_UPPER(1) --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : AIO_EVENT_DATA_LOWER(2) --> ���ۿ� �����Ͱ� ���� ���� ������ ������ ��
    //             : AIO_EVENT_DATA_FULL(3)  --> ���ۿ� �����Ͱ� �� á�� ��
    //             : AIO_EVENT_DATA_EMPTY(4) --> ���ۿ� �����Ͱ� ���� ��
    //==============================================================================//
    DWORD    __stdcall AxaiEventRead(long *lpChannelNo, DWORD *upMode);

    //������ ����� ���ͷ�Ʈ ����ũ�� �����Ѵ�. �� �Լ��� ������ ��ȣ���ø� �� ��쿡 �ϵ����(���)�� FIFO ���� ����ڰ� ������ ũ���� ���۷� ���� ���ͷ�Ʈ�� ���� ������ �̵� ������ �����ϱ� ���� ���ȴ�. (SIO-AI4RB�� �������� �ʴ´�.)
    //==================================================================================================//
    // uMask       : SCAN_END(1)       --> ���õ� ä�� ���  ADC ��ȯ�� �ѹ� �̷�� �� �� ���� ���ͷ�Ʈ�� �߻�
    //             : FIFO_HALF_FULL(2) --> ��⳻�� FIFO�� HALF�̻� á�� ��� ���� ���ͷ�Ʈ �߻�
    //==================================================================================================//
    DWORD    __stdcall AxaiInterruptSetModuleMask(long lModuleNo, DWORD uMask);
    
    //������ ����� ���ͷ�Ʈ ����ũ�� Ȯ���Ѵ�.
    //==================================================================================================//
    // *upMask     : SCAN_END(1)       --> ���õ� ä�� ���  ADC ��ȯ�� �ѹ� �̷�� �� �� ���� ���ͷ�Ʈ�� �߻�
    //             : FIFO_HALF_FULL(2) --> ��⳻�� FIFO�� HALF�̻� á�� ��� ���� ���ͷ�Ʈ �߻�
    //==================================================================================================//
    DWORD    __stdcall AxaiInterruptGetModuleMask(long lModuleNo, DWORD *upMask);
    
//========== �Է� ��� �Ķ���� ���� �� Ȯ�� �Լ�
    //������ �Է� ä�ο� �Է� ���� ������ �����Ѵ�.
    //==================================================================================================//
    // AI4RB
    // dMinVolt    : -10V/-5V�� ���� ����
    // dMaxVolt    : 10V/5V/�� ���� ����
    //
    // AI16Hx
    // dMinVolt    : -10V ����
    // dMaxVolt    : 10V ����
    //==================================================================================================//
    DWORD    __stdcall AxaiSetRange(long lChannelNo, double dMinVolt, double dMaxVolt);
    
    //������ �Է� ä���� �Է� ���� ������ Ȯ���Ѵ�.
    //==================================================================================================//
    // AI4RB
    // *dpMinVolt  : -10V/-5V�� ���� ����
    // *dpMaxVolt  : 10V/5V/�� ���� ����
    //
    // AI16Hx
    // *dpMaxVolt  : -10V ����
    // *dpMaxVolt  : 10V ����
    //==================================================================================================//
    DWORD    __stdcall AxaiGetRange(long lChannelNo, double *dpMinVolt, double *dpMaxVolt);

    //������ ���� �Է� ��⿡ ��� �Է� ���� ������ �����Ѵ�.
    //==================================================================================================//
    // lModuleNo   : ����� �Է� ��� ��ȣ
    //
    // RTEX AI16F
    // Mode -5~+5  : dMinVolt = -5, dMaxVolt = +5
    // Mode -10~+10: dMinVolt = -10, dMaxVolt = +10
    //==================================================================================================//
    DWORD    __stdcall AxaiSetRangeModule(long lModuleNo, double dMinVolt, double dMaxVolt);

    //������ ���� �Է� ��⿡ ��� �Է� ���� ������ Ȯ���Ѵ�.
    //==================================================================================================//
    // lModuleNo   : ����� �Է� ��� ��ȣ
    //
    // RTEX AI16F
    // *dMinVolt   : -5V, -10V
    // *dMaxVolt   : +5V, +10V
    //==================================================================================================//
    DWORD    __stdcall AxaiGetRangeModule(long lModuleNo, double *dMinVolt, double *dMaxVolt);
    
    //������ ���� �Է� ä�ο� ��� �Է� ���� ������ �����Ѵ�.
    //==================================================================================================//
    // lSize       : ����� �Է� ä���� ����
    // *lpChannelNo: ����� ä�� ��ȣ�� �迭
    //
    // AI4RB
    // dMinVolt    : -10V/-5V�� ���� ����
    // dMaxVolt    : 10V/5V/�� ���� ����
    //
    // AI16Hx
    // dMinVolt    : -10V
    // dMaxVolt    : 10V
    //==================================================================================================//

    DWORD    __stdcall AxaiSetMultiRange(long lSize, long *lpChannelNo, double dMinVolt, double dMaxVolt);
    
    //������ �Է� ��⿡ Ʈ���� ��带 �����Ѵ�.
    //==================================================================================================//
    // uTriggerMode: NORMAL_MODE(1)   --> ����ڰ� ���ϴ� ������ A/D��ȯ�ϴ� Software Trigger ��� 
    //             : TIMER_MODE(2)    --> H/W�� ���� Ŭ���� �̿��ؼ� A/D��ȯ�ϴ� Trigger ���
    //             : EXTERNAL_MODE(3) --> �ܺ� �Է´����� Ŭ���� �̿��ؼ� A/D��ȯ�ϴ� Trigger ���
    //==================================================================================================//
    DWORD    __stdcall AxaiSetTriggerMode(long lModuleNo, DWORD uTriggerMode);
    
    //������ ��⿡ Ʈ���� ��带 Ȯ���Ѵ�.
    //==================================================================================================//
    // *upTriggerMode : NORMAL_MODE(1)   --> ����ڰ� ���ϴ� ������ A/D��ȯ�ϴ� Software Trigger ��� 
    //                : TIMER_MODE(2)    --> H/W�� ���� Ŭ���� �̿��ؼ� A/D��ȯ�ϴ� Trigger ���
    //                : EXTERNAL_MODE(3) --> �ܺ� �Է´����� Ŭ���� �̿��ؼ� A/D��ȯ�ϴ� Trigger ���
    //==================================================================================================//
    DWORD    __stdcall AxaiGetTriggerMode(long lModuleNo, DWORD *upTriggerMode);
    
    //������ �Է¸���� Offset�� mVolt ����(mV)�� �����Ѵ�. �ִ� -100~100mVolt
    //==================================================================================================//
    // dMiliVolt      : -100 ~ 100 
    //==================================================================================================//
    DWORD    __stdcall AxaiSetModuleOffsetValue(long lModuleNo, double dMiliVolt); 
    
    //������ �Է¸���� Offset ���� Ȯ���Ѵ�. mVolt ����(mV)
    //==================================================================================================//
    // *dpMiliVolt    : -100 ~ 100 
    //==================================================================================================//
    DWORD    __stdcall AxaiGetModuleOffsetValue(long lModuleNo, double *dpMiliVolt); 

//========== �Է� ��� A/D ��ȯ �Լ� 
//==Software Trigger Mode �Լ�
    //����ڰ� ������ �Է� ä�ο� �Ƴ��α� �Է� ���� A/D��ȯ�� �� ���� ������ ��ȯ�Ѵ�.�� �Լ��� ����ϱ� ���� AxaSetTriggerModeAdc �Լ��� ����Ͽ� Normal Trigger Mode�� �����Ǿ� �־�� �Ѵ�.
    DWORD    __stdcall AxaiSwReadVoltage(long lChannelNo, double *dpVolt);
    
    //������ �Է� ä�ο� �Ƴ��α� �Է� ���� Digit ������ ��ȯ�Ѵ�. Normal Trigger Mode�� �����Ǿ� �־�� �Ѵ�.
    DWORD    __stdcall AxaiSwReadDigit(long lChannelNo, DWORD *upDigit);
    
    //������ ���� �Է� ä�ο� �Ƴ��α� �Է� ���� ���� ������ ��ȯ�Ѵ�. Normal Trigger Mode�� �����Ǿ� �־�� �Ѵ�.
    DWORD    __stdcall AxaiSwReadMultiVoltage(long lSize, long *lpChannelNo, double *dpVolt);

    //������ ���� �Է� ä�ο� �Ƴ��α� �Է� ���� Digit ������ ��ȯ�Ѵ�. Normal Trigger Mode�� �����Ǿ� �־�� �Ѵ�.
    DWORD    __stdcall AxaiSwReadMultiDigit(long lSize, long *lpChannelNo, DWORD *upDigit);

//==Hardware Trigger Mode �Լ�
    //������ ���� �Է� ä�ο� Immediate��带 ����ϱ� ���� ���� ���� �����Ѵ�. �� �Լ��� ����ϱ� ���� AxaSetTriggerModeAdc �Լ��� ����Ͽ� Timer Trigger Mode�� �����Ǿ� �־�� �Ѵ�.
    DWORD    __stdcall AxaiHwSetMultiAccess(long lSize, long *lpChannelNo, long *lpWordSize);
    
    //������ ������ŭ A/D��ȯ �� ���� ���� ��ȯ�Ѵ�. �� �Լ��� ����ϱ� ���� AxaiHwSetMultiAccess�Լ��� �̿� �������� �����ؾ� �ϸ� , AxaSetTriggerModeAdc �Լ��� ����Ͽ� Timer Trigger Mode�� �����Ǿ� �־�� �Ѵ�.
    DWORD    __stdcall AxaiHwStartMultiAccess(double *dpBuffer);
    
    //������ ��⿡ ���ø� ������ ���ļ� ����(Hz)�� �����Ѵ�.
    //==================================================================================================//
    // dSampleFreq    : 10 ~ 100000 
    //==================================================================================================//
    DWORD    __stdcall AxaiHwSetSampleFreq(long lModuleNo, double dSampleFreq);
    
    //������ ��⿡ ���ø� ������ ���ļ� ������(Hz) ������ ���� Ȯ���Ѵ�.
    //==================================================================================================//
    // *dpSampleFreq  : 10 ~ 100000 
    //==================================================================================================//
    DWORD    __stdcall AxaiHwGetSampleFreq(long lModuleNo, double *dpSampleFreq);
    
    //������ ��⿡ ���ø� ������ �ð� ����(uSec)�� �����Ѵ�.
    //==================================================================================================//
    // dSamplePeriod  : 100000 ~ 1000000000
    //==================================================================================================//
    DWORD    __stdcall AxaiHwSetSamplePeriod(long lModuleNo, double dSamplePeriod);
    
    //������ ��⿡ ���ø� ������ �ð� ����(uSec)�� ������ ���� Ȯ���Ѵ�.
    //==================================================================================================//
    // *dpSamplePeriod: 100000 ~ 1000000000
    //==================================================================================================//
    DWORD    __stdcall AxaiHwGetSamplePeriod(long lModuleNo, double *dpSamplePeriod);
    
    //������ �Է� ä�ο� ���۰� Full�� á�� �� ���� ����� �����Ѵ�.
    //==================================================================================================//
    // uFullMode      : NEW_DATA_KEEP(0)  --> ���ο� ������ ����
    //                : CURR_DATA_KEEP(1) --> ���� ������ ����
    //============================================================f======================================//
    DWORD    __stdcall AxaiHwSetBufferOverflowMode(long lChannelNo, DWORD uFullMode);
    
    //������ �Է� ä���� ���۰� Full�� á�� �� ���� ����� Ȯ���Ѵ�.
    //==================================================================================================//
    // *upFullMode    : NEW_DATA_KEEP(0)  --> ���ο� ������ ����
    //                : CURR_DATA_KEEP(1) --> ���� ������ ����
    //==================================================================================================//
    DWORD    __stdcall AxaiHwGetBufferOverflowMode(long lChannelNo, DWORD *upFullMode);
    
    //������ ���� �Է� ä�ο� ���۰� Full�� á�� �� ���� ����� �����Ѵ�.
    //==================================================================================================//
    // uFullMode      : NEW_DATA_KEEP(0)  --> ���ο� ������ ����
    //                : CURR_DATA_KEEP(1) --> ���� ������ ����
    //==================================================================================================//
    DWORD    __stdcall AxaiHwSetMultiBufferOverflowMode(long lSize, long *lpChannelNo, DWORD uFullMode);
    
    //������ �Է� ä�ο� ������ ���� ���� ���� ���� �����Ѵ�.
    DWORD    __stdcall AxaiHwSetLimit(long lChannelNo, long lLowLimit, long lUpLimit);
    
    //������ �Է� ä�ο� ������ ���� ���� ���� ���� Ȯ���Ѵ�.
    DWORD    __stdcall AxaiHwGetLimit(long lChannelNo, long *lpLowLimit, long *lpUpLimit);

    //������ ���� �Է� ä�ο� ������ ���� ���� ���� ���� �����Ѵ�.
    DWORD    __stdcall AxaiHwSetMultiLimit(long lSize, long *lpChannelNo, long lLowLimit, long lUpLimit);
    
    //������ ���� �Է� ä�ο� H/WŸ�̸Ӹ� �̿��� A/D��ȯ�� �����Ѵ�.
    DWORD    __stdcall AxaiHwStartMultiChannel(long lSize, long *lpChannelNo, long lBuffSize);

    //������ �Է� ä�ο� H/WŸ�̸Ӹ� �̿��� A/D��ȯ�� �����Ѵ�.
    DWORD    __stdcall AxaiHwStartSingleChannelAdc(long lChannelNo, long lBuffSize);

    //H/WŸ�̸Ӹ� �̿��� ���� ��ȣ A/D��ȯ�� �����Ѵ�.
    DWORD    __stdcall AxaiHwStopSingleChannelAdc(long lChannelNo);
    
    //������ ���� �Է� ä�ο� A/D��ȯ�� ���� �� ������ ������ŭ ���� ó���ؼ� �������� ��ȯ�Ѵ�.
    //==================================================================================================//
    // lSize           : ����� �Է� ä���� ����
    // *lpChannelNo    : ����� ä�� ��ȣ�� �迭
    // lFilterCount    : Filtering�� �������� ����
    // lBuffSize       : �� ä�ο� �Ҵ�Ǵ� ������ ����
    //==================================================================================================//
    DWORD    __stdcall AxaiHwStartMultiFilter(long lSize, long *lpChannelNo, long lFilterCount, long lBuffSize);
    
    //H/WŸ�̸Ӹ� �̿��� ���� ��ȣ A/D��ȯ�� �����Ѵ�.
    DWORD    __stdcall AxaiHwStopMultiChannel(long lModuleNo);
    
    //������ �Է� ä���� �޸� ���ۿ� �����Ͱ� �� ������ �˻��Ѵ�.
    DWORD    __stdcall AxaiHwReadDataLength(long lChannelNo, long *lpDataLength);
    
    //������ �Է� ä�ο� H/WŸ�̸Ӹ� �̿��Ͽ� A/D��ȯ�� ���� ���� ������ �д´�.
    DWORD    __stdcall AxaiHwReadSampleVoltage(long lChannelNo, long *lpSize, double *dpVolt);
    
    //������ �Է� ä�ο� H/WŸ�̸Ӹ� �̿��Ͽ� A/D��ȯ�� ���� Digit ������ �д´�.
    DWORD    __stdcall AxaiHwReadSampleDigit(long lChannelNo, long *lpsize, DWORD *upDigit);

//========== �Է� ��� ���� ���� üũ �Լ�
    //������ �Է� ä���� �޸� ���ۿ� �����Ͱ� ���� �� �˻��Ѵ�.
    //==================================================================================================//
    // *upEmpty        : FALSE(0) --> �޸� ���ۿ� �����Ͱ� ���� ���
    //                 : TRUE(1) --> �޸� ���ۿ� �����Ͱ� ���� ���
    //==================================================================================================//
    DWORD    __stdcall AxaiHwIsBufferEmpty(long lChannelNo, DWORD *upEmpty);
    
    //������ �Է� ä���� �޸� ���ۿ� �����Ǿ� �ִ� ���� ������ �����Ͱ� ���� �� �˻��Ѵ�
    //==================================================================================================//
    // *upUpper        : FALSE(0) --> �޸� ���ۿ� �����Ͱ� ���� ������ ���� ���
    //                 : TRUE(1) --> �޸� ���ۿ� �����Ͱ� ���� ������ ���� ���
    //==================================================================================================//
    DWORD    __stdcall AxaiHwIsBufferUpper(long lChannelNo, DWORD *upUpper);
    
    //������ �Է� ä���� �޸� ���ۿ� �����Ǿ� �ִ� ���� ������ �����Ͱ� ���� �� �˻��Ѵ�.
    //==================================================================================================//
    // *upLower        : FALSE(0) --> �޸� ���ۿ� �����Ͱ� ���� ������ ���� ���
    //                 : TRUE(1) --> �޸� ���ۿ� �����Ͱ� ���� ������ ���� ���
    //==================================================================================================//
    DWORD    __stdcall AxaiHwIsBufferLower(long lChannelNo, DWORD *upLower);

//==External Trigger Mode �Լ�
    //������ �Է¸���� ���õ� ä�ε��� �ܺ� Ʈ���� ��带 �����Ѵ�.
    //==================================================================================================//
    // lSize           : ������ �Է� ��⿡�� �ܺ�Ʈ���Ÿ� ��� �� ä�ΰ���
    // *lpChannelPos   : ������ �Է� ��⿡�� �ܺ�Ʈ���Ÿ� ��� �� ä���� Index
    DWORD   __stdcall AxaiExternalStartADC(long lModuleNo, long lSize, long *lpChannelPos);
    //������ �Է¸���� �ܺ�Ʈ���� ��带 �����Ѵ�.  
    DWORD   __stdcall AxaiExternalStopADC(long lModuleNo);
    //������ �Է¸���� Fifo���¸� ��ȯ�Ѵ�.
    //==================================================================================================//
    // *dwpStatus      : FIFO_DATA_EXIST(0)
    //                 : FIFO_DATA_EMPTY(1)
    //                 : FIFO_DATA_HALF(2)
    //                 : FIFO_DATA_FULL(6)
    //==================================================================================================//
    DWORD   __stdcall AxaiExternalReadFifoStatus(long lModuleNo, DWORD *dwpStatus);
    //������ �Է¸���� �ܺν�ȣ�� ���� ��ȯ�� A/D���� �о��.
    // lSize           : ������ �Է� ��⿡�� ��ȯ�� A/D���� �о�� ä���� ����(AxaiExternalStartADC�� ����� ä�ΰ����� ���� �ؾߵ�)
    // *lpChannelPos   : ������ �Է� ��⿡�� ��ȯ�� A/D���� �о�� ä���� Index(AxaiExternalStartADC�� ����� ä���� Index�� ���� �ؾߵ�)
    // lDataSize       : �ܺ�Ʈ���ſ� ���� A/D��ȯ�� ���� �ѹ��� �о� �� �ִ� ����Ÿ�� ����
    // lBuffSize       : �ܺο���(����� Program) �Ҵ��� Data Buffer�� Size
    // lStartDataPos   : �ܺο���(����� Program) �Ҵ��� Data Buffer�� ���� ���� �� ��ġ 
    // *dpVolt         : A/D��ȯ�� ���� �Ҵ� ���� 2���� �迭 ����Ʈ(dpVlot[Channel][Count])
    // *lpRetDataSize  : A/D��ȯ�� ���� Data Buffer�� ���� �Ҵ�� ����
    // *dwpStatus      : A/D��ȯ�� ���� Fifo(H/W Buffer)�� ���� ���� �� Fifo���¸� ��ȯ��.
    DWORD   __stdcall AxaiExternalReadVoltage(long lModuleNo, long lSize, long *lpChannelPos, long lDataSize, long lBuffSize, long lStartDataPos, double *dpVolt, long *lpRetDataSize, DWORD *dwpStatus);
    
//==��� ��� Info
       //������ ��� ä�� ��ȣ�� ��� ��ȣ�� Ȯ���Ѵ�.
    DWORD    __stdcall AxaoInfoGetModuleNoOfChannelNo(long lChannelNo, long *lpModuleNo);

    //�Ƴ��α� ��� ����� ��ü ä�� ������ Ȯ���Ѵ�.
    DWORD    __stdcall AxaoInfoGetChannelCount(long *lpChannelCount);

//========== ��� ��� ���� �� Ȯ�� �Լ�
    //������ ��� ä�ο� ��� ���� ������ �����Ѵ�
    //==================================================================================================//
    // AO4R, AO2Hx
    // dMinVolt    : -10V
    // dMaxVolt    : 10V
    //==================================================================================================//
    DWORD    __stdcall AxaoSetRange(long lChannelNo, double dMinVolt, double dMaxVolt);
    
    //������ ��� ä���� ��� ���� ������ Ȯ���Ѵ�.
    //==================================================================================================//
    // AO4R, AO2Hx
    // *dpMinVolt  : -10V
    // *dpMaxVolt  : 10V
    //==================================================================================================//
    DWORD    __stdcall AxaoGetRange(long lChannelNo, double *dpMinVolt, double *dpMaxVolt);
    
    //������ ���� ��� ä�ο� ��� ���� ������ �����Ѵ�.
    //==================================================================================================//
    // AO4R, AO2Hx
    // dMinVolt    : -10V
    // dMaxVolt    : 10V
    //==================================================================================================//
    DWORD    __stdcall AxaoSetMultiRange(long lSize, long *lpChannelNo, double dMinVolt, double dMaxVolt);
    
    //������ ��� ä�ο� �Էµ� ������ ��� �ȴ�.
    DWORD    __stdcall AxaoWriteVoltage(long lChannelNo, double dVolt);

	//������ ���� ��� ä�ο� �Էµ� ������ ��� �ȴ�.
	DWORD    __stdcall AxaoWriteMultiVoltage(long lSize, long *lpChannelNo, double *dpVolt);

	//������ ��� ä�ο� �Էµ� ������ ��� �ȴ�.
	DWORD    __stdcall AxaoWriteDigit(long lChannelNo, DWORD dwDigit);

	//������ ���� ��� ä�ο� �Էµ� ������ ��� �ȴ�.
	DWORD    __stdcall AxaoWriteMultiDigit(long lSize, long *lpChannelNo, DWORD *dpDigit);

    //������ ��� ä�ο� ��µǴ� ���� ���� Ȯ���Ѵ�.
    DWORD    __stdcall AxaoReadVoltage(long lChannelNo, double *dpVolt);

    //������ ���� ��� ä�ο� ��µǴ� ���� ���� Ȯ���Ѵ�.
    DWORD    __stdcall AxaoReadMultiVoltage(long lSize, long *lpChannelNo, double *dpVolt); 

	//������ ��� ä�ο� ��µǴ� ���� ���� Ȯ���Ѵ�.
	DWORD    __stdcall AxaoReadDigit(long lChannelNo, DWORD *upDigit);

	//������ ���� ��� ä�ο� ��µǴ� ���� ���� Ȯ���Ѵ�.
	DWORD    __stdcall AxaoReadMultiDigit(long lSize, long *lpChannelNo, DWORD *dpDigit); 

	//============================ AXA User Define Pattern Generator ============================
	// Pattern Generator ���� �Լ����� PCI-R32IO_RTEX_LCM ������ ����
	// Channel User Define Pattern Generator�� ���� �ϴ� �Լ� �̴�.
	// AxaoPgSetUserInterval�� ������ �ð� ���� Pattern�� ���������� ����Ѵ�.
    // lLoopCnt       : '0'(�Էµ� ������ ���Ѵ�� �ݺ�), 'value' : ������ Ƚ�� ��ŭ �Է� ���� ��� �� ������ ���� ����
    //                : (MAX : 60000)
	// lPatternSize   : �Է� ���� ����(MAX : 8192) , PCI-R
	DWORD   __stdcall AxaoPgSetUserPatternGenerator(long lChannelNo, long lLoopCnt, long lPatternSize, double *dpPattern);
	//AxaoPgSetUserPatternGenerator�� ���� ���, Digit ������ ǥ��
	DWORD   __stdcall AxaoPgSetUserPatternGeneratorDigit(long lChannelNo, long lLoopCnt, long lPatternSize, DWORD *pDigitPattern);
	
	// User Define Pattern Generator�� Ȯ�� �ϴ� �Լ� �̴�.
	DWORD   __stdcall AxaoPgGetUserPatternGenerator(long lChannelNo, long *lpLoopCnt, long *lpPatternSize, double *dpPattern);
	// User Define Pattern Generator�� Ȯ�� �ϴ� �Լ� �̴�. Digit������ Ȯ��
	DWORD   __stdcall AxaoPgGetUserPatternGeneratorDigit(long lChannelNo, long *lpLoopCnt, long *lpPatternSize, DWORD *pDigitPattern);
	
	// �ش� Channel�� Pattern Generator Interval�� ���� �ϴ� �Լ� �̴�.
	// ������ us(�⺻ Resolution : 500uSec)
	DWORD   __stdcall AxaoPgSetUserInterval(long lChannelNo, double dInterval);
	
	// �ش� Channel�� Pattern Generator Interval�� Ȯ�� �ϴ� �Լ� �̴�.
	DWORD   __stdcall AxaoPgGetUserInterval(long lChannelNo, double *dpInterval);
	
	// �ش� Channel�� Pattern Index / Loop Cnt�� Ȯ�� �ϴ� �Լ� �̴�.
	// Status�� ��� �Ʒ��� Status�� Ȯ�� �� �� �ִ�.
	// lpIndexNum : ���� User Pattern�� Index
	// lpLoopCnt : ���� �������� Loop Ƚ��
	// dwpInBusy : Pattern Generator�� ���� ����
	DWORD	__stdcall AxaoPgGetStatus(long lChannelNo, long *lpIndexNum, long *lpLoopCnt, DWORD *dwpInBusy);
	
	// �ش� Channel�� User Define Pattern Generator�� ���� �ϴ� �Լ��̴�. (AO ��� ����)
	// Start Channel ��ȣ�� �迭�� �Է�
	// �Էµ� ä�ο� ���Ͽ� ���ÿ� ���� ���� ����� �����Ѵ�.
	DWORD   __stdcall AxaoPgSetUserStart(long *lpChannelNo, long lSize);
	
	// �ش� Channel�� User Define Pattern Generator�� ���� �ϴ� �Լ��̴�. (AO ��� ����)
	// ��� ������ ��°��� 0Volt�� ��ȯ�ȴ�.
	DWORD   __stdcall AxaoPgSetUserStop(long *lpChannelNo, long lSize);
	
	// Pattern Data�� Clear �ϴ� �Լ�. (0x00���� ��� ������ Reset)
	DWORD   __stdcall AxaPgSetUserDataReset(long lChannelNo);

	// ������ ��� ����� Network�� ���� ���� ��� ��� ���¸� ä�� ���� �����ϴ� �Լ��̴�.
	//===============================================================================================//
	// lChannelNo  : ä�� ��ȣ(�л��� �����̺� ��ǰ�� ���� ��)
	// dwSetValue  : ���� �� ���� ��(Default�� Network ���� ���� �� ���� ����)
	//             : 0 --> Network ���� ���� �� ���� ����
	//             : 1 --> Analog Max 
	//             : 2 --> Analog MIN 
	//             : 3 --> User Vaule(Default user value�� 0V�� ������, AxaoSetNetworkErrorUserValue() �Լ��� ���氡��)
	//             : 4 --> Analog 0 V
	//===============================================================================================//
    DWORD   __stdcall AxaoSetNetworkErrorAct(long lChannelNo, DWORD dwSetValue);

	// ������ ��� ����� Network�� ���� ���� ��� ��� ���¸� ��� Byte ������ �����Ѵ�.
	//===============================================================================================//
	// lChannelNo  : ä�� ��ȣ(�л��� �����̺� ��ǰ�� ���� ��)
	// dVolt       : ����� ���� �Ƴ��α� ��� ���� ��
	//===============================================================================================//
	DWORD   __stdcall AxaoSetNetworkErrorUserValue(long lChannelNo, double dVolt);

#ifdef __cplusplus

}
#endif    //__cplusplus

#endif    //__AXT_AXA_H__
