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
#define CONFIG_MEASURE_PERIOD 3000000
#define CONFIG_CONTROL_PERIOD 5000000
#define GPIO_RIEGO GPIO_22
#define GPIO_AGUA GPIO_21
#define GPIO_PHA GPIO_20
#define GPIO_PHB GPIO_19
TaskHandle_t ControlarBombas_task_handle = NULL;
TaskHandle_t Medir_task_handle = NULL;
TaskHandle_t Mostrar_task_handle = NULL;
/*==================[internal data definition]===============================*/
bool iniciar = false;
float pH = 0;
bool agua = false;
bool phA = false;
bool phB = false;
/*==================[internal functions declaration]=========================*/

void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(Medir_task_handle, pdFALSE);   
	vTaskNotifyGiveFromISR(ControlarBombas_task_handle, pdFALSE);   
}

void FuncTimerB(void* param){

    vTaskNotifyGiveFromISR(Mostrar_task_handle, pdFALSE); 
}

static void ControlarBombas(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if(iniciar){
		if(agua){ 
			GPIOOn(GPIO_AGUA);

			UartSendString(UART_PC, " Bomba de Agua Encendida ");
			UartSendString(UART_PC, "\r\n" );
		}

		else{
			GPIOOff(GPIO_AGUA);
		}

		if(pH<6){
			GPIOOn(GPIO_PHB);
			UartSendString(UART_PC, " Bomba de pHB Encendida ");
			UartSendString(UART_PC, "\r\n" );
		}

		if(pH>6.7){
			GPIOOn(GPIO_PHA);
			UartSendString(UART_PC, " Bomba de pHA Encendida ");
			UartSendString(UART_PC, "\r\n" );
		}
	}}
}

static void Medir(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (iniciar){
	uint16_t lectura;
	AnalogInputReadSingle(CH1, &lectura); //La función AnalogInputReadSingle ya me devuelve el valor leído en mV
	pH = (lectura*14)/3000; //Sabiendo que para 0V tengo un pH igual a 0, y que para 3000mV (3V) tengo un pH igual a 14

	//Como el pH es un valor flotante, debo transforma a ASCII el número antes y después de la coma
	UartSendString(UART_PC, (char *)UartItoa(lectura, 10));
	UartSendString(UART_PC, "\r" );

	agua = GPIORead(GPIO_RIEGO); //Leo si el sensor me indica si debo regar o no

}}}

static void Mostrar(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (iniciar){
	

}}}

void static LeerSwitches (void *pvParameter){

	int tecla;

	tecla = SwitchesRead();

	switch (tecla)
	{
	case SWITCH_1:
		iniciar = true;
		break;
	
	case SWITCH_2:
		iniciar = false;
		break;
	}

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

	SwitchActivInt(SWITCH_1, LeerSwitches, NULL);
	SwitchActivInt(SWITCH_2, LeerSwitches, NULL);

	timer_config_t timer_medicion = {
    	.timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

	timer_config_t timer_control = {
    	.timer = TIMER_B,
        .period = CONFIG_CONTROL_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };

	TimerInit(&timer_medicion);
	TimerInit(&timer_control);
	xTaskCreate(&Medir, "Sensar el pH y si se necesita regar o no", 512, NULL, 5, &Medir_task_handle);
	xTaskCreate(&ControlaryMostrar, "Enciende/apaga las bombas según se necesite y muestra a través de la UART si las mismas están encendidas y el nivel de pH medido", 512, NULL, 5, &ControlaryMostrar_task_handle);
	TimerStart(timer_medicion.timer);
	TimerStart(timer_control.timer);

}
/*==================[end of file]============================================*/