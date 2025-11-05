/*
 * rtime.h
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#ifndef INC_RTIME_H_
#define INC_RTIME_H_

#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include "ds1307.h"
#include "UI.h"


void timeInit(void);    // In toàn bộ thời gian ban đầu
void timeUpdate(void);  // Cập nhật từng phần nếu có thay đổi

#endif /* INC_RTIME_H_ */
