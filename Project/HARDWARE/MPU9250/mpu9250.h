#ifndef __MPU9250_H
#define __MPU9250_H

#include "sys.h"

#define MPU9250_I2C_ADDR_LOW       0x68U
#define MPU9250_I2C_ADDR_HIGH      0x69U
#define AK8963_I2C_ADDR            0x0CU

#define MPU9250_CHIP_ID_VALUE      0x71U
#define MPU9255_CHIP_ID_VALUE      0x73U
#define MPU6500_CHIP_ID_VALUE      0x70U
#define MPU9250_COMPAT_CHIP_ID     0x60U
#define AK8963_CHIP_ID_VALUE       0x48U

#define MPU9250_OK                 0U
#define MPU9250_ERR_I2C            1U
#define MPU9250_ERR_ID             2U
#define MPU9250_ERR_PARAM          3U
#define MPU9250_ERR_MAG_ID         4U
#define MPU9250_ERR_MAG            5U

typedef enum
{
    MPU9250_ACCEL_RANGE_2G = 0,
    MPU9250_ACCEL_RANGE_4G = 1,
    MPU9250_ACCEL_RANGE_8G = 2,
    MPU9250_ACCEL_RANGE_16G = 3
} MPU9250_AccelRange;

typedef enum
{
    MPU9250_GYRO_RANGE_250DPS = 0,
    MPU9250_GYRO_RANGE_500DPS = 1,
    MPU9250_GYRO_RANGE_1000DPS = 2,
    MPU9250_GYRO_RANGE_2000DPS = 3
} MPU9250_GyroRange;

typedef struct
{
    u8 i2c_addr;
    MPU9250_AccelRange accel_range;
    MPU9250_GyroRange gyro_range;
    u8 sample_rate_div;
    u8 gyro_dlpf_cfg;
    u8 accel_dlpf_cfg;
} MPU9250_Config;

typedef struct
{
    s16 accel[3];
    s16 gyro[3];
    s16 mag[3];
    s16 temperature;
} MPU9250_RawData;

typedef struct
{
    s32 accel_mg[3];
    s32 gyro_dps_x100[3];
    s32 mag_ut_x100[3];
    s32 temperature_x100;
} MPU9250_Data;

void MPU9250_GetDefaultConfig(MPU9250_Config *config);
u8 MPU9250_Init(const MPU9250_Config *config);
u8 MPU9250_ReadID(u8 *chip_id);
u8 MPU9250_ReadMagID(u8 *chip_id);
u8 MPU9250_MagIsReady(void);
u8 MPU9250_ReadRaw(MPU9250_RawData *raw);
u8 MPU9250_ReadData(MPU9250_Data *data);

#endif
