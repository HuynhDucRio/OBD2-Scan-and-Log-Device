/*
 * OBD2.h
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#ifndef INC_OBD2_H_
#define INC_OBD2_H_

#include "main.h"

void OBD2_Config(void);
void OBD2_init(void);
void OBD2_DTC(void);
void OBD2_VIN(void);
void OBD2_SendPID(uint8_t pid);
void OBD2_SendPIDs(void);
void OBD2_SendFC(void);
void CAN_SF_Handler(void);
void CAN_FF_Handler(void);
void CAN_CF_Handler(void);

extern uint8_t DTC_COUNT;

typedef struct {
	uint8_t payload;
	uint8_t code[12];
	uint8_t reading;
} DTC_Def;
extern DTC_Def DTCs;
typedef struct {
	uint8_t payload;
	uint8_t code[17];
	uint8_t reading;
} VIN_Def;

extern uint8_t Speed;
extern uint16_t RPM;
extern uint8_t inTakeAir;
extern uint8_t ThrPos;
extern uint8_t EngineLoad;
extern uint8_t CoolantTemp;
extern uint16_t MassAirFlow;
extern uint8_t FuelLevel;

extern uint8_t init_OK;
extern uint8_t initiating;

extern uint32_t TxMailbox;
extern uint8_t TxData[8];
extern uint8_t RxData[8];

extern uint32_t spPids;
extern uint8_t pidList[];
extern uint8_t pidIndex;

extern CAN_TxHeaderTypeDef TxHeader;
extern CAN_RxHeaderTypeDef RxHeader;

extern VIN_Def VIN;
extern DTC_Def DTCs;
#endif /* INC_OBD2_H_ */
