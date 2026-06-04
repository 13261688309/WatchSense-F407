WatchSense-F407 desktop station prototype

Goal:
- Build the first stable prototype before shrinking it into a watch form.
- Current firmware runs as a serial dashboard and linkage controller.

Implemented modules:
- VL53L0X over software I2C.
- BME280 over SPI1.
- 0.96 inch OLED over software I2C, SSD1306-compatible, default address 0x3C.
- MAX30102 over software I2C, default address 0x57, with finger detection, raw IR/RED, demo heart-rate estimate, and demo SpO2 estimate.
- LED/BEEP/KEY interaction.

Reserved module slots:
- GY-91 over I2C: planned for posture, tap, lift-up, and rough step estimation.

Wiring:
- Use WATCHSENSE_WIRING_GUIDE.txt as the single authoritative wiring guide.
- I2C bus: PB8=SCL, PB9=SDA for OLED, VL53L0X, MAX30102, and later GY-91.
- BME280 SPI: PA5=SCK, PA6=MISO, PA7=MOSI, PA4=CSB.
- VL53L0X XSHUT: PB5.
- USART1: PA9=TX, PA10=RX, 115200 baud.

Note:
- The original BME280 package used PB3/PB4/PB5 for SPI1.
- This merged prototype uses PA5/PA6/PA7 instead, because PB5 is already used as VL53L0X XSHUT.

Controls:
- KEY0: switch serial dashboard page.
- KEY1: switch VL53L0X profile: default, speed, accuracy, long.
- KEY2: cycle distance alarm threshold, then alarm off.
- WK_UP: calibrate VL53L0X offset at a 100 mm reference target.

OLED and serial dashboard pages:
- dashboard: distance, environment, health summary, and module status.
- environment: BME280 temperature, humidity, and pressure.
- distance: VL53L0X distance, threshold, offset, and profile.
- health: MAX30102 finger status, raw IR/RED, beat count, HR estimate, and SpO2 estimate.
- motion-todo: GY-91 integration placeholder.

Display behavior:
- OLED uses 6x8 text mode, 8 lines, 21 characters per line.
- If OLED is not connected, the firmware keeps running through USART1.
- If your OLED address is 0x3D, change OLED_I2C_ADDR in HARDWARE/OLED/oled.h.
- OLED, VL53L0X, and MAX30102 share PB8/PB9; keep pull-ups on the I2C bus.

Linkage:
- If VL53L0X distance is below the active threshold, LED1 turns on and BEEP sounds.
- If a sensor is missing or fails to initialize, the station continues running in degraded mode.
- Missing BME280/VL53L0X/MAX30102 modules are retried every 5 seconds.

MAX30102 use notes:
- Put a fingertip lightly on the sensor window and wait a few seconds.
- HR needs several clean pulse peaks before stabilizing.
- SpO2 is a demo estimate based on RED/IR AC/DC ratio, not a medical measurement.
- If IR stays low, check 3.3V, GND, SCL/SDA, address 0x57, and I2C pull-ups.

Next steps:
- Add GY-91 driver and map lift-up/tap events to page switching.
- Add richer OLED pages after GY-91 data is stable.
