WatchSense-F407 桌面站原型说明

目标:
- 先做一个稳定的桌面原型，再考虑缩小成手表形态。
- 当前固件作为串口看板、OLED 看板和传感器联动控制器运行。

已实现模块:
- VL53L0X，软件 I2C。
- BME280，SPI1。
- 0.96 寸 OLED，软件 I2C，SSD1306 兼容，默认地址 0x3C。
- MAX30102，软件 I2C，默认地址 0x57，支持手指检测、IR/RED 原始值、心率演示估算和 SpO2 演示估算。
- GY-91 / MPU9250，软件 I2C，默认地址 0x68，AK8963 磁力计透传地址 0x0C。
- LED、BEEP、KEY 交互。

教程:
- 第一次接线和上电请先看 WATCHSENSE_FIRST_RUN_GUIDE.txt。
- 接线表以 WATCHSENSE_WIRING_GUIDE.txt 为准。

接线摘要:
- I2C 总线: PB8=SCL，PB9=SDA，连接 OLED、VL53L0X、MAX30102、GY-91/MPU9250。
- BME280 SPI: PA5=SCK，PA6=MISO，PA7=MOSI，PA4=CSB。
- VL53L0X XSHUT: PB5。
- USART1: PA9=TX，PA10=RX，115200 baud。

注意:
- 原 BME280 资料包用过 PB3/PB4/PB5 做 SPI1。
- 当前整合版不能那样接，因为 PB5 已用于 VL53L0X XSHUT。
- 本工程以 PA5/PA6/PA7/PA4 作为 BME280 SPI 接线。

按键:
- KEY0: 切换串口和 OLED 页面。
- KEY1: 普通页面切换 VL53L0X 测距模式；motion 页面清空运动计数。
- KEY2: 切换距离报警阈值，最后可关闭报警。
- WK_UP: 普通页面做 VL53L0X 100mm 偏移校准；motion 页面刷新 MPU9250 静止校准。

OLED 和串口页面:
- dashboard: 距离、环境、健康摘要和模块状态。
- environment: BME280 温度、湿度和气压。
- distance: VL53L0X 距离、阈值、偏移和测距模式。
- health: MAX30102 手指状态、IR/RED 原始值、心跳计数、HR 演示估算和 SpO2 演示估算。
- motion: MPU9250 姿态、粗略步数、轻敲次数、抬腕次数、磁力计方向和温度。

显示行为:
- OLED 使用 6x8 文本模式，共 8 行，每行约 21 字符。
- 如果 OLED 未连接，固件仍会通过 USART1 继续运行。
- 如果你的 OLED 地址是 0x3D，修改 HARDWARE/OLED/oled.h 里的 OLED_I2C_ADDR。
- OLED、VL53L0X、MAX30102、GY-91/MPU9250 共用 PB8/PB9，请确保 I2C 上拉合适。

联动:
- VL53L0X 距离低于当前阈值时，LED1 点亮并且 BEEP 鸣叫。
- MPU9250 检测到轻敲或抬腕时，在距离报警未激活的情况下触发短促 LED1/BEEP 提示。
- 如果某个传感器缺失或初始化失败，系统会降级运行，不会直接卡死。
- BME280、VL53L0X、MAX30102、MPU9250 未就绪时会每 5 秒重试。

首次校准顺序:
- VL53L0X: 准备 100mm 目标，在非 motion 页面按 WK_UP。
- MPU9250: 切到 motion 页面，让 GY-91 静止平放，再按 WK_UP。
- BME280: 不需要手动校准，上电后等待几十秒，并远离热源。
- MAX30102: 不需要手动校准，重点是调整手指压力和遮光。
- 磁力计: 当前固件只做简化方向判断；如果后续要做精确指南针，再加入 8 字校准。

MAX30102 使用提示:
- 手指轻轻放在传感器窗口上，等待几秒。
- HR 需要几个干净脉搏峰后才会稳定。
- SpO2 是基于 RED/IR AC/DC 比值的演示估算，不是医疗测量。
- 如果 IR 一直很低，检查 3.3V、GND、SCL/SDA、地址 0x57 和 I2C 上拉。

后续建议:
- 上真实硬件后，根据走路、轻敲、抬腕手感微调 motion 阈值。
- 九轴数据稳定后，再继续做更像手表的 OLED 页面。
