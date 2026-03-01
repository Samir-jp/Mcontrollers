#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "analoog.h"

static const char *TAG = "LABO3";

// GPIO pins for LEDs
#define LED_MAIN GPIO_NUM_2           // Main LED - turns on when it's too dark
#define LED_THRESHOLD_PIN GPIO_NUM_4  // LED for threshold indicator
#define LED_TOO_HIGH_PIN GPIO_NUM_15  // LED for light too high

// ADC channels
#define LIGHT_SENSOR_CHANNEL ADC_CHANNEL_0   // Light sensor input

// Fixed threshold settings (no potentiometer)
#define DARK_THRESHOLD 2048
#define THRESHOLD_BAND 150

// Update interval in milliseconds
#define UPDATE_INTERVAL 500

// Global variables
int g_light_value = 0;
int g_threshold_value = DARK_THRESHOLD;

void gpio_setup(void)
{
    // Configure LED pins as outputs
    gpio_set_direction(LED_MAIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_THRESHOLD_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_TOO_HIGH_PIN, GPIO_MODE_OUTPUT);
    
    // Initialize LEDs to off
    gpio_set_level(LED_MAIN, 0);
    gpio_set_level(LED_THRESHOLD_PIN, 0);
    gpio_set_level(LED_TOO_HIGH_PIN, 0);
    
    ESP_LOGI(TAG, "GPIO pins initialized");
}

void adc_setup(void)
{
    // Initialize only the light sensor channel
    myADC_setup(LIGHT_SENSOR_CHANNEL);
    
    ESP_LOGI(TAG, "ADC initialization complete");
}

void monitor_light_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Light monitoring task started");
    
    while (1) {
        // Read light sensor value
        g_light_value = myADC_getValue(LIGHT_SENSOR_CHANNEL);

        if (g_light_value < (g_threshold_value - THRESHOLD_BAND)) {
            // Too dark
            gpio_set_level(LED_MAIN, 1);
            gpio_set_level(LED_THRESHOLD_PIN, 0);
            gpio_set_level(LED_TOO_HIGH_PIN, 0);
        } else if (g_light_value > (g_threshold_value + THRESHOLD_BAND)) {
            // Too bright
            gpio_set_level(LED_MAIN, 0);
            gpio_set_level(LED_THRESHOLD_PIN, 0);
            gpio_set_level(LED_TOO_HIGH_PIN, 1);
        } else {
            // Around threshold
            gpio_set_level(LED_MAIN, 0);
            gpio_set_level(LED_THRESHOLD_PIN, 1);
            gpio_set_level(LED_TOO_HIGH_PIN, 0);
        }
        
        // Log the values for debugging
        ESP_LOGI(TAG, "Light: %d, Threshold: %d", g_light_value, g_threshold_value);
        
        vTaskDelay(UPDATE_INTERVAL / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Application starting...");
    
    // Initialize GPIO and ADC
    gpio_setup();
    adc_setup();
    
    // Create the monitoring task
    xTaskCreate(
        monitor_light_task,           // Task function
        "monitor_light_task",         // Task name
        4096,                         // Stack size
        NULL,                         // Parameter
        1,                            // Priority
        NULL                          // Task handle
    );
    
    ESP_LOGI(TAG, "Application initialized successfully");
}
