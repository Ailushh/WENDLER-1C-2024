/*! @mainpage Examen - Sistema de Irrigación Automática de Plantas
 *
 * @section genDesc General Description
 *
 * El programa consiste en un sistema de irrigación automática de plantas. El mismo consiste en un sensor analógico de pH,
 * un sensor digital de riego que envía un 1 si la planta debe regarse o un 0 si la planta posee buena humedad, una bomba de agua
 * para el riego automático y dos bombas de ph, Acido y Basico.
 * 
 * Cuando el sensor de riego detecta que es necesario regar, debe encenderse la bomba de agua. Si el nivel de pH medido 
 * es menor a 6, se activa la bomba de ph Basico. Si el pH medido es mayor a 6,7 se activa la bomba de pH Acido. 
 * 
 * El valor de pH medido, si la humedad es correcta o no y qué bombas se encuentran encendidas se envía a través de la
 * UART para observarse en el Serial Monitor.
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
/**
 * @def CONFIG_MEASURE_PERIOD
 * @brief Periodo en microsegundos que se utiliza para el timer que controla la medición del Sensor de pH y el de riego y el 
 * 		  encendido/apagado de las bombas. 
 */
#define CONFIG_MEASURE_PERIOD 3000000


/**
 * @def CONFIG_SHOW_PERIOD
 * @brief Periodo en microsegundos que se utiliza para el timer que controla la tarea encargada de mostrar los datos por
 * 		  la UART. 
 */
#define CONFIG_SHOW_PERIOD 5000000


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
bool regar = false;
bool agua = false;
bool phA = false;
bool phB = false;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Timer que coordina las tareas Medir y ControlarBombas.
 * 
 */
void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(Medir_task_handle, pdFALSE);   
	vTaskNotifyGiveFromISR(ControlarBombas_task_handle, pdFALSE);   
}

/**
 * @brief Timer que coordina la tarea encargada de mostrar los datos por la UART.
 * 
 */

void FuncTimerB(void* param){

    vTaskNotifyGiveFromISR(Mostrar_task_handle, pdFALSE); 
}

/**
 * @brief Tarea encargada de Encender/Apagar las bombas de agua, phA y phB según lo indicado por el sensor de riego
 * 		  y según el nivel de pH medido.
 */
static void ControlarBombas(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if(iniciar){
		if(regar){ 
			GPIOOn(GPIO_AGUA);
			agua = true;
		}

		else{
			GPIOOff(GPIO_AGUA);
			agua = false;
		}

		if(pH<6){
			GPIOOn(GPIO_PHB);
			phB = true;
		}

		else{
			GPIOOff(GPIO_PHB);
			phB = false;
		}

		if(pH>6.7){
			GPIOOn(GPIO_PHA);
			phA = true;
		}

		else{
			GPIOOff(GPIO_PHA);
			phA = false;
		}
	}}
}

/**
 * @brief Tarea encargada de determinar el pH medido por el sensor de pH de acuerdo al valor de tensión enviado por el mismo. También	
 * 		  determina si es necesario regar o no de acuerdo a si el sensor de riego envía un 0 y o un 1.
 * 
 */
static void Medir(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (iniciar){
	uint16_t lectura;
	AnalogInputReadSingle(CH1, &lectura); //La función AnalogInputReadSingle ya me devuelve el valor leído en mV
	pH = (lectura*14)/3000; //Sabiendo que para 0V tengo un pH igual a 0, y que para 3000mV (3V) tengo un pH igual a 14

	regar = GPIORead(GPIO_RIEGO); //Leo si el sensor me indica si debo regar o no

}}}

/**
 * @brief Tarea encargada de mostrar por la UART el nivel de pH medido, si la humedad es correcta o no, y qué bombas
 * 		  se encuentran encendidas.
 */
static void Mostrar(void *param){
	while (true){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	if (iniciar){

		UartSendString(UART_PC, " pH: ");
		UartSendString(UART_PC, (char *)UartItoa(pH, 10));

		if(regar){
			UartSendString(UART_PC, ", humedad incorrecta");
			UartSendString(UART_PC, "\r\n" );
		}

		else{
			UartSendString(UART_PC, ", humedad correcta");
			UartSendString(UART_PC, "\r\n" );
		}

		if (phB){
			UartSendString(UART_PC, " Bomba de pHB Encendida ");
			UartSendString(UART_PC, "\r\n" );
		}

		if(phA){
			UartSendString(UART_PC, " Bomba de pHA Encendida ");
			UartSendString(UART_PC, "\r\n" );
		}

		if(agua){
			UartSendString(UART_PC, " Bomba de Agua Encendida ");
			UartSendString(UART_PC, "\r\n" );
		}
		
}}}

/**
 * @brief Funcion de interrupcion de los switches. Al presionar el switch 1 inicia el dispositivo.
 * 		  Con el switch 2 se detiene el dispositivo.
 */
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
        .period = CONFIG_SHOW_PERIOD,
        .func_p = FuncTimerB,
        .param_p = NULL
    };

	TimerInit(&timer_medicion);
	TimerInit(&timer_control);
	xTaskCreate(&Medir, "Sensar el pH y si se necesita regar o no", 512, NULL, 5, &Medir_task_handle);
	xTaskCreate(&ControlarBombas, "Enciende/apaga las bombas según se necesite y", 512, NULL, 5, &ControlarBombas_task_handle);
	xTaskCreate(&Mostrar, "Muestra por la UART si las bombas están encedidas, si la humedad es correcta o no y el pH medido", 512, NULL, 5, &Mostrar_task_handle);
	TimerStart(timer_medicion.timer);
	TimerStart(timer_control.timer);

}
/*==================[end of file]============================================*/