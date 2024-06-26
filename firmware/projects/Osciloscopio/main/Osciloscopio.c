/*! @mainpage Osciloscopio - Projecto 2 Ejercicio 4
 *
 * @section genDesc General Description
 *
 * Programa que permite leer una señal analógica y convertirla a digital y viceversa. 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	CH1 	 	| 	GPIO1		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 25/04/2024 | Creación del Documento                         |
 * | 02/05/2024 | Finalizacion y Documentacion					 |
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
#include <timer_mcu.h>
#include <uart_mcu.h>
#include <analog_io_mcu.h>
#include <switch.h>
/*==================[macros and definitions]=================================*/

/** @def CONFIG_MEASURE_PERIOD
 *  @brief Periodo en microsegundos que se utiliza para el timer que controla la conversion de analógico
 * a digital. Se calcula considerando la frecuencia de muestreo.
*/
#define CONFIG_MEASURE_PERIOD 2000

/** @def BUFFER_SIZE
 * @brief Tamanio del buffer que posee los datos del ecg.
*/
#define BUFFER_SIZE 231

/*==================[internal data definition]===============================*/
TaskHandle_t ConvertirADigital_task_handle = NULL;
TaskHandle_t ConvertirAAnalogico_task_handle = NULL;
int indice = 0;

/**
 * @brief Periodo en microsegundos que utiliza el timer que controla la conversion de digital a analógico.
 */
int ECG_FREQUENCY = 4329;

bool actualizar_ecg = false;

/**
 * @brief Arreglo que contiene los datos del ecg.
 */

const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

/**
 * @brief Timer que coordina la conversion digital a analogico del ecg.
 */
timer_config_t timer_ecg = {
    	.timer = TIMER_B,
        .period = 0,
        .func_p = NULL,
        .param_p = NULL
    };

/*==================[internal functions declaration]=========================*/

/**
 * @brief Funcion que se ejecuta en cada interrupcion del timer encargado de llamar a la conversion analogica-digital.
 */
void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(ConvertirADigital_task_handle, pdFALSE);   
}

/**
 * @brief Funcion que se ejecuta en cada interrupcion del timer encargado de llamar a la conversion digital-analogica del ecg.
 */
void FuncTimerB(void* param){

    vTaskNotifyGiveFromISR(ConvertirAAnalogico_task_handle, pdFALSE);   
}

/**
 * @brief Funcion que permite leer una entrada analógica y convertirla a Digital. 
*/

static void ConvertirADigital(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	uint16_t lectura;
	AnalogInputReadSingle(CH1, &lectura);
	UartSendString(UART_PC, (char *)UartItoa(lectura, 10));
	UartSendString(UART_PC, "\r" );
}}

/**
 * @brief Funcion que permite convertir la señal digital de ecg a analógica.
 */

static void ConvertirAAnalogico(void *param){

	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (indice == BUFFER_SIZE){
		indice = 0;
	}

	uint8_t dato;
	dato = ecg[indice];

	AnalogOutputWrite(dato);

	indice++;
}}

/**
 * @brief Funcion de interrupcion de los switches. Al presionar el switch 1 se aumenta la frecuencia del muestro del ecg.
 * Con el switch 2 se disminuye la frecuencia del ecg. 
 */
void static LeerSwitches (void *pvParameter){

	int tecla;

	tecla = SwitchesRead();

	switch (tecla)
	{
	case SWITCH_1:
		ECG_FREQUENCY = ECG_FREQUENCY - 100;
		timer_ecg.period = ECG_FREQUENCY;
		TimerInit(&timer_ecg);
		TimerStart(timer_ecg.timer);
		break;
	
	case SWITCH_2:
		ECG_FREQUENCY = ECG_FREQUENCY + 100;
		timer_ecg.period = ECG_FREQUENCY;
		TimerInit(&timer_ecg);
		TimerStart(timer_ecg.timer);
		break;
	}
}


/*==================[external functions definition]==========================*/
void app_main(void){

	SwitchesInit();

	analog_input_config_t entrada_analogica ={
		.input = CH1,
		.mode = ADC_SINGLE,
	};
	
	AnalogInputInit(&entrada_analogica);
	AnalogOutputInit();

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

	timer_ecg.period = ECG_FREQUENCY;
	timer_ecg.func_p = FuncTimerB;

	TimerInit(&timer_medicion);
	TimerInit(&timer_ecg);
	xTaskCreate(&ConvertirADigital, "Convertir señal a Digital", 512, NULL, 5, &ConvertirADigital_task_handle);
	xTaskCreate(&ConvertirAAnalogico, "Convertir señal a Analogica", 512, NULL, 5, &ConvertirAAnalogico_task_handle);
	TimerStart(timer_medicion.timer);
	TimerStart(timer_ecg.timer);
	
}
/*==================[end of file]============================================*/