# WatchSense-F407 原型工程

这是一个基于 STM32F407ZGT6 的桌面原型，目标是先把传感器联调、显示、校准和联动跑稳，后续再缩小成手表形态。

当前固件已经整合:

- VL53L0X 激光测距模块，软件 I2C。
- BME280 温湿度气压模块，SPI1。
- MAX30102 心率/血氧模块，软件 I2C。
- GY-91 / MPU9250 九轴运动模块，软件 I2C。
- 0.96 寸 SSD1306 兼容 OLED，软件 I2C。
- USART1 串口看板、按键、LED、蜂鸣器联动。

## 当前状态

- Keil 工程: `Project/USER/TEST.uvprojx`
- 预编译 HEX: `Project/OBJ/TEST.hex`
- 主程序: `Project/USER/test.c`
- 傻瓜式首次上电教程: `Project/WATCHSENSE_FIRST_RUN_GUIDE.txt`
- 集中接线表: `Project/WATCHSENSE_WIRING_GUIDE.txt`
- 原型说明: `Project/WATCHSENSE_PROTOTYPE_README.txt`
- 参考项目说明: `REFERENCE_PROJECTS.md`

## 第一次使用

第一次接线、上电、校准和排查问题，先看:

`Project/WATCHSENSE_FIRST_RUN_GUIDE.txt`

最短安全顺序:

1. 先接 USB-TTL 和 OLED。
2. 再接 VL53L0X，用 100mm 目标按 `WK_UP` 校准。
3. 再接 MAX30102，放手指确认 IR/RED 数值会变大。
4. 再接 GY-91 / MPU9250，切到 `motion` 页面，保持静止，按 `WK_UP` 校准。
5. 最后接 BME280 到 SPI1，等待温湿度读数稳定。

## 硬件引脚速查

详细接线以 `Project/WATCHSENSE_WIRING_GUIDE.txt` 为准。

| 模块 | 总线 | STM32F407 引脚 |
| --- | --- | --- |
| OLED | I2C | PB8=SCL, PB9=SDA |
| VL53L0X | I2C | PB8=SCL, PB9=SDA, PB5=XSHUT |
| MAX30102 | I2C | PB8=SCL, PB9=SDA |
| GY-91 / MPU9250 | I2C | PB8=SCL, PB9=SDA, AD0=GND 时地址 0x68 |
| BME280 | SPI1 | PA5=SCK, PA6=MISO, PA7=MOSI, PA4=CS |
| USART1 | UART | PA9=TX, PA10=RX, 115200 baud |

所有模块必须共地。本工程按 3.3V 逻辑设计。

## 按键功能

| 按键 | 功能 |
| --- | --- |
| KEY0 / PE4 | 切换 OLED 和串口页面 |
| KEY1 / PE3 | 普通页面切换 VL53L0X 测距模式；motion 页面清空运动计数 |
| KEY2 / PE2 | 切换距离报警阈值或关闭报警 |
| WK_UP / PA0 | 普通页面做 VL53L0X 100mm 校准；motion 页面做 MPU9250 静止校准 |

## 页面

- `dashboard`: 距离、环境、健康摘要和模块状态。
- `environment`: BME280 温度、湿度、气压。
- `distance`: VL53L0X 距离、报警阈值、偏移和测距模式。
- `health`: MAX30102 手指状态、IR/RED 原始值、心跳计数、HR/SpO2 演示估算。
- `motion`: MPU9250 姿态、粗略步数、轻敲/抬腕次数、磁力计方向和温度。

## 编译

使用 Keil uVision 编译:

```powershell
& 'C:\Keil_v5\UV4\UV4.exe' -j0 -b 'Project\USER\TEST.uvprojx'
```

最近一次检查结果为 `0 Error(s), 0 Warning(s)`。

## 可靠性说明

- 传感器初始化采用 fail-soft 策略: 某个模块缺失会显示 `FAIL`，其他模块继续运行。
- BME280、VL53L0X、MAX30102、MPU9250 未就绪时会每 5 秒重试。
- 按键扫描是非阻塞的，长按按键不会卡住 MAX30102 采样。
- MPU9250 每 50ms 采样一次，用于姿态、轻敲、抬腕和粗略步数联动。
- MAX30102 的 HR/SpO2 是项目演示估算，不是医疗测量。

## 九轴说明

GY-91 上电时尽量保持静止。进入 `motion` 页面后，按 `WK_UP` 可重新做陀螺仪零偏校准，按 `KEY1` 可清空 STEP/TAP/LIFT 计数。

STEP、TAP、LIFT 是实用演示算法，适合项目展示和联动验证；如果要做真正手表，还需要结合实测继续调阈值。

## 许可说明

这是一个混合嵌入式原型工程，包含本地项目代码和厂商/参考代码。第三方源码保留其原始声明和许可；在完整审计前，不要假设整个工程只有一个统一开源许可证。
