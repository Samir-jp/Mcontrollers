#include <stdio.h>
#include "myGPIO.h"         //de eigen functie definitie's
#include "driver/gpio.h"    //omdat ik de GPIO gebruik         
#include "esp_attr.h"       //omdat ik een ISR in RAM wil steken

//voor de leds zijn de meeste functie's vanzelfsprekend 
//maar verken zeker eens de verschillende drive modes

void myGPIO_LED_Setup(int pin)
{
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    /*
    gpio_config_t myconfig = {
        .intr_type = 
    };
    gpio_config(&myconfig);
    */
}
void myGPIO_LED_SetValue(int pin, int value)
{
    gpio_set_level(pin, value);
}
void myGPIO_LED_On(int pin)
{
    gpio_set_level(pin, 1);
}
void myGPIO_LED_Off(int pin)
{
    gpio_set_level(pin, 0);
}
void myGPIO_LED_Toggle(int pin)
{
    gpio_set_level(pin, !gpio_get_level(pin));
}

void myGPIO_KNOP_Setup(int pin)
{
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_pullup_en(pin);
}


#ifndef KNOP_ISR

static void IRAM_ATTR KNOP_ISR(void* arg)
{
    //hier moet je dus de actie bij interrupt schrijven maar houd ze kort
}

#endif

void myGPIO_KNOP_SetupInterrupt(int pin)
{
    gpio_set_intr_type(pin, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(pin, KNOP_ISR, 0);

}
int myGPIO_KNOP_GetValue(int pin)
{
    return gpio_get_level(pin);

}
int myGPIO_KNOP_GetFlank(int pin)
{
    static uint64_t vroeger = ~0, nu = ~0;

    //____update 1 pin
    vroeger = nu;
    nu = (nu & ~(1<<pin)) | (gpio_get_level(pin) << pin);

    if((vroeger & (1<<pin)) > (nu & (1<<pin)))
    {
        return 1;
    }

    return 0;



}