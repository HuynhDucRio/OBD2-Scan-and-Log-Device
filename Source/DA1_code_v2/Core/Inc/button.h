/*
 * button.h
 *
 *  Created on: May 25, 2025
 *      Author: Le Huynh Duc
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"
#include "UI.h"
#include "OBD2.h"

// Hàm gọi từ HAL_GPIO_EXTI_Callback()
void handle_button(uint16_t GPIO_Pin);

#endif /* INC_BUTTON_H_ */
