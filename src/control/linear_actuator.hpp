#pragma once
#include "stddef.h"
#include "AXL.h"
#include "AXM.h"
#include <iostream>
#include <conio.h>
#include "../constants.hpp"
#include "../utils/sleep.hpp"

/// char mot_file[] = "src/mot/ajin20190628.mot";        // *.mot file Path
char mot_file[] = "hinguri.mot";


class LinearActuator
{
public:
	LinearActuator()
	{
		DWORD Code = AxlOpen(7);
		if (Code == AXT_RT_SUCCESS)
		{
			printf("Library Reset. \n");
			//Check for Motion Module
			DWORD uStatus;
			Code = AxmInfoIsMotionModule(&uStatus);
			if (Code == AXT_RT_SUCCESS)
			{
				printf("Library Reset.\n");
				if (uStatus == STATUS_EXIST)
				{
					printf("Library Reset. Motion model exists. \n");

					AxmMotLoadParaAll(mot_file);

					AxmStatusSetActPos(LINEAR_AXIS_NO, 0.0);
					AxmStatusSetCmdPos(LINEAR_AXIS_NO, 0.0);


					AxmSignalServoOn(LINEAR_AXIS_NO, ENABLE);

					AxmMotSetAbsRelMode(LINEAR_AXIS_NO, LINEAR_ABS_REL_MODE);
					AxmMotSetProfileMode(LINEAR_AXIS_NO, LINEAR_PROFILE_MODE);
				}
			}
		}
	}

	~LinearActuator()
	{
		AxmSignalServoOn(LINEAR_AXIS_NO, 0);
		AxlClose();
	}

	void move_actu(int pos)
	{
		AxmMotSetMoveUnitPerPulse(LINEAR_AXIS_NO, 10, LINEAR_PULSE_PER_10_UNITS); // mm
		AxmMovePos(0, pos, LINEAR_VEL, LINEAR_ACCEL, LINEAR_DECEL);
		DWORD uStatus;
		AxmStatusReadInMotion(LINEAR_AXIS_NO, &uStatus);
		while (uStatus)
		{
			AxmStatusReadInMotion(LINEAR_AXIS_NO, &uStatus);
		}
	}

	void move_to(int pos) {
		AxmMoveToAbsPos(LINEAR_AXIS_NO, pos, LINEAR_VEL, LINEAR_ACCEL, LINEAR_DECEL);
	}

	/*
	void CLinear_actu::scan_home() {
		long nHmDir = 0; // 1단계 이동 방향 - (-) 방향
		DWORD uHomeSignal = HomeSensor; // 신호 검색 구동에서의 신호 사용
		DWORD uZphas = 1; // z상 검출 유무 (1: 검출)
		double dHomeClrTime = 2000.0; // Enc값을 세팅하기 위한 대기 시간
		double dHomeOffset = 0.0; // 검색 후 오프셋 값만큼 이동
		AxmHomeSetMethod(LINEAR_AXIS_NO, nHmDir, uHomeSignal, uZphas, dHomeClrTime, dHomeOffset);

		double dVelFirst = 100;
		double dVelSecond = 20;
		double dVelThird = 10;
		double dVelLast = 10;
		double dAccFirst = 200;
		double dAccSecond = 40;
		AxmHomeSetVel(LINEAR_AXIS_NO, dVelFirst, dVelSecond, dVelThird, dVelLast, dAccFirst, dAccSecond);

		if (AxmHomeSetResult(0, HOME_ERR_USER_BREAK) == AXT_RT_SUCCESS)
		{
			printf("0축 원점 검색 결과를 사용자 BREAK로 설정하였습니다.");
		}
		else
		{
			printf("0축 원점 검색 결과 설정을 실패하였습니다.");
		}

		BOOL bRun = TRUE;
		DWORD uHomeResult;

		AxmHomeSetStart(0);
		while (bRun)
		{
			//원점 검색 함수를 이용하여 성공적으로 원점 검색이 수행되고 나면 검색 결과를 출력한다.
			AxmHomeGetResult(0, &uHomeResult);
			switch (uHomeResult)
			{
			case HOME_ERR_UNKNOWN:
				printf("알 수 없는 축 번호임");
				bRun = FALSE;
				break;

			case HOME_ERR_GNT_RANGE:
				printf("Gantry Offset을 벗어났음");
				bRun = FALSE;
				break;

			case HOME_ERR_USER_BREAK:
				printf("원점 검색을 정지함");
				bRun = FALSE;
				break;

			case HOME_ERR_VELOCITY:
				printf("속도 설정이 되어있지 않음.");
				bRun = FALSE;
				break;

			case HOME_ERR_AMP_FAULT:
				printf("서보팩 알람이 발생했음");
				bRun = FALSE;
				break;

			case HOME_ERR_NEG_LIMIT:
				printf("(-)리미트 센서가 검출됨");
				bRun = FALSE;
				break;

			case HOME_ERR_POS_LIMIT:
				printf("(+)리미트 센서가 검출됨");
				bRun = FALSE;
				break;

			case HOME_SEARCHING:
				printf("현재 원점 검색 진행 중임");
				sleep_s(1);
				break;

			case HOME_SUCCESS:
				printf("원점 검색 성공함");
				bRun = FALSE;
				break;
			}
		}

		DWORD uHomeMainStepNumber, uHomeStepNumber, uHomeResult;

		while (1) {
			AxmHomeGetRate(LINEAR_AXIS_NO, &uHomeMainStepNumber, &uHomeStepNumber);
			printf("Home scan progress: %d\n", uHomeStepNumber);
			sleep_s(1);
			if (uHomeStepNumber >= 100) {
				AxmHomeGetResult(LINEAR_AXIS_NO, &uHomeResult);
				printf("Home: %d\n", uHomeResult);
				AxmHomeSetStart(LINEAR_AXIS_NO);
				break;
			}
		}
	}

	void find_home() {
		LinearActuator actuator;
		actuator.scan_home();
	}
	*/
};