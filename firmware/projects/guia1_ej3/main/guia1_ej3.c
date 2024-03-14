/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
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
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define MODE_ON 1
#define MODE_OFF 2
#define MODE_TOGGLE 3
#define LED1 1
#define LED2 2
#define LED3 3
#define RETARDO 100
/*==================[internal data definition]===============================*/
struct leds
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;      //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;   //indica el tiempo de cada ciclo
} my_leds; 
/*==================[internal functions declaration]=========================*/
void ControlLeds(struct leds *ptr_led) { //Utilizo punteros para, por ejemplo, ahorrar memoria y para poder modificar variables dentro de la función.
	switch(ptr_led->mode)
	{
	case MODE_ON:
		if ( ptr_led->n_led == LED1){
			LedOn(LED1);
		}	
		else if(ptr_led->n_led == LED2){
			LedOn(LED2);
		}
		else {
			LedOn(LED3);
		}
		break;
	case MODE_OFF:
		if ( ptr_led->n_led == LED1){
			LedOff(LED1);
		}	
		else if(ptr_led->n_led == LED2){
			LedOff(LED2);
		}
		else {
			LedOff(LED3);
		}
		break;
	case MODE_TOGGLE:
		for (int i=0; i<ptr_led->n_ciclos; i++){
			if ( ptr_led->n_led == LED1){
					LedToggle(LED1);}	
			else if(ptr_led->n_led == LED2){
					LedToggle(LED2); }
			else {
					LedToggle(LED3); }
			for (int j=0; j<ptr_led->periodo; j++){
					vTaskDelay( RETARDO / portTICK_PERIOD_MS);}	
					}
		break;
	}}
/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	SwitchesInit();

	my_leds.mode = 3;
	my_leds.n_ciclos = 10;
	my_leds.n_led = 2;
	my_leds.periodo = 5;

	ControlLeds(&my_leds);
	

	
}
/*==================[end of file]============================================*/