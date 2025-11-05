/*
 * st7735.c
 *
 *  Created on: May 8, 2025
 *      Author: Thanh
 */


#include "stm32f1xx_hal.h"
#include "st7735.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"


#define DELAY 0x80

// based on Adafruit ST7735 library for Arduino
static const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
  init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_INVON, 0 },        //  3: Invert colors
#endif

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Gamma Adjustments (pos. polarity), 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Gamma Adjustments (neg. polarity), 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

static void ST7735_Select() {
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
}

void ST7735_Unselect() {
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

static void ST7735_Reset() {
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
}

static void ST7735_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
}

static void ST7735_ExecuteCommandList(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_Init() {
    ST7735_Select();
    ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    ST7735_Unselect();
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;

    ST7735_Select();

    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    ST7735_Unselect();
}

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}

/*
Simpler (and probably slower) implementation:

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color) {
    uint32_t i, b, j;

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                ST7735_DrawPixel(x + j, y + i, color);
            }
        }
    }
}
*/

void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ST7735_Select();

    while(*str) {
        if(x + font.width >= ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ST7735_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ST7735_Unselect();
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
    if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(&ST7735_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
        }
    }

    ST7735_Unselect();
}

void ST7735_FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
    if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    // Prepare whole line in a single buffer
    uint8_t pixel[] = { color >> 8, color & 0xFF };
    uint8_t *line = malloc(w * sizeof(pixel));
    for(x = 0; x < w; ++x)
    	memcpy(line + x * sizeof(pixel), pixel, sizeof(pixel));

    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--)
        HAL_SPI_Transmit(&ST7735_SPI_PORT, line, w * sizeof(pixel), HAL_MAX_DELAY);

    free(line);
    ST7735_Unselect();
}

