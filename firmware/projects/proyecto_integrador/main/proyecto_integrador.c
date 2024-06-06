/*! @mainpage Proyecto Integrador
 *
 * @section genDesc General Description
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  	 |        ESP32   	    |
 * |:-------------------:|:---------------------|
 * | 	LM35C	 		 | 	 GPIO_01 (CH1)		|
 * |	RELÉ LÁMPARA UV	 |	 GPIO_22			|
 * |	RELÉ PC			 |	 GPIO_21			|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | 						Description                             |
 * |:----------:|:--------------------------------------------------------------|
 * | 16/05/2024 | Creación del Documento                         				|
 * | 30/05/2024	| Se comprueba correcto funcionamiento del sensor de temperatura|
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
#include <ble_mcu.h>
#include <time.h>
#include <led.h>
/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 1000000
#define CONFIG_CONTROL_PERIOD 60000000
#define GPIO_PC GPIO_22
#define GPIO_UV GPIO_21

/*==================[internal data definition]===============================*/
TaskHandle_t MeasureTemperature_task_handle = NULL;
TaskHandle_t IRControl_task_handle = NULL;
TaskHandle_t DeviceControl_task_handle = NULL;
TaskHandle_t ReadTime_task_handle = NULL;
uint16_t temperature = 0;
bool UV = false;
bool PC = false;
bool AIR = false;
int hours, minutes, day, month, year;
/*==================[internal functions declaration]=========================*/
void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(MeasureTemperature_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(DeviceControl_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(ReadTime_task_handle, pdFALSE);      
}

void FuncTimerB(void* param){

	vTaskNotifyGiveFromISR(IRControl_task_handle, pdFALSE);
}

static void MeasureTemperature(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	temperature = lm35cMeasureTemperature();
	UartSendString(UART_PC, (char *)UartItoa(temperature, 10));
	UartSendString(UART_PC, " °C ");
	UartSendString(UART_PC, "\r\n" );

}}

static void DeviceControl(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (UV){
		GPIOOn(GPIO_UV);
		UartSendString(UART_PC, "Lampara UV Encendida");
		UartSendString(UART_PC, "\r\n" );
	}

	else{
		GPIOOff(GPIO_UV);
		UartSendString(UART_PC, "Lampara UV Apagada");
		UartSendString(UART_PC, "\r\n" );
	}

	if (PC){
		GPIOOn(GPIO_PC);
		UartSendString(UART_PC, "PC Encendida");
		UartSendString(UART_PC, "\r\n" );
	}

	else{
		GPIOOff(GPIO_PC);
		UartSendString(UART_PC, "PC Apagada");
		UartSendString(UART_PC, "\r\n" );
	}

}}

static void IRControl(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (AIR){
	}
}}


static void ReadTime(void *param){
	while (true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	//Extraigo la hora y fecha actual
	time_t now = time(NULL);
  	struct tm *local = localtime(&now);

	year = local->tm_year + 1900;
    month = local->tm_mon + 1;
    day = local->tm_mday;
    hours = local->tm_hour;
    minutes = local->tm_min;

}}
void ReadApp(uint8_t * data, uint8_t length){
	while (true){

	UartSendString(UART_PC, " BLUETOOTH CONECTADO ");
	UartSendString(UART_PC, "\r\n" );

}
}
void ReadUart(){

	uint8_t lectura; 
	UartReadByte(UART_PC, &lectura);

	if (lectura == 'O'){
		UV = !UV;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	lm35cInit(CH1); //Sensor de temperatura

	GPIOInit(GPIO_PC, GPIO_OUTPUT); //GPIO que activará el relé que encederá la Lampara UV
	GPIOInit(GPIO_UV, GPIO_OUTPUT); //GPIO que activará el relé que encenderá la PC

	ble_config_t ble_configuration = {
        "ESP_EDU_AILU",
        ReadApp
    };
	
	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = ReadUart,
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

	BleInit(&ble_configuration);
	TimerInit(&timer_measure);
	TimerInit(&timer_control);
	xTaskCreate(&MeasureTemperature, "Sensar la temperatura del ambiente", 512, NULL, 5, &MeasureTemperature_task_handle);
	xTaskCreate(&IRControl, "Controla el Aire Acondicionado", 512, NULL, 5, &IRControl_task_handle);
	xTaskCreate(&DeviceControl, "Enciende/Apaga Lampara UV y PC", 512, NULL, 5, &DeviceControl_task_handle);
	xTaskCreate(&ReadTime, "Lee la fecha y hora actual", 512, NULL, 5, &ReadTime_task_handle);
	TimerStart(timer_measure.timer);
	TimerStart(timer_control.timer);
}
/*==================[end of file]============================================*/