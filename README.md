# WatchSense-F407

WatchSense-F407 is an STM32F407ZGT6 desktop prototype for a future watch-style sensor project. The first goal is to make the sensors, display, calibration flow, and linkage behavior stable before shrinking the hardware into a wearable form.

## What It Includes

- VL53L0X time-of-flight distance sensor over software I2C.
- BME280 temperature, humidity, and pressure sensor over SPI1.
- MAX30102 heart-rate / SpO2 module over software I2C.
- GY-91 / MPU9250 9-axis motion module over software I2C.
- 0.96 inch SSD1306-compatible OLED over software I2C.
- USART1 serial dashboard, keys, LEDs, and buzzer linkage.

## Download

Use this package for the complete Keil project:

- `WatchSense-F407-Prototype.zip`

The ZIP contains the full `Project/` tree, Keil project file, source code, drivers, documentation, and prebuilt HEX file.

## Repository Guide

- First-run tutorial: `Project/WATCHSENSE_FIRST_RUN_GUIDE.txt`
- Wiring guide: `Project/WATCHSENSE_WIRING_GUIDE.txt`
- Prototype notes: `Project/WATCHSENSE_PROTOTYPE_README.txt`
- Reference notes: `REFERENCE_PROJECTS.md`

## Quick First Run

1. Connect USB-TTL and OLED first.
2. Add VL53L0X and calibrate it at 100 mm with `WK_UP`.
3. Add MAX30102 and check that IR/RED values change when a finger is placed on the sensor.
4. Add GY-91 / MPU9250, switch to the `motion` page, keep the module still, and press `WK_UP`.
5. Add BME280 on SPI1 and wait for environmental readings to stabilize.

## Pin Summary

| Module | Bus | STM32F407 pins |
| --- | --- | --- |
| OLED | I2C | PB8=SCL, PB9=SDA |
| VL53L0X | I2C | PB8=SCL, PB9=SDA, PB5=XSHUT |
| MAX30102 | I2C | PB8=SCL, PB9=SDA |
| GY-91 / MPU9250 | I2C | PB8=SCL, PB9=SDA, AD0=GND for address 0x68 |
| BME280 | SPI1 | PA5=SCK, PA6=MISO, PA7=MOSI, PA4=CS |
| USART1 | UART | PA9=TX, PA10=RX, 115200 baud |

All modules must share GND. The project is designed for 3.3 V logic.

## Controls

| Key | Function |
| --- | --- |
| KEY0 / PE4 | Switch OLED and serial dashboard page |
| KEY1 / PE3 | Switch VL53L0X profile; reset motion counters on the motion page |
| KEY2 / PE2 | Cycle distance alarm threshold or turn the alarm off |
| WK_UP / PA0 | Calibrate VL53L0X at 100 mm; calibrate MPU9250 gyro bias on the motion page |

## Pages

- `dashboard`: distance, environment, health summary, and module status.
- `environment`: BME280 temperature, humidity, and pressure.
- `distance`: VL53L0X distance, alarm threshold, offset, and profile.
- `health`: MAX30102 finger status, raw IR/RED, beat count, HR estimate, and SpO2 estimate.
- `motion`: MPU9250 posture, rough step count, tap/lift count, magnetometer heading, and temperature.

## Build

Build with Keil uVision:

```powershell
& 'C:\Keil_v5\UV4\UV4.exe' -j0 -b 'Project\USER\TEST.uvprojx'
```

The latest checked local build completed with `0 Error(s), 0 Warning(s)`.

## Notes

- Sensor initialization is fail-soft: missing modules show `FAIL`, while the rest of the station keeps running.
- BME280, VL53L0X, MAX30102, and MPU9250 are retried every 5 seconds when not ready.
- MPU9250 is sampled every 50 ms for posture, tap, lift-up, and rough step linkage.
- MAX30102 HR/SpO2 values are demo estimates only and are not medical measurements.
