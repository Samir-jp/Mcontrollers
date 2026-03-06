# Lichtsensor ADC Project (LABO3)

## Beschrijving
Dit ESP32-project meet de lichtintensiteit met een lichtsensor en stuurt drie LEDs aan op basis van een vaste drempelwaarde.

- **Lichtsensor (ADC1_CH0)**: Meet hoe donker het is
- **Vaste drempel**: 2048 (midden van 0-4095 bereik)

De drie LEDs geven de volgende status weer:
- **Te laag**: Lichtsensor onder drempel → te donker
- **Drempel**: Referentie-indicator
- **Te hoog**: Lichtsensor boven drempel → voldoende licht

## Hardware Setup

### GPIO Pinnen
- **LED_MAIN (GPIO2)**: Hoofdlamp - gaat aan wanneer het te donker is
- **LED_THRESHOLD_PIN (GPIO4)**: Drempel indicator LED
- **LED_TOO_HIGH_PIN (GPIO15)**: Indicator voor voldoende licht

### ADC Pinnen
- **LIGHT_SENSOR_CHANNEL (ADC1_CH0)**: Lichtsensor input

## Functionaliteit

1. **ADC Inlezing**: Leest lichtsensor elke 500ms
2. **Drempel Vergelijking**: Vergelijkt lichtsensorwaarde met vaste drempel (2048)
3. **LED Besturing**: 
   - LED_MAIN (GPIO2): AAN als lichtsensor < drempelwaarde (te donker)
   - LED_THRESHOLD_PIN (GPIO4): Altijd AAN als referentie
   - LED_TOO_HIGH_PIN (GPIO15): AAN als lichtsensor >= drempelwaarde (voldoende licht)
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
- 3x LEDs met weerstanden

## Auteur
Samir - Labo3 Project
