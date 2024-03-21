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
void CambiarEstadoGPIO(uint8_t digit, gpioConf_t *ptr_gpio){

	for(int i = 0; i<4; i++){
		GPIOInit(ptr_gpio[i].pin, ptr_gpio[i].dir);
	}
	for (int i = 0; i<4; i++){
		
		if( (digit & (1<<i)) == 0){
			GPIOOff(ptr_gpio[i].pin);
			printf("GPIO%d 0 \n", ptr_gpio[i].pin);

		}
		else{
			GPIOOn(ptr_gpio[i].pin);
			printf("GPIO%d 1 \n", ptr_gpio[i].pin);
		}
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	gpioConf_t bits[4] = {{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};

	uint8_t digito = 4;

	CambiarEstadoGPIO(digito, bits);

}
/*==================[end of file]============================================*/