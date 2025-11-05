/*
 * SDlog.h
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#ifndef INC_SDLOG_H_
#define INC_SDLOG_H_

#include "fatfs.h"
#include "ds1307.h"

#define LOG_BUFFER_SIZE 512

void SDLog_Init(void);
void SDLog_AddEntry(uint16_t rpm, uint8_t speed, int8_t temp, uint8_t tp);
void SDLog_Flush(void);
void log_data_to_csv(uint16_t rpm, uint8_t speed, uint8_t Atemp, uint8_t tp, uint8_t load, uint8_t Ctemp, uint16_t MAF, uint8_t fuel );
uint8_t SDLog_IsBufferNearlyFull(void);

#endif /* INC_SDLOG_H_ */
