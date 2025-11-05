/*
 * UI.h
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#ifndef INC_UI_H_
#define INC_UI_H_

#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include "rtime.h"
#include "OBD2.h"

typedef enum {
    SCREEN_INIT,
    SCREEN_CONNECT,
    MENU_MAIN,
    MENU_READ_DATA,
    MENU_DTC_CHECK,
    MENU_LOGGING,
	MENU_VIN_READ
} MenuState;

void display_init(void);
void ShowInitScreen(void);
void ShowConnectingScreen(void);
void ShowMainMenu(void);
void ShowReadDataScreen(void);
void ShowDTCReadingScreen(void);
void ShowDTCCheckScreen(void);
void ShowLoggingScreen(void);
void ShowVINReadingScreen(void);
void ShowVINScreen(void);
void printNumber(uint8_t x, uint8_t y, uint16_t value);
void printTime(uint8_t x, uint8_t y, uint16_t value);
void showData(void);
void DecodeDTC(uint8_t *raw, char *output);
void printValueWithUnit(uint16_t value, uint16_t x, uint16_t y, const char* unit);

extern uint8_t currentPage;
extern MenuState currentMenu;
extern uint8_t selectedIndex;
extern uint8_t showingData;
extern uint8_t isLogging;

#endif /* INC_UI_H_ */
