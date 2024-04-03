/*! @mainpage Guia 1 - Ejercicio 2
 *
 * @section genDesc General Description
 *
 * El programa hace titilar los leds 1 y 2 al mantener presionada las teclas 1 y 2 correspondientemente y hace
 * titilar el led 3 al presionar simultáneamente las teclas 1 y 2.
 *
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 07/03/2024 | Document creation		                         |
 *
 * @author Tatiana Ailen Wendler (ailuwendler@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
//*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 300
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t teclas;
	LedsInit();
	SwitchesInit();
    while(1)    {
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
    			LedOn(LED_1);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				LedOff(LED_1);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    		break;
    		case SWITCH_2:
    			LedOn(LED_2);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				LedOff(LED_2);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    		break;
			case SWITCH_1|SWITCH_2:
				LedOn(LED_3);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				LedOff(LED_3);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
			break;
    	}
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
		
	}
}