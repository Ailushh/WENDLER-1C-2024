#ifndef LM35C_H
#define LM35C_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup Sensor_t Sensor de Temperatura
 ** @{ */

/** \brief Sensor_t driver for the ESP-EDU Board.
 *
 * 
 * @author Wendler Tatiana Ailen
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 23/05/2023 | Creación del Documento                        						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief 
 * 
 * @param pin 
 * @return true 
 * @return false 
 */
bool lm35cInit(gpio_t pin);

float lm35cMeasureTemperature(void);


#endif /* #ifndef LM35C_H */

/*==================[end of file]============================================*/

