/*
 * Lilygo E-Paper display example for ESP-IDF.
*/
// #ifndef BOARD_HAS_PSRAM
// #error "Please enable PSRAM !!!"
// #endif

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <esp_task_wdt.h>
#include "epd_driver.h"
#include "firasans.h"
#include "esp_adc_cal.h"
#include "logo.h"

#define BATT_PIN            36
#define SD_MISO             12
#define SD_MOSI             13
#define SD_SCLK             14
#define SD_CS               15

uint8_t *framebuffer;
int vref = 1100;

void delay(int ms) {
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

void setup()
{
    char buf[128];

    // Serial.begin(115200);
    /*
    * SD Card test
    * Only as a test SdCard hardware, use example reference
    * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD/examples
    * * */
    snprintf(buf, 128, "➸ Don't care about SdCard :-)");
    // SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    // bool rlst = SD.begin(SD_CS);
    // if (!rlst) {
    //     Serial.println("SD init failed");
    //     snprintf(buf, 128, "➸ No detected SdCard");
    // } else {
    //     snprintf(buf, 128, "➸ Detected SdCard insert:%.2f GB", SD.cardSize() / 1024.0 / 1024.0 / 1024.0);
    // }

    // Correct the ADC reference voltage
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("eFuse Vref:%u mV\n", adc_chars.vref);
        vref = adc_chars.vref;
    }

    epd_init();

    framebuffer = (uint8_t *)malloc(sizeof(uint8_t) * EPD_WIDTH * EPD_HEIGHT / 2);
    // framebuffer = (uint8_t *)heap_caps_malloc(sizeof(uint8_t) * EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    // framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer) {
        printf("alloc memory failed !!!\n");
        while (1);
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    Rect_t area = {
        .x = 230,
        .y = 20,
        .width = logo_width,
        .height = logo_height,
    };

    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(area, (uint8_t *)logo_data);
    epd_poweroff();


    int cursor_x = 200;
    int cursor_y = 250;

    char *string1 = "➸ 16 color grayscale  😀 \n";
    char *string2 = "➸ Use with 4.7\" EPDs 😍 \n";
    char *string3 = "➸ High-quality font rendering ✎🙋";
    char *string4 = "➸ ~630ms for full frame draw 🚀\n";

    epd_poweron();

    writeln((GFXfont *)&FiraSans, buf, &cursor_x, &cursor_y, NULL);
    delay(500);
    cursor_x = 200;
    cursor_y += 50;
    writeln((GFXfont *)&FiraSans, string1, &cursor_x, &cursor_y, NULL);
    delay(500);
    cursor_x = 200;
    cursor_y += 50;
    writeln((GFXfont *)&FiraSans, string2, &cursor_x, &cursor_y, NULL);
    delay(500);
    cursor_x = 200;
    cursor_y += 50;
    writeln((GFXfont *)&FiraSans, string3, &cursor_x, &cursor_y, NULL);
    delay(500);
    cursor_x = 200;
    cursor_y += 50;
    writeln((GFXfont *)&FiraSans, string4, &cursor_x, &cursor_y, NULL);
    delay(500);

    epd_poweroff();

}

void loop()
{
    // When reading the battery voltage, POWER_EN must be turned on
    epd_poweron();

    // FIXME?
    // uint16_t v = analogRead(BATT_PIN);
    // float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    float battery_voltage = 42.0f;

    char voltage[30];
    sprintf(voltage, "➸ Voltage : %fV", battery_voltage);
    puts(voltage);

    Rect_t area = {
        .x = 200,
        .y = 460,
        .width = 320,
        .height = 50,
    };

    int cursor_x = 200;
    int cursor_y = 500;
    epd_clear_area(area);
    writeln((GFXfont *)&FiraSans, (char *)voltage, &cursor_x, &cursor_y, NULL);


    // There are two ways to close


    // It will turn off the power of the ink screen, but cannot turn off the blue LED light.
    // epd_poweroff();

    //It will turn off the power of the entire
    // POWER_EN control and also turn off the blue LED light
    epd_poweroff_all();
    delay(5000);
}


void helloWorld();

void app_main(void) {
  // helloWorld();
  setup();
  loop();
}

void helloWorld() {
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Free heap: %d\n", esp_get_free_heap_size());

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

