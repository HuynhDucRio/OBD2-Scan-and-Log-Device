/*
 * button.c
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#include "button.h"
#include "SDlog.h"

void handle_button(uint16_t GPIO_Pin)
{
	// 50ms debounce
	static uint32_t last_press_time = 0;
	uint32_t now = HAL_GetTick();

	if (now - last_press_time < 100) return;
	    last_press_time = now;

	//Handle
    if (currentMenu == SCREEN_INIT)
    {
        if (GPIO_Pin == GPIO_PIN_5) // OK (ENTER)
        {
            currentMenu = SCREEN_CONNECT;
            ShowConnectingScreen();
            initiating = 1;
        }
    }
    else if (currentMenu == MENU_MAIN)
    {
        if (GPIO_Pin == GPIO_PIN_6) // DOWN
        {
            selectedIndex = (selectedIndex + 1) % MENU_ITEM_COUNT;
            ShowMainMenu();
        }
        else if (GPIO_Pin == GPIO_PIN_7) // UP
        {
            selectedIndex = (selectedIndex == 0) ? MENU_ITEM_COUNT - 1 : selectedIndex - 1;
            ShowMainMenu();
        }
        else if (GPIO_Pin == GPIO_PIN_5) // OK (ENTER)
        {
            if (selectedIndex == 0)
            {
                currentMenu = MENU_READ_DATA;
                currentPage = 0;
                ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
                ST7735_WriteString(31, 15, "Live Data", Font_11x18, ST7735_GREEN, ST7735_RED);
            	ST7735_FillRectangle(0, 34, ST7735_WIDTH, 1, ST7735_WHITE);

                ShowReadDataScreen();
            }
            else if (selectedIndex == 1)
            {
                currentMenu = MENU_DTC_CHECK;
                OBD2_DTC();
                ShowDTCReadingScreen();
            }
            else if (selectedIndex == 2)
            {
                currentMenu = MENU_LOGGING;
                ShowLoggingScreen();
            }
            else if (selectedIndex ==3)
            {
            	currentMenu = MENU_VIN_READ;
            	OBD2_VIN();
            	ShowVINReadingScreen();
            }
        }
    }
    else if (currentMenu == MENU_READ_DATA) //Live Data change pages
    {
    	if (GPIO_Pin == GPIO_PIN_7) //UP
    	{
    		currentPage = (currentPage + 1) % DATA_PAGE_COUNT;
    		ShowReadDataScreen();
    	}
    	else if (GPIO_Pin == GPIO_PIN_6) //DOWN
    	{
    		currentPage = (currentPage == 0) ? DATA_PAGE_COUNT - 1 : currentPage - 1;
    		ShowReadDataScreen();
		}
    	else if (GPIO_Pin == GPIO_PIN_4) //BACK
    	{
			if (showingData == 1)
			{
				showingData = 0;
			}

			ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
			currentMenu = MENU_MAIN;
			ShowMainMenu();
    	}
    }
    else if (currentMenu == MENU_DTC_CHECK ||currentMenu == MENU_LOGGING ||currentMenu  == MENU_VIN_READ )
		{
			if (GPIO_Pin == GPIO_PIN_4) // BACK
			{
				if (isLogging == 1)
				{
	//        		SDLog_Flush();
					isLogging = 0;


				}
				if (showingData == 1)
				{
					showingData = 0;
				}

				ST7735_FillRectangle(0, 15, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
				currentMenu = MENU_MAIN;
				ShowMainMenu();
			}
		}
}
