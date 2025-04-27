#include "unity.h"
#include "adc.h"

void setUp(void) {
    // código para rodar antes de cada teste
}

void tearDown(void) {
    // código para rodar depois de cada teste
}

void teste_adc_para_calsius(void){
    uint16_t adc_val = (uint16_t)((0.706f*4095.0f)/3.3f);
    float temperaura = adc_to_celsius(adc_val);

    TEST_ASSERT_EQUAL_FLOAT(27.0f, temperaura);
}

int main(void){
    UNITY_BEGIN();
    RUN_TEST(teste_adc_para_calsius);
    return UNITY_END();
}

