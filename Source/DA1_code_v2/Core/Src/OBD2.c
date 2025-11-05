/*
 * OBD2.c
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#include "OBD2.h"
#include "UI.h"

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[8] = {0x02, 0x01, 0x0D, 0x55, 0x55, 0x55, 0x55, 0x55};
uint8_t RxData[8];
uint32_t TxMailbox;

CAN_FilterTypeDef ObdII_FilterConfig;

uint8_t Speed = 0;
uint16_t RPM = 0;
uint8_t inTakeAir = 0;
uint8_t ThrPos = 0;
uint8_t EngineLoad = 0;
uint8_t CoolantTemp = 0;
uint16_t MassAirFlow = 0;
uint8_t FuelLevel = 0;

uint8_t pidList[] = {0x0D, 0x0C, 0x0F, 0x11, 0x04, 0x05, 0x10, 0x2F};
uint8_t pidIndex = 0;

uint8_t init_OK;
uint8_t initiating;
uint32_t spPids;

DTC_Def DTCs = {
    .payload = 12,
    .reading = 0
};

VIN_Def VIN = {
    .payload = 17,
    .reading = 0
};

uint8_t DTC_COUNT = 0;
uint8_t totalPayloadLength = 0;
uint8_t dtcIndex = 0;
uint8_t vinIndex = 0;

void OBD2_DTC(void)
{
	TxData[0] = 0x01;      // Data length: 2 bytes
	TxData[1] = 0x03;      // Mode 01 - current data
	for (int i = 2; i < 8; i++) TxData[i] = 0x55;

	HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
}

void OBD2_VIN(void)
{
	TxData[0] = 0x02;
	TxData[1] = 0x09;
	TxData[2] = 0x02;

	for (int i = 3; i < 8; i++) TxData[i] = 0x55;

	HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
}

void OBD2_SendPID(uint8_t pid)
{
    TxData[0] = 0x02;      // Data length: 2 bytes
    TxData[1] = 0x01;      // Mode 01 - current data
    TxData[2] = pid;       // PID
    for (int i = 3; i < 8; i++) TxData[i] = 0x55;

    HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
}

void OBD2_SendPIDs(void)
{
	OBD2_SendPID(pidList[pidIndex]);
	pidIndex++;
	if (pidIndex >= sizeof(pidList) / sizeof(pidList[0])) pidIndex = 0;
}

void OBD2_SendFC(void)
{
	TxData[0] = 0x30;
	for (int i = 1; i < 8; i++) TxData[i] = 0x00;

	HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
}

void CAN_SF_Handler(void)
{
	uint8_t mode = RxData[1];
	uint8_t pid = RxData[2];

	if (mode == 0x41 )
	{
		switch(pid)
		{
			case 0x00:
				spPids = ((uint32_t)RxData[3] << 24) | ((uint32_t)RxData[4] << 16) | ((uint32_t)RxData[5] << 8) | RxData[6];
				init_OK = 1;
				initiating = 0;
				break;
			case 0x0D:
				Speed = RxData[3];

				break;

			case 0x0C:
				RPM =  (RxData[3] << 8 | RxData[4]) / 4;
				break;

			case 0x0F:
				inTakeAir = RxData[3] - 40;

				break;

			case 0x11:
				ThrPos = (RxData[3] * 100 ) / 255;
				break;

			case 0x05:
				CoolantTemp = RxData[3] - 40;
				break;

			case 0x04:
				EngineLoad = (RxData[3] * 100 ) / 255;
				break;

			case 0x2F:
				FuelLevel = (RxData[3] * 100 ) / 255;
				break;

			case 0x10:
				MassAirFlow = (256 * RxData[3] + RxData[4]) / 100;
				break;

			default:
				break;
		}
	}
	else if(mode == 0x43)
	{
		DTC_COUNT = (RxData[0] - 2) / 2;
		for (int i = 0; i < DTC_COUNT*2; i++)
		{
			DTCs.code[i] = RxData[i + 2];
		}
		ShowDTCCheckScreen();
	}
}

void CAN_FF_Handler(void)
{
    totalPayloadLength = ((RxData[0] & 0x0F) << 8) | RxData[1];
    uint8_t mode = RxData[2];
    DTC_COUNT = RxData[3];
    if (mode == 0x43) {
        dtcIndex = 0;
        for (int i = 4; i < 8 && dtcIndex < sizeof(DTCs.code); i++) {
            DTCs.code[dtcIndex++] = RxData[i];
        }
        DTCs.reading = 1;
        OBD2_SendFC();
    }
    else if (mode == 0x49 && RxData[3] == 0x02) {  // VIN PID
        vinIndex = 0;
        for (int i = 5; i < 8 && vinIndex < sizeof(VIN.code); i++) {
            VIN.code[vinIndex++] = RxData[i];
        }
        VIN.reading = 1;
        OBD2_SendFC();
    }
}


void CAN_CF_Handler(void)
{
    if (DTCs.reading) {

        for (int i = 1; i < 8 && dtcIndex < (totalPayloadLength - 1); i++) {
            DTCs.code[dtcIndex++] = RxData[i];
        }
        if (dtcIndex >= totalPayloadLength - 1) { // -1 vì byte đầu là 0x43
            DTCs.reading = 0;
            ShowDTCCheckScreen();
        }
    }
    else if (VIN.reading) {
        for (int i = 1; i < 8 && vinIndex < (totalPayloadLength - 2); i++) {
            VIN.code[vinIndex++] = RxData[i];
        }
        if (vinIndex >= totalPayloadLength - 3) { // -3 vì 0x49 + 0x02 + NODI
            VIN.reading = 0;
            ShowVINScreen();
        }
    }
}



void OBD2_Config(void)
{
	HAL_CAN_Start(&hcan);
	  //Configure the id field for OBD-II
	  TxHeader.DLC = 8;
	  TxHeader.IDE = CAN_ID_STD;
	  TxHeader.RTR = CAN_RTR_DATA;
	  TxHeader.StdId = 0x7DF;

	  //Activate the filter
	  ObdII_FilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	  ObdII_FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	  ObdII_FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	  ObdII_FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	  ObdII_FilterConfig.FilterIdHigh = 0x7E8 << 5;
	  ObdII_FilterConfig.FilterIdLow = 0;
	  ObdII_FilterConfig.FilterMaskIdHigh = 0x7F8 << 5;
	  ObdII_FilterConfig.FilterMaskIdLow = 0;

	  HAL_CAN_ConfigFilter(&hcan, &ObdII_FilterConfig);

	  // Activate the notification
	  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
}

void OBD2_init(void)
{
	TxData[2] = 0x00;
	HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
}