void ST7735_FillScreen(uint16_t color) {
    ST7735_FillRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_FillScreenFast(uint16_t color) {
    ST7735_FillRectangleFast(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) return;
    if((y + h - 1) >= ST7735_HEIGHT) return;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    ST7735_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ST7735_Unselect();
}

void ST7735_InvertColors(bool invert) {
    ST7735_Select();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    ST7735_Unselect();
}

void ST7735_SetGamma(GammaDef gamma)
{
	ST7735_Select();
	ST7735_WriteCommand(ST7735_GAMSET);
	ST7735_WriteData(&gamma, sizeof(gamma));
	ST7735_Unselect();
}


////custom functions
//
//void timeUpdate(void)
//{
//    DS1307_ReadTime(&ds1307);
//
//    // Date: Chỉ in nếu khác
//    if (ds1307.date != ds1307_prev.date)
//    {
//    	ST7735_WriteString(5, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//        printTime(5 + 7, 0, ds1307.date);
//    }
//    if (ds1307.month != ds1307_prev.month)
//    {
//    	ST7735_WriteString(5 + 7*3, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//        printTime(5 + 7 + 7*3, 0, ds1307.month);
//    }
//    if (ds1307.year != ds1307_prev.year)
//    {
//    	ST7735_WriteString(5 + 7*6, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//    	printTime(5 + 7 + 7*6, 0, 2000 + ds1307.year);
//    }
//    // Time: Chỉ in nếu khác
//    if (ds1307.hours != ds1307_prev.hours)
//    {
//    	ST7735_WriteString(100, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//    	printTime(100 + 7, 0, ds1307.hours);
//    }
//    if (ds1307.minutes != ds1307_prev.minutes)
//    {
//    	ST7735_WriteString(100 + 7*3, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//    	printTime(100 + 7 + 7*3, 0, ds1307.minutes);
//    }
//    if (ds1307.seconds != ds1307_prev.seconds)
//    {
//    	ST7735_WriteString(100 + 7*6, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//    	printTime(100 + 7 + 7*6, 0, ds1307.seconds);
//    }
//
//    // Cập nhật lại giá trị cũ
//    ds1307_prev = ds1307;
//}
//
//void timeInit(void)
//{
//	ST7735_WriteString(5, 0, "  /  /    ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//	ST7735_WriteString(100, 0, "  :  :  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//	DS1307_ReadTime(&ds1307);
//	printTime(5 + 7, 0, ds1307.date);
//	printTime(5 + 7 + 7*3, 0, ds1307.month);
//	printTime(5 + 7 + 7*6, 0, 2000 + ds1307.year);
//	printTime(100 + 7, 0, ds1307.hours);
//	printTime(100 + 7 + 7*3, 0, ds1307.minutes);
//	printTime(100 + 7 + 7*6, 0, ds1307.seconds);
//
//}
//
//void ShowReadDataScreen(void)
//{
//	ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
//	ST7735_WriteString(20, 15, "OBD-II Data", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    ST7735_WriteString(5, 20 + 15, "RPM:", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    ST7735_WriteString(5, 40 + 15, "Spd(Km/h):", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    ST7735_WriteString(5, 60 + 15, "Temp:", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    ST7735_WriteString(5, 80 + 15, "ThrPos(%):", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    showingData = 1;
//}
//
//void ShowDTCCheckScreen(void)
//{
//	ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
//    ST7735_WriteString(0, 15, "DTC:", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//}
//
//void ShowLoggingScreen(void)
//{
//	ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
//    ST7735_WriteString(0, 15, "Logging...", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    // Nếu muốn làm hiệu ứng hoạt ảnh thì cần timer update chuỗi liên
//    isLogging = 1;
//}
//
//void ShowConnectingScreen(void)
//{
//	ST7735_FillScreen(ST7735_BLACK);
//
//	ST7735_WriteString(5, 5, "OBD-2 Scan&Log", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//	ST7735_WriteString(40, 24, " Device", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//
//	ST7735_WriteString(50, 85, "Connecting...", Font_7x10, ST7735_WHITE, ST7735_BLACK);
//
//}
//
//void ShowMainMenu(void)
//{
//	timeInit();
//    ST7735_WriteString(50, 15, "OBD-II", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++) {
//        uint16_t bg = (i == selectedIndex) ? ST7735_RED : ST7735_BLACK;
//        uint16_t color = ST7735_GREEN;
//
//        // Hiển thị tên menu
//        ST7735_WriteString(11, 20 + 20 + i * 20, menuItems[i], Font_11x18, color, bg);
//
//        // Hiển thị dấu ">" nếu là mục đang ch�?n, ngược lại xóa đi
//        ST7735_WriteString(0, 20 + 20 + i * 20, (i == selectedIndex) ? ">" : " ", Font_11x18, color, bg);
//    }
//}
//
//void showData(void)
//{
//	printNumber(5 + 11*5, 20 + 15, (uint16_t)RPM);
//	printNumber(5 + 11*11, 40 + 15, (uint16_t)Speed);
//	printNumber(5 + 11*7, 60 + 15, (uint16_t)inTakeAir);
//	printNumber(5 + 11*11, 80 + 15, (uint16_t)ThrPos);
//}
//
//void display_init(void)
//{
//
//	ST7735_Init();
//	ST7735_FillScreen(ST7735_BLACK);
//	ShowInitScreen();
//}
//
//void ShowInitScreen(void)
//{
//	ST7735_WriteString(5, 5, "OBD-2 Scan&Log", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//	ST7735_WriteString(40, 24, " Device", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//
//	ST7735_WriteString(15, 30 + 20, "Press OK to connect!", Font_7x10, ST7735_WHITE, ST7735_BLACK);
//
//	ST7735_WriteString(70, 65, "UP", Font_11x18, ST7735_WHITE, ST7735_RED);
//	ST7735_WriteString(60, 110, "DOWN", Font_11x18, ST7735_WHITE, ST7735_RED);
//	ST7735_WriteString(110, 85, "OK", Font_11x18, ST7735_WHITE, ST7735_RED);
//	ST7735_WriteString(20, 85, "BACK", Font_11x18, ST7735_WHITE, ST7735_RED);
//
//}
//
//void printNumber(uint8_t x, uint8_t y, uint16_t value)
//{
//    static char oldBuff[7] = "";  // Lưu giá trị trước đó
//    char newBuff[7];
//    sprintf(newBuff, "%d", value);
//
//    uint8_t charWidth = 11;  // Font_11x18: rộng 11 pixel
//    uint8_t len = strlen(newBuff);
//
//    for (uint8_t i = 0; i < len; i++) {
//        if (oldBuff[i] != newBuff[i]) {
//            // Ghi lại ký tự mới nếu có sự thay đổi
//            char c[2] = {newBuff[i], '\0'};
//            ST7735_WriteString(x + i * charWidth, y, c, Font_11x18, ST7735_GREEN, ST7735_BLACK);
//        }
//    }
//
//    // Nếu độ dài số cũ lớn hơn số mới (VD: từ 999 -> 99), xóa phần dư
//    for (uint8_t i = len; i < strlen(oldBuff); i++) {
//        ST7735_WriteString(x + i * charWidth, y, " ", Font_11x18, ST7735_GREEN, ST7735_BLACK);
//    }
//
//    // Cập nhật buffer cũ
//    strcpy(oldBuff, newBuff);
//}
//
//void printTime(uint8_t x, uint8_t y, uint16_t value)
//{
//    char buff[7];
//	sprintf(buff, "%d", value);
//	if (value < 10)
//	{
//		ST7735_WriteString(x-7, y, "0", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//		ST7735_WriteString(x, y, buff, Font_7x10, ST7735_GREEN, ST7735_BLACK);
//	}
//	else {
//		ST7735_WriteString(x-7, y, buff, Font_7x10, ST7735_GREEN, ST7735_BLACK);
//	}
//	//ST7735_WriteString(x, y, "", Font_7x10, ST7735_GREEN, ST7735_BLACK);
//}
