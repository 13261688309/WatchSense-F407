# WatchSense-F407 Prototype

STM32F407ZGT6 desktop prototype for a future watch-style sensor project.

The firmware currently combines:

- VL53L0X time-of-flight distance sensor over software I2C.
- BME280 temperature, humidity, and pressure sensor over SPI1.
- MAX30102 heart-rate / SpO2 module over software I2C.
- 0.96 inch SSD1306-compatible OLED over software I2C.
- USART1 serial dashboard, keys, LEDs, and buzzer linkage.

## Current Status

- Build target: `Project/USER/TEST.uvprojx`
- Prebuilt hex: `Project/OBJ/TEST.hex`
- Main application: `Project/USER/test.c`
- Wiring guide: `Project/WATCHSENSE_WIRING_GUIDE.txt`
- Prototype notes: `Project/WATCHSENSE_PROTOTYPE_README.txt`
- Reference projects: `REFERENCE_PROJECTS.md`

## Hardware Pins

Use `Project/WATCHSENSE_WIRING_GUIDE.txt` as the single wiring source.

Quick summary:

| Module | Bus | STM32F407 pins |
| --- | --- | --- |
| OLED | I2C | PB8=SCL, PB9=SDA |
| VL53L0X | I2C | PB8=SCL, PB9=SDA, PB5=XSHUT |
| MAX30102 | I2C | PB8=SCL, PB9=SDA |
| BME280 | SPI1 | PA5=SCK, PA6=MISO, PA7=MOSI, PA4=CS |
| USART1 | UART | PA9=TX, PA10=RX, 115200 baud |

All modules must share GND. The project is designed for 3.3V logic.

## Controls

| Key | Function |
| --- | --- |
| KEY0 / PE4 | Switch OLED and serial page |
| KEY1 / PE3 | Switch VL53L0X ranging profile |
| KEY2 / PE2 | Cycle distance alarm threshold / turn alarm off |
| WK_UP / PA0 | Calibrate VL53L0X at a 100 mm reference target |

## Pages

- `dashboard`: distance, environment, health summary, and module status.
- `environment`: BME280 temperature, humidity, and pressure.
- `distance`: VL53L0X distance, alarm threshold, offset, and profile.
- `health`: MAX30102 finger status, raw IR/RED, beat count, HR estimate, and SpO2 estimate.
- `motion-todo`: reserved GY-91 page.

## Build

Build with Keil uVision:

```powershell
& 'C:\Keil_v5\UV4\UV4.exe' -j0 -b 'Project\USER\TEST.uvprojx'
```

The latest checked build completed with `0 Error(s), 0 Warning(s)`.

## Reliability Notes

- Sensor initialization is fail-soft: missing modules show `FAIL`, while the rest of the station keeps running.
- BME280, VL53L0X, and MAX30102 are retried every 5 seconds when not ready.
- KEY scanning is non-blocking, so holding a key does not stop MAX30102 sampling.
- MAX30102 HR/SpO2 values are project demo estimates only and are not medical measurements.

## Next Useful Step

The next module to integrate is GY-91, most likely for lift-to-wake, tap switching, posture, and rough step estimation.

## License

This repository is a mixed embedded prototype built from local project code and vendor/reference code. Third-party source files retain their original notices and licenses. Do not assume a single unified open-source license for the whole tree until each imported component has been audited.
