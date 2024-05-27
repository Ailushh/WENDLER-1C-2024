/*! @mainpage Proyecto Integrador
 *
 * @section genDesc General Description
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	LM35C	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 16/05/2024 | Creación del Documento                         |
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
#include <lm35c.h>
#include <timer_mcu.h>
#include <uart_mcu.h>
#include <analog_io_mcu.h>
/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 2000
#define CONFIG_CONTROL_PERIOD 600000

/*==================[internal data definition]===============================*/
TaskHandle_t MeasureTemperature_task_handle = NULL;
TaskHandle_t AirControl_task_handle = NULL;
TaskHandle_t UVControl_task_handle = NULL;
TaskHandle_t PCControl_task_handle = NULL;
uint16_t temperature = 0;
bool UV = false;
bool PC = false;
/*==================[internal functions declaration]=========================*/
void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(MeasureTemperature_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(AirControl_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(UVControl_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(PCControl_task_handle, pdFALSE);   
}

void FuncTimerB(void* param){

	vTaskNotifyGiveFromISR(AirControl_task_handle, pdFALSE);
}

static void MeasureTemperature(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	temperature = lm35cMeasureTemperature();
	UartSendString(UART_PC, (char *)UartItoa(temperature, 10));
	UartSendString(UART_PC, " °C ");
	UartSendString(UART_PC, "\r\n" );
	}
}

static void UVControl(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (UV){
		GPIOOn(GPIO_11);
		UartSendString(UART_PC, "Lampara UV Encendida");
		UartSendString(UART_PC, "\r\n" );
	}

	else{
		GPIOOff(GPIO_11);
		UartSendString(UART_PC, "Lampara UV Apagada");
		UartSendString(UART_PC, "\r\n" );
	}

}}

static void AirControl(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);


}}

static void PCControl(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (PC){
		GPIOOn(GPIO_13);
		UartSendString(UART_PC, "PC Encendida");
		UartSendString(UART_PC, "\r\n" );
	}

	else{
		GPIOOff(GPIO_13);
		UartSendString(UART_PC, "PC Apagada");
		UartSendString(UART_PC, "\r\n" );
	}

}}

/*==================[external functions definition]==========================*/
void app_main(void){

	lm35cInit(GPIO_10); //Sensor de temperatura

	GPIOInit(GPIO_12, GPIO_OUTPUT); //GPIO que activará el relé que encederá la Lampara UV
	GPIOInit(GPIO_13, GPIO_OUTPUT); //GPIO que activará el relé que encenderá la PC
	
	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL,
	};

	UartInit(&serial_pc);

	timer_config_t timer_measure = {
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

	TimerInit(&timer_measure);
	TimerInit(&timer_control);
	xTaskCreate(&MeasureTemperature, "Sensar la temperatura del ambiente", 512, NULL, 5, &MeasureTemperature_task_handle);
	xTaskCreate(&AirControl, "Controla el Aire Acondicionado", 512, NULL, 5, &AirControl_task_handle);
	xTaskCreate(&UVControl, "Enciende/Apaga Lampara UV", 512, NULL, 5, &UVControl_task_handle);
	xTaskCreate(&PCControl, "Enciende/Apaga PC", 512, NULL, 5, &PCControl_task_handle);
	TimerStart(timer_measure.timer);
	TimerStart(timer_control.timer);
}
/*==================[end of file]============================================*/