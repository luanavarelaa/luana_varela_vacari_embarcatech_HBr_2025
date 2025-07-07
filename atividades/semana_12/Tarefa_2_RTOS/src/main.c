#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buttons.h"
#include "reaction_time.h"
#include "rgb_led.h"
#include "oled.h"

int main(void)
{
    /* Inicializações básicas */
    stdio_init_all();
    oled_init_display();
    oled_clear_display();
    sleep_ms(1000);

    /* Inicializa o módulo de reaction_time (cria a fila) */
    reaction_time_init();

    /* Inicializa os botões com a fila */
    buttons_init(get_button_queue());

    /* Inicializa o gerador de números pseudoaleatórios */
    srand(to_ms_since_boot(get_absolute_time()));

    /* Criação das tarefas */
    xTaskCreate(reaction_time_task,"Reaction Task",1024,NULL,2,NULL);

    xTaskCreate(buttons_task,"Button Task",256,NULL,1,NULL);

    /* Inicia o escalonador*/
    vTaskStartScheduler();

    /* Nunca deverá chegar aqui */
    while (true)
    {
        printf("ERROR: Scheduler exited!\n");
        sleep_ms(1000);
    }
}