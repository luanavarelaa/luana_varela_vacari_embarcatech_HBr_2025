#include "adc.h"

float adc_to_celsius(uint16_t adc_val){
    float voltagem = (adc_val * 3.3f)/4095.0f;
    float temperatura = 27.0f - (voltagem - 0.706f)/0.001721f;
    return temperatura;
}
