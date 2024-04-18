/*! @mainpage Medidor de Distancia por Ultrasonido con Interrupciones y Puerto Serie
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	LCD 	 	| 	GPIO_19     |
 * |				|	GPIO_18		|
 * |				|	GPIO_9		|
 * |	HC-SR04		|	GPIO_03		|
 * |				|	GPIO_02		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 18/04/2024 | Creación del Document                          |
 *
 * @author Tatiana Ailen Wendler (ailuwendler@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <led.h>
#include <switch.h>
#include <hc_sr04.h>
#include <lcditse0803.h>
#include <timer_mcu.h>
#include <uart_mcu.h>
/*==================[macros and definitions]=================================*/
#define CONFIG_MEASURE_PERIOD 500000
/*==================[internal data definition]===============================*/
bool medir_distancia = false;
bool hold_medicion = false;
uint16_t distancia = 0;
TaskHandle_t MedirDistancia_task_handle = NULL;
TaskHandle_t EncenderLedsSegunDistancia_task_handle = NULL;
TaskHandle_t MostrarDistanciaLCD_task_handle = NULL;
TaskHandle_t PuertoSerie_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(MedirDistancia_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(MostrarDistanciaLCD_task_handle, pdFALSE); 
	vTaskNotifyGiveFromISR(EncenderLedsSegunDistancia_task_handle, pdFALSE);   
}

static void MedirDistancia (void *pvParameter){

	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (medir_distancia){

	distancia = HcSr04ReadDistanceInCentimeters();}}

}

static void EncenderLedsSegunDistancia (void *pvParameter){

	while(true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (medir_distancia){

		if (distancia<10){
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

		else if (10<=distancia && distancia<20){
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

		else if(20<=distancia && distancia<30){
			LedOn(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);
		}

		else {
			LedOn(LED_1);
			LedOn(LED_2);
			LedOn(LED_3);
		}}

	else{
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

}}

void static MostrarDistanciaLCD (void *pvParameter){
	while(true){

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if(medir_distancia){

		if (!hold_medicion){

			LcdItsE0803Write(distancia);
			UartSendString(UART_PC, (char *)UartItoa(distancia, 10));
			UartSendString(UART_PC, " cm ");
	}}

	else{ 
		LcdItsE0803Write(0);
	}

}}

void static LeerSwitches (void *pvParameter){

	
	bool *banderas = (bool*) pvParameter;

	*banderas = !*banderas;

}

//void static PuertoSerie (void *pvParameter){

	//uint8_t lectura = UartReadByte();
	
//}
 
/*==================[external functions definition]==========================*/
void app_main(void){

	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();

	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL,
	};

	UartInit(&serial_pc);

	SwitchActivInt(SWITCH_1, LeerSwitches, &medir_distancia);
	SwitchActivInt(SWITCH_2, LeerSwitches, &hold_medicion);

	timer_config_t timer_tareas = {
    	.timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    TimerInit(&timer_tareas);
	xTaskCreate(&MedirDistancia, "Medir_Distancia", 512, NULL, 5, &MedirDistancia_task_handle);
	xTaskCreate(&EncenderLedsSegunDistancia, "Encender/Apagar_Leds", 512, NULL, 5, &EncenderLedsSegunDistancia_task_handle);
	xTaskCreate(&MostrarDistanciaLCD, "Mostrar_distancia_por_LCD", 512, NULL, 5, &MostrarDistanciaLCD_task_handle);
	TimerStart(timer_tareas.timer);




}
/*==================[end of file]============================================*/