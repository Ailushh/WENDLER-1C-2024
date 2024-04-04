/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1, LED_2 and LED_3 blink at different rates, using FreeRTOS tasks.
 * 
 * Permite hacer que titilen los tres leds en diferentes periodos. Si utilizo bare metal, como están todos dentro del mismo loop, sería difícil configurar diferentes frecuencias entre cada led si las mismas no son múltiplos entre sí.
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
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_LED_1 1000
#define CONFIG_BLINK_PERIOD_LED_2 1500
#define CONFIG_BLINK_PERIOD_LED_3 500
/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
TaskHandle_t led2_task_handle = NULL;
TaskHandle_t led3_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

static void Led1Task(void *pvParameter){ //Tarea 1
    while(true){
        printf("LED_1 ON\n");
        LedOn(LED_1);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_1 / portTICK_PERIOD_MS); //vTaskDelay también es una función del kernel del sistema operativo. Me permite liberar el micro.
        printf("LED_1 OFF\n");
        LedOff(LED_1);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_1 / portTICK_PERIOD_MS); //Si no libero el micro nunca sale del while infinito y no puedo delegar el control a otras tareas.
    }
}

static void Led2Task(void *pvParameter){ //Tarea 2
    while(true){
        printf("LED_2 ON\n");
        LedOn(LED_2);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_2 / portTICK_PERIOD_MS);
        printf("LED_2 OFF\n");
        LedOff(LED_2);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_2 / portTICK_PERIOD_MS);
    }
}

static void Led3Task(void *pvParameter){ //Tarea 3
    while(true){
        printf("LED_3 ON\n");
        LedOn(LED_3);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_3 / portTICK_PERIOD_MS);
        printf("LED_3 OFF\n");
        LedOff(LED_3);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_3 / portTICK_PERIOD_MS);
    }
}

/*==================[external functions definition]==========================*/

void app_main(void){ //app_main es la primer tarea que se ejecuta y crea más tareas.
    LedsInit();
    xTaskCreate(&Led1Task, "LED_1", 512, NULL, 5, &led1_task_handle); //Todas las tareas que empiezan con x son parte del kernel del sistema operativo
    xTaskCreate(&Led2Task, "LED_2", 512, NULL, 5, &led2_task_handle); //El 512 es el tamaño (en bytes) de RAM que le asigno a la tarea. El NULL es el parámetro que quiero con el que arranque la tarea, en este caso no necesita nada para arrancar.
    xTaskCreate(&Led3Task, "LED_3", 512, NULL, 5, &led3_task_handle); //El 5 es la prioridad que tiene la tarea. Cuanto más grande el número, más alta la prioridad.
}
