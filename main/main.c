#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

static const char *TAG = "ADC_TEST";

#define PHOTOTRANSISTOR_ADC_CHANNEL  ADC1_CHANNEL_6  // GPIO34

// Zet op 1 als je later een luidspreker/oortje aansluit op SPEAKER_GPIO.
#define ENABLE_AUDIO_OUTPUT 0
#define SPEAKER_GPIO        25
#define THEREMIN_MIN_FREQ   220
#define THEREMIN_MAX_FREQ   1760

static esp_adc_cal_characteristics_t adc_chars;

void setup_adc(void)
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(PHOTOTRANSISTOR_ADC_CHANNEL, ADC_ATTEN_DB_11);
	
	// Karakteriseer ADC voor betere accuracy
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
	
	ESP_LOGI(TAG, "ADC setup complete - GPIO34 (ADC1_CHANNEL_6)");
	ESP_LOGI(TAG, "ADC Width: 12-bit (0-4095)");
	ESP_LOGI(TAG, "Attenuation: 11dB (meetbereik: 0-2600mV)");
	
	// Test met enkele metingen
	int test_val = adc1_get_raw(PHOTOTRANSISTOR_ADC_CHANNEL);
	ESP_LOGI(TAG, "Initial test reading: %d", test_val);
}

int read_light_level(void)
{
	const int sample_count = 16;
	int sum = 0;

	for (int i = 0; i < sample_count; i++) {
		sum += adc1_get_raw(PHOTOTRANSISTOR_ADC_CHANNEL);
	}

	return sum / sample_count;
}

static int map_light_to_freq(int raw)
{
	if (raw < 0) {
		raw = 0;
	}
	if (raw > 4095) {
		raw = 4095;
	}

	int range = THEREMIN_MAX_FREQ - THEREMIN_MIN_FREQ;
	return THEREMIN_MIN_FREQ + ((raw * range) / 4095);
}

static void setup_audio_output(void)
{
#if ENABLE_AUDIO_OUTPUT
	ledc_timer_config_t ledc_timer = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.duty_resolution = LEDC_TIMER_10_BIT,
		.timer_num = LEDC_TIMER_0,
		.freq_hz = THEREMIN_MIN_FREQ,
		.clk_cfg = LEDC_AUTO_CLK,
	};
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel = {
		.gpio_num = SPEAKER_GPIO,
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = LEDC_CHANNEL_0,
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = LEDC_TIMER_0,
		.duty = 512, // 50% duty voor hoorbare blokgolf
		.hpoint = 0,
	};
	ledc_channel_config(&ledc_channel);
	ESP_LOGI(TAG, "Audio output actief op GPIO%d (PWM)", SPEAKER_GPIO);
#else
	ESP_LOGI(TAG, "Audio output UITGESCHAKELD (geen luidspreker aangesloten)");
#endif
}

static void set_tone_frequency(int freq)
{
#if ENABLE_AUDIO_OUTPUT
	if (freq < THEREMIN_MIN_FREQ) {
		freq = THEREMIN_MIN_FREQ;
	}
	if (freq > THEREMIN_MAX_FREQ) {
		freq = THEREMIN_MAX_FREQ;
	}
	ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
#else
	(void)freq;
#endif
}

void app_main(void)
{
	ESP_LOGI(TAG, "=== ADC Test Starting ===");
	setup_adc();
	setup_audio_output();
	ESP_LOGI(TAG, "Meet nu met GPIO34 (fototransistor)");
	ESP_LOGI(TAG, "Expected: waarde verandert met licht");
	ESP_LOGI(TAG, "Theremin: licht -> frequentie (%d-%d Hz)", THEREMIN_MIN_FREQ, THEREMIN_MAX_FREQ);
	ESP_LOGI(TAG, "=========================");
    
	while(1) {
		int light_raw = read_light_level();
		// Converteer naar spanning met calibratie
		uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(light_raw, &adc_chars);
		int tone_freq = map_light_to_freq(light_raw);
		set_tone_frequency(tone_freq);
		
		ESP_LOGI(TAG, "ADC: %4d (0x%03X) | %4u mV | %4d Hz | %s", 
			light_raw,
			light_raw,
			voltage_mv,
			tone_freq,
			light_raw == 0 ? "** POSSIBLY NOT CONNECTED **" : "OK");
		
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

