/*
 * rtime.c
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#include "rtime.h"

void timeUpdate(void)
{
    DS1307_ReadTime(&ds1307);

    // Date: Chỉ in nếu khác
    if (ds1307.date != ds1307_prev.date)
    {
    	ST7735_WriteString(5, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
        printTime(5 + 7, 0, ds1307.date);
    }
    if (ds1307.month != ds1307_prev.month)
    {
    	ST7735_WriteString(5 + 7*3, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
        printTime(5 + 7 + 7*3, 0, ds1307.month);
    }
    if (ds1307.year != ds1307_prev.year)
    {
    	ST7735_WriteString(5 + 7*6, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
    	printTime(5 + 7 + 7*6, 0, 2000 + ds1307.year);
    }
    // Time: Chỉ in nếu khác
    if (ds1307.hours != ds1307_prev.hours)
    {
    	ST7735_WriteString(100, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
    	printTime(100 + 7, 0, ds1307.hours);
    }
    if (ds1307.minutes != ds1307_prev.minutes)
    {
    	ST7735_WriteString(100 + 7*3, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
    	printTime(100 + 7 + 7*3, 0, ds1307.minutes);
    }
    if (ds1307.seconds != ds1307_prev.seconds)
    {
    	ST7735_WriteString(100 + 7*6, 0, "  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
    	printTime(100 + 7 + 7*6, 0, ds1307.seconds);
    }

    // Cập nhật lại giá trị cũ
    ds1307_prev = ds1307;
}

void timeInit(void)
{
	ST7735_WriteString(5, 0, "  /  /    ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
	ST7735_WriteString(100, 0, "  :  :  ", Font_7x10, ST7735_GREEN, ST7735_BLACK);
	DS1307_ReadTime(&ds1307);
	printTime(5 + 7, 0, ds1307.date);
	printTime(5 + 7 + 7*3, 0, ds1307.month);
	printTime(5 + 7 + 7*6, 0, 2000 + ds1307.year);
	printTime(100 + 7, 0, ds1307.hours);
	printTime(100 + 7 + 7*3, 0, ds1307.minutes);
	printTime(100 + 7 + 7*6, 0, ds1307.seconds);

}
