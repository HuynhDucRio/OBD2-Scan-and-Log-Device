/*
 * SDlog.c
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */


#include "SDlog.h"
#include "string.h"
#include "stdio.h"


static FATFS fs;
static FIL file;
static char logBuffer[LOG_BUFFER_SIZE];
static char line[128];
static FRESULT     fres;
static UINT bw;

static UINT logLen = 0;
static uint8_t isFileOpen = 0;

void SDLog_Init(void)
{
   fres = f_mount(&fs, "", 1);

    if (f_open(&file, "obdlog.csv", FA_OPEN_ALWAYS | FA_WRITE) == FR_OK)
    {
        if (f_size(&file) == 0)
        {
            const char *header = "Timestamp,RPM,Speed,Temperature,Throttle Position\r\n";
            f_write(&file, header, strlen(header), &bw);
        }

        // Seek đến cuối file
        f_lseek(&file, f_size(&file));
        isFileOpen = 1;
    }
}

void SDLog_AddEntry(uint16_t rpm, uint8_t speed, int8_t temp, uint8_t tp)
{
    if (!isFileOpen) return;

    int len = snprintf(logBuffer + logLen, LOG_BUFFER_SIZE - logLen,
                       "%04d-%02d-%02d %02d:%02d:%02d,%d,%d,%d,%d\r\n",
                       2000 + ds1307.year, ds1307.month, ds1307.date,
                       ds1307.hours, ds1307.minutes, ds1307.seconds,
                       rpm, speed, temp, tp);

    logLen += len;

    if (logLen >= LOG_BUFFER_SIZE - 64)  // Đệm gần đầy, ghi ra file
        SDLog_Flush();
}

void SDLog_Flush(void)
{
    if (!isFileOpen || logLen == 0) return;

    UINT bw;
    f_write(&file, logBuffer, logLen, &bw);
    f_sync(&file);
    logLen = 0;
}

uint8_t SDLog_IsBufferNearlyFull(void)
{
    return (logLen >= LOG_BUFFER_SIZE - 64);  // 64 byte cuối được giữ để tránh tràn
}

void log_data_to_csv(uint16_t rpm, uint8_t speed, uint8_t Atemp, uint8_t tp, uint8_t load, uint8_t Ctemp, uint16_t MAF, uint8_t fuel ) {
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);


	    fres = f_mount(&fs, "", 1);// mount

	    if (f_open(&file, "obdlog6.csv", FA_OPEN_ALWAYS | FA_WRITE) == FR_OK) {
	        if (f_size(&file) == 0) {
	            // Nếu file mới tạo, ghi header trước
	            char header[] = "Timestamp,Engine Speed,Vehicle Speed,In-Take Air Temperature,Throttle Position,Engine Load, Engine Coolant Temperature, Mass Air Flow, Fuel Level\r\n";
	            f_write(&file, header, strlen(header), &bw);
	        }

	        sprintf(line, "%04d-%02d-%02d %02d:%02d:%02d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
	            2000 + ds1307.year, ds1307.month, ds1307.date ,
				ds1307.hours, ds1307.minutes, ds1307.seconds,
	            rpm, speed, Atemp, tp, load, Ctemp, MAF, fuel);

	        f_lseek(&file, f_size(&file));
	        f_write(&file, line, strlen(line), &bw);
	        f_close(&file);
	    } else {
	        // Gợi ý: in lỗi qua UART nếu cần
	        char err[] = "Failed to open CSV file!\r\n";
	        HAL_UART_Transmit(&huart1, (uint8_t*)err, strlen(err), HAL_MAX_DELAY);
	    }

	    f_mount(NULL, "", 0); // unmount
}
