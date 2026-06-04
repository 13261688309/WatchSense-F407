# Helpful Reference Projects

These projects are useful references for the next WatchSense-F407 iterations. They are not copied into this repository; check each project's license before importing code.

## MAX30102 / Health Sensing

### NimaGhayour/MAX30102-STM32-HeartRateMonitor

- URL: https://github.com/NimaGhayour/MAX30102-STM32-HeartRateMonitor
- Usefulness: small STM32-focused MAX30102 heart-rate monitor project.
- What to learn from it: signal flow, heart-rate display logic, and simple embedded MAX30102 integration structure.
- Caution: compare algorithm assumptions before replacing the current demo estimator.

### libdriver/max30102

- URL: https://github.com/libdriver/max30102
- Usefulness: dedicated MAX30102 driver library.
- What to learn from it: register coverage, FIFO handling, interrupt configuration, self-test style APIs.
- Caution: adapt only the concepts or clearly licensed driver portions; the current project uses a compact custom driver.

## Watch Firmware Architecture

### ravindsk/STM32_Smart_watch_firmware

- URL: https://github.com/ravindsk/STM32_Smart_watch_firmware
- Usefulness: STM32 smartwatch firmware reference.
- What to learn from it: page layout, firmware structure, power and UI organization.
- Caution: this project may target different STM32 hardware and peripherals.

### No-Chicken/OV-Watch

- URL: https://github.com/No-Chicken/OV-Watch
- Usefulness: more complete open smartwatch firmware direction.
- What to learn from it: application-level watch features, task split, LVGL/FreeRTOS style architecture.
- Caution: much larger than the current bare-metal StdPeriph prototype; use it for architecture inspiration, not direct porting.

## Display / UI

### afiskon/stm32-ssd1306

- URL: https://github.com/afiskon/stm32-ssd1306
- Usefulness: mature SSD1306 OLED library for STM32.
- What to learn from it: framebuffer drawing, fonts, lines, icons, and cleaner UI primitives.
- Caution: framebuffer costs RAM; the current OLED driver is intentionally lightweight text mode.

## Motion / GY-91 Direction

### kriswiner/MPU9250

- URL: https://github.com/kriswiner/MPU9250
- Usefulness: widely referenced MPU9250 calibration and AHRS examples.
- What to learn from it: accelerometer/gyro/magnetometer calibration, Madgwick/Mahony filtering ideas.
- Caution: Arduino-oriented examples need careful STM32 porting.

### mirhamza708/Madgwick-filter-STM32-MPU9250

- URL: https://github.com/mirhamza708/Madgwick-filter-STM32-MPU9250
- Usefulness: STM32-oriented MPU9250 + Madgwick filter reference.
- What to learn from it: attitude estimation loop and filter timing.
- Caution: verify I2C/SPI bus assumptions and CPU load before integrating.

## Suggested Integration Order

1. Keep the current firmware as the stable bare-metal first version.
2. Add GY-91 as a driver-only module first: raw accel/gyro/mag, WHO_AM_I checks, and calibration status.
3. Add simple gestures: lift-to-wake, tap-to-page, rough step count.
4. Only after the sensor pages are stable, consider a richer OLED framebuffer or a small UI task layer.
5. Move toward FreeRTOS/LVGL only if the project outgrows the current scheduler.
