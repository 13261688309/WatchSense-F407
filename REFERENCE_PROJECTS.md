# 参考项目说明

这些项目适合作为 WatchSense-F407 后续迭代参考。本仓库没有直接复制这些项目源码；真正移植代码前，请先检查每个项目自己的许可证。

## MAX30102 / 健康检测方向

### NimaGhayour/MAX30102-STM32-HeartRateMonitor

- 地址: https://github.com/NimaGhayour/MAX30102-STM32-HeartRateMonitor
- 参考价值: STM32 上的 MAX30102 心率监测小项目。
- 可以学习: 信号处理流程、心率显示逻辑、MAX30102 嵌入式集成方式。
- 注意事项: 替换当前演示估算算法前，要先对比采样频率、滤波和峰值判断假设。

### libdriver/max30102

- 地址: https://github.com/libdriver/max30102
- 参考价值: 较完整的 MAX30102 驱动库。
- 可以学习: 寄存器覆盖、FIFO 处理、中断配置、自检接口。
- 注意事项: 当前工程使用轻量自写驱动，建议只参考思路或在确认许可证后移植。

## 手表固件架构方向

### ravindsk/STM32_Smart_watch_firmware

- 地址: https://github.com/ravindsk/STM32_Smart_watch_firmware
- 参考价值: STM32 智能手表固件参考。
- 可以学习: 页面组织、固件结构、电源和 UI 管理方式。
- 注意事项: 目标硬件和外设可能与本项目不同，不能直接照搬。

### No-Chicken/OV-Watch

- 地址: https://github.com/No-Chicken/OV-Watch
- 参考价值: 更完整的开源手表固件方向。
- 可以学习: 应用层手表功能、任务划分、LVGL/FreeRTOS 风格架构。
- 注意事项: 规模明显大于当前裸机 StdPeriph 原型，适合作架构参考，不适合直接移植。

## OLED / UI 方向

### afiskon/stm32-ssd1306

- 地址: https://github.com/afiskon/stm32-ssd1306
- 参考价值: 成熟的 STM32 SSD1306 OLED 库。
- 可以学习: framebuffer 绘图、字体、线条、图标和更清晰的 UI 基础能力。
- 注意事项: framebuffer 会占用 RAM；当前工程的 OLED 驱动故意保持轻量文本模式。

## GY-91 / MPU9250 运动算法方向

### kriswiner/MPU9250

- 地址: https://github.com/kriswiner/MPU9250
- 参考价值: 常被引用的 MPU9250 校准和 AHRS 示例。
- 可以学习: 加速度计、陀螺仪、磁力计校准，以及 Madgwick/Mahony 滤波思路。
- 注意事项: 示例偏 Arduino，需要谨慎移植到 STM32。

### mirhamza708/Madgwick-filter-STM32-MPU9250

- 地址: https://github.com/mirhamza708/Madgwick-filter-STM32-MPU9250
- 参考价值: STM32 上 MPU9250 + Madgwick 滤波参考。
- 可以学习: 姿态估计循环和滤波时序。
- 注意事项: 集成前要确认 I2C/SPI 总线假设和 CPU 负载。

## 推荐后续集成顺序

1. 保持当前固件作为稳定裸机第一版。
2. 先在真实硬件上验证 GY-91 原始数据、WHO_AM_I、校准状态。
3. 调整简单手势: 抬腕唤醒、轻敲切页、粗略步数。
4. 传感器页面稳定后，再考虑更丰富的 OLED 图形界面。
5. 只有当当前 20ms 调度器不够用时，再考虑 FreeRTOS/LVGL。
