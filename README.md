# Lichtsensor ADC Project (LABO3)

## Beschrijving
Dit ESP32-project meet twee analoge waarden:
- **Lichtsensor (ADC0)**: Meet hoe donker het is
- **Potentiometer (ADC3)**: Stelt de drempelwaarde in voor de lichtsensor

Wanneer de lichtsensorwaarde onder de drempelwaarde daalt (te donker), gaat de LED aan.

## Hardware Setup

### GPIO Pinnen
- **LED_MAIN (GPIO2)**: Hoofdlamp - gaat aan wanneer het te donker is
- **LED_THRESHOLD_PIN (GPIO4)**: Drempel indicator LED
- **LED_TOO_HIGH_PIN (GPIO15)**: Over-limiet indicator LED

### ADC Pinnen
- **LIGHT_SENSOR_CHANNEL (ADC1_CH0)**: Lichtsensor input
- **POT_THRESHOLD_CHANNEL (ADC1_CH3)**: Potentiometer threshold control

## Functionaliteit

1. **ADC Inlezing**: Leest beide analoge kanalen elke 500ms
2. **Drempel Vergelijking**: Vergelijkt lichtsensorwaarde met potentiometer waarde
3. **LED Besturing**: 
   - LED AAN als lichtsensor < drempelwaarde
   - LED UIT als lichtsensor >= drempelwaarde
4. **Logging**: Print waarden naar console voor debugging

## Code Structuur

### main.c
- `gpio_setup()`: Initialiseert GPIO pins als outputs
- `adc_setup()`: Initialiseert ADC kanalen
- `monitor_light_task()`: FreeRTOS task voor continue monitoring
- `app_main()`: Hoofd applicatie

### components/analoog/
De ADC component bevat helper functies:
- `myADC_setup(channel)`: Setup ADC kanaal
- `myADC_getValue(channel)`: Leest raw ADC waarde
- `myADC_getMiliVolt(channel)`: Leest ADC waarde in mV

## Compilatie en Flashing

```bash
# Build project
idf.py build

# Flash naar ESP32
idf.py flash

# Monitor serial output
idf.py monitor

# Build, Flash en Monitor samen
idf.py build flash monitor
```

## Requirements
- ESP-IDF v5.0 of hoger
- ESP32 microcontroller
- Lichtsensor (analoog)
- Potentiometer (analoog)
- LEDs met weerstanden

## Auteur
Labo3 Project
