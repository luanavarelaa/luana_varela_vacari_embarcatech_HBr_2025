#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hal_led.h"

int main(){
    stdio_init_all();

    if (cyw43_arch_init()){
        return -1;
    }

    while (true){
        hal_led_toggle();
    }
}