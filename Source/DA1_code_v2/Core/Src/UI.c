/*
 * UI.c
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */
#include "UI.h"
#include "OBD2.h"

MenuState currentMenu = SCREEN_INIT;
uint8_t currentPage = 0;
uint8_t selectedIndex = 0;
uint8_t showingData = 0;
uint8_t isLogging = 0;
const char* menuItems[] = {
    "Live data",
    "DTC check",
    "Log data",
	"VIN check"
};
void printValueWithUnit(uint16_t value, uint16_t x, uint16_t y, const char* unit)
{
    char buffer[10];
    sprintf(buffer, "%u", value); // convert number to string

    ST7735_WriteString(x, y, buffer, Font_11x18, ST7735_GREEN, ST7735_BLACK); // in số

    uint16_t textLength = strlen(buffer);
    uint16_t unitX = x + textLength * 11; // mỗi ký tự rộng 11 pixel
    ST7735_WriteString(unitX, y, unit, Font_11x18, ST7735_GREEN, ST7735_BLACK); // in đơn vị ngay sau số
}
void showData(void)
{
    if (currentPage == 0)
    {
        printValueWithUnit(RPM,         5 + 11*3, 40 + 19, "r/min");
        printValueWithUnit(Speed,       5 + 11*4, 90 + 15, "km/h");
    }
    else if (currentPage == 1)
    {
        printValueWithUnit(EngineLoad,  5 + 11*10, 30 + 19, "%");
        printValueWithUnit(ThrPos,      5 + 11*10, 80 + 15, "%");

    }
    else if (currentPage == 2)
    {
    	printValueWithUnit(MassAirFlow, 5 + 11*5, 40 + 19, "g/s");
    	printValueWithUnit(FuelLevel,   5 + 11*6, 90 + 15, "%");
    }
    else if (currentPage == 3)
    {
        printValueWithUnit(CoolantTemp, 5 + 11*8, 40 + 19, "*C");
    	printValueWithUnit(inTakeAir,   5 + 11*8, 90 + 15, "*C");

    }
}
void ShowReadDataScreen(void)
{
	ST7735_FillRectangle(0, 35, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
	ST7735_FillRectangle(5, 80, ST7735_WIDTH - 15, 1, ST7735_WHITE);

	if (currentPage == 0)
	{
	    ST7735_WriteString(5, 20 + 19, "Engine Speed", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	    ST7735_WriteString(5, 70 + 15, "Vehicle Speed", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	}
	else if(currentPage == 1)
	{
	    ST7735_WriteString(5, 20 + 19, "Engine", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	    ST7735_WriteString(5, 40 + 19, "load", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	    ST7735_WriteString(5, 70 + 15, "Throttle", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	    ST7735_WriteString(5, 90 + 15, "positon", Font_11x18, ST7735_WHITE, ST7735_BLACK);

	}
	else if(currentPage == 2)
	{

		ST7735_WriteString(10, 20 + 19, "Mass Air Flow", Font_11x18, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(5 + 11*2, 70 + 15, "Fuel Level", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	}

	else if(currentPage == 3)
	{

		ST7735_WriteString(5, 20 + 19, "Engine Coolant", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	    ST7735_WriteString(5, 40 + 19, "Temp:", Font_11x18, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(5, 70 + 15, "Intake Air", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	    ST7735_WriteString(5, 90 + 15, "Temp:", Font_11x18, ST7735_WHITE, ST7735_BLACK);

	}


    showingData = 1;
}

void ShowDTCReadingScreen(void)
{
    ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
    ST7735_WriteString(20, 15, "Stored DTCs", Font_11x18, ST7735_GREEN, ST7735_RED);
	ST7735_FillRectangle(0, 34, ST7735_WIDTH, 1, ST7735_WHITE);
    ST7735_WriteString(20, 60, " Checking...", Font_11x18, ST7735_GREEN, ST7735_BLACK);

}
void ShowDTCCheckScreen(void)
{
    ST7735_FillRectangle(0, 60, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);

    char buffer[8];
    int x, y;

    for (int i = 0; i < DTC_COUNT; i++) {
        DecodeDTC(DTCs.code + i * 2, buffer);  // Mỗi DTC là 2 byte

        if (i < 3) {
            x = 10;
            y = 55 + i * 20;
        } else {
            x = 100;
            y = 55 + (i - 3) * 20;
        }

        ST7735_WriteString(x, y, buffer, Font_11x18, ST7735_WHITE, ST7735_BLACK);
    }
}

void ShowVINReadingScreen(void)
{
	ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
	ST7735_WriteString(60, 15, "VIN", Font_11x18, ST7735_GREEN, ST7735_RED);
	ST7735_FillRectangle(0, 34, ST7735_WIDTH, 1, ST7735_WHITE);
	ST7735_WriteString(20,60 , " Reading... ", Font_11x18, ST7735_GREEN, ST7735_BLACK);


}
void ShowVINScreen(void)
{
    ST7735_FillRectangle(0, 50, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);

    char vinString[18];
    memcpy(vinString, VIN.code,17);
    vinString[17] = '\0';
    ST7735_WriteString(21, 80  , vinString, Font_7x10, ST7735_WHITE, ST7735_BLACK);

}

void ShowLoggingScreen(void)
{
	ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
	ST7735_WriteString(40, 15, "Log Data", Font_11x18, ST7735_GREEN, ST7735_RED);
	ST7735_FillRectangle(0, 34, ST7735_WIDTH, 1, ST7735_WHITE);
	ST7735_WriteString(20,60 , " Logging... ", Font_11x18, ST7735_GREEN, ST7735_BLACK);
    isLogging = 1;
}

void ShowConnectingScreen(void)
{
	ST7735_FillRectangle(0, 50, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
	ST7735_WriteString(5, 75, "Connecting...", Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void ShowMainMenu(void)
{

    ST7735_WriteString(50, 15, "OBD-II", Font_11x18, ST7735_GREEN, ST7735_BLACK);
	ST7735_FillRectangle(0, 34, ST7735_WIDTH, 1, ST7735_WHITE);

    for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++) {
        uint16_t bg = (i == selectedIndex) ? ST7735_RED : ST7735_BLACK;
        uint16_t color = ST7735_WHITE;

        // Hiển thị tên menu
        ST7735_WriteString(33, 20 + 20 + i * 20, menuItems[i], Font_11x18, color, bg);

        // Hiển thị dấu ">" nếu là mục đang ch�?n, ngược lại xóa đi
        ST7735_WriteString(0, 20 + 20 + i * 20, (i == selectedIndex) ? " > " : "   ", Font_11x18, ST7735_BLUE, bg);
    }
}



void display_init(void)
{

	ST7735_Init();
	ST7735_FillScreen(ST7735_BLACK);
	ShowInitScreen();
}

void ShowInitScreen(void)
{
	ST7735_WriteString(5, 5, "OBD-2 Scan&Log", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString(40, 24, " Device", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	ST7735_FillRectangle(0, 43, ST7735_WIDTH, 1, ST7735_WHITE);

	ST7735_WriteString(5, 30 + 20, "Press ENTER to connect", Font_7x10, ST7735_WHITE, ST7735_BLACK);

	ST7735_WriteString(65, 65, "UP", Font_11x18, ST7735_WHITE, ST7735_RED);
	ST7735_WriteString(55, 110, "DOWN", Font_11x18, ST7735_WHITE, ST7735_RED);
	ST7735_WriteString(100, 85, "ENTER", Font_11x18, ST7735_WHITE, ST7735_RED);
	ST7735_WriteString(10, 85, "BACK", Font_11x18, ST7735_WHITE, ST7735_RED);

}

void printNumber(uint8_t x, uint8_t y, uint16_t value)
{
    static char oldBuff[7] = "";  // Lưu giá trị trước đó
    char newBuff[7];
    sprintf(newBuff, "%d", value);

    uint8_t charWidth = 11;  // Font_11x18: rộng 11 pixel
    uint8_t len = strlen(newBuff);

    for (uint8_t i = 0; i < len; i++) {
        if (oldBuff[i] != newBuff[i]) {
            // Ghi lại ký tự mới nếu có sự thay đổi
            char c[2] = {newBuff[i], '\0'};
            ST7735_WriteString(x + i * charWidth, y, c, Font_11x18, ST7735_GREEN, ST7735_BLACK);
        }
    }

    // Nếu độ dài số cũ lớn hơn số mới (VD: từ 999 -> 99), xóa phần dư
    for (uint8_t i = len; i < strlen(oldBuff); i++) {
        ST7735_WriteString(x + i * charWidth, y, " ", Font_11x18, ST7735_GREEN, ST7735_BLACK);
    }

    // Cập nhật buffer cũ
    strcpy(oldBuff, newBuff);
}

void printTime(uint8_t x, uint8_t y, uint16_t value)
{
    char buff[7];
	sprintf(buff, "%d", value);
	if (value < 10)
	{
		ST7735_WriteString(x-7, y, "0", Font_7x10, ST7735_GREEN, ST7735_BLACK);
		ST7735_WriteString(x, y, buff, Font_7x10, ST7735_GREEN, ST7735_BLACK);
	}
	else {
		ST7735_WriteString(x-7, y, buff, Font_7x10, ST7735_GREEN, ST7735_BLACK);
	}
	//ST7735_WriteString(x, y, "", Font_7x10, ST7735_GREEN, ST7735_BLACK);
}

void DecodeDTC(uint8_t *raw, char *output)
{
    char type;
    uint8_t ch1 = (raw[0] >> 6) & 0x03;
    switch (ch1) {
        case 0: type = 'P'; break;
        case 1: type = 'C'; break;
        case 2: type = 'B'; break;
        case 3: type = 'U'; break;
    }

    uint16_t code = ((raw[0] & 0x3F) << 8) | raw[1];
    sprintf(output, "%c%04X", type, code);
}

