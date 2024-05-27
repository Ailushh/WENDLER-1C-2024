/**
 * @file lm35c.c
 * @author Wendler Tatiana Ailen (ailuwendler@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "lm35c.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define V_REF 5                /**< Tensión de referencia*/
#define TOTAL_BITS 1024          /**< Cantidad total de bits*/
/*==================[internal data declaration]==============================*/
analog_input_config_t temp_config;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
bool lm35cInit(gpio_t pin){
	

	temp_config.input = pin;
	temp_config.mode = ADC_SINGLE;

	AnalogInputInit(&temp_config);  

	return true;
}

float lm35cMeasureTemperature(void){

	uint16_t value;
	float temperature = 0;
	float valor = 0;
	
	AnalogInputReadSingle(temp_config.input, &value);
	valor = (value/1000.0)/V_REF;
	temperature = valor/10;
	
	return temperature;
}
/*==================[end of file]============================================*/
