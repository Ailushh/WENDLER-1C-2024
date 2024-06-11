/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  		|  		 ESP32   		|
 * |:----------------------:|:----------------------|
 * | 	Sensor de Riego	 	| 		GPIO_22			|
 * | 	Sensor de pH		|		CH1				|
 * |	Bomba de Agua		|		GPIO_21			|
 * |	Bomba de phA		|		GPIO_20			|
 * | 	Bomba de phB		|		GPIO_19			|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/06/2024 | Document creation		                         |
 *
 * @author Wendler Tatiana Ailen (ailuwendler@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_MEASURE_PERIOD 500000
#define GPIO_RIEGO GPIO_22
#define GPIO_AGUA GPIO_21
#define GPIO_PHA GPIO_20
#define GPIO_PHB GPIO_19
/*==================[internal data definition]===============================*/
bool iniciar = false;
bool detener = false;
uint16_t pH = 0;
/*==================[internal functions declaration]=========================*/
static void ControlarBombas(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}}

static void MedirpH(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	uint16_t lectura;
	AnalogInputReadSingle(CH1, &lectura);
	UartSendString(UART_PC, (char *)UartItoa(lectura, 10));
	UartSendString(UART_PC, "\r" );
}}

void static LeerSwitches (void *pvParameter){

	
	bool *banderas = (bool*) pvParameter;

	*banderas = !*banderas;

}

/*==================[external functions definition]==========================*/
void app_main(void){
	
	SwitchesInit();

	GPIOInit(GPIO_AGUA, GPIO_OUTPUT); //GPIO que activará la bomba de agua
	GPIOInit(GPIO_PHA, GPIO_OUTPUT); //GPIO que activará la bomba de pHa
	GPIOInit(GPIO_PHB, GPIO_OUTPUT); //GPIO que activará la bomba de pHb
	GPIOInit(GPIO_RIEGO, GPIO_INPUT); //GPIO que recibirá el dato del sensor de Riego

	analog_input_config_t entrada_sensorpH ={
		.input = CH1,
		.mode = ADC_SINGLE,
	};

	AnalogInputInit(&entrada_sensorpH);

	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL,
	};

	UartInit(&serial_pc);

	SwitchActivInt(SWITCH_1, LeerSwitches, &iniciar);
	SwitchActivInt(SWITCH_2, LeerSwitches, &detener);

}
/*==================[end of file]============================================*/