#include "sd_card.h"
#include "pico/stdlib.h"
#include "string.h"
#include "sd_card.h"
#include "hw_config.h"
#include "ff.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

// Variável estática para manter o estado do cartão SD
static sd_card_t *sd_card_instance;

FRESULT sd_card_init() {
    sd_card_instance = sd_get_by_num(0);
    return f_mount(&sd_card_instance->fatfs, sd_card_instance->pcName, 1);
}

FRESULT sd_card_write_to_file(const char* filename, const char* data) {
    FIL file;
    FRESULT fr = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        return fr;
    }
    UINT bytes_written;
    fr = f_write(&file, data, strlen(data), &bytes_written);
    f_close(&file);
    return fr;
}

FRESULT sd_card_append_to_csv(const char* filename, const char* data) {
    FIL file;
    FRESULT fr;
    FILINFO fno;
    
    fr = f_stat(filename, &fno);
    
    if (fr == FR_NO_FILE) {
        fr = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if (fr == FR_OK) {
            const char* header = "timestamp,tensão_rms,corrente_rms,energia_consumida,status_tensao\n";
            UINT bytes_written;
            f_write(&file, header, strlen(header), &bytes_written);
            f_close(&file);
        }
    }

    fr = f_open(&file, filename, FA_WRITE | FA_OPEN_APPEND);
    if (fr != FR_OK) {
        return fr;
    }
    
    UINT bytes_written;
    fr = f_write(&file, data, strlen(data), &bytes_written);
    f_close(&file);
    return fr;
}

FRESULT sd_card_unmount() {
    return f_unmount(sd_card_instance->pcName);
}

void sd_card_get_formatted_timestamp(char* buffer, size_t size) {
    datetime_t now;
    rtc_get_datetime(&now);
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d",
            now.year, now.month, now.day, now.hour, now.min, now.sec);
}