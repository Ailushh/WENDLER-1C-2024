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
#include <gpio_mcu.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
void MostrarDisplay (uint32_t data, uint8_t digits, gpioConf_t *ptr_bits, gpioConf_t *ptr_lcd ){
	printf("hola");
}
/*==================[external functions definition]==========================*/
void app_main(void){
	gpioConf_t bits[4] = {{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};
	gpioConf_t LCD[3] = {{GPIO_19,GPIO_OUTPUT},{GPIO_18,GPIO_OUTPUT},{GPIO_9,GPIO_OUTPUT}};

	
}
/*==================[end of file]============================================*/