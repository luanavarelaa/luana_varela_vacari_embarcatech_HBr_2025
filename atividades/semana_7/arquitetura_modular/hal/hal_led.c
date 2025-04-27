#include "hal_led.h"
#include "led_embutido.h"
#include "pico/stdlib.h"

void hal_led_toggle(){
    led_on();
    sleep_ms(500);
    led_off();
    sleep_ms(500);
}