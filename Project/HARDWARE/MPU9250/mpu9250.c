#include "mpu9250.h"
#include "I2C.h"
#include "delay.h"

#define MPU9250_REG_SMPLRT_DIV       0x19U
#define MPU9250_REG_CONFIG           0x1AU
#define MPU9250_REG_GYRO_CONFIG      0x1BU
#define MPU9250_REG_ACCEL_CONFIG     0x1CU
#define MPU9250_REG_ACCEL_CONFIG2    0x1DU
#define MPU9250_REG_INT_PIN_CFG      0x37U
#define MPU9250_REG_ACCEL_XOUT_H     0x3BU
#define MPU9250_REG_USER_CTRL        0x6AU
#define MPU9250_REG_PWR_MGMT_1       0x6BU
#define MPU9250_REG_PWR_MGMT_2       0x6CU
#define MPU9250_REG_WHO_AM_I         0x75U

#define AK8963_REG_WIA               0x00U
#define AK8963_REG_ST1               0x02U
#define AK8963_REG_HXL               0x03U
#define AK8963_REG_ST2               0x09U
#define AK8963_REG_CNTL1             0x0AU
#define AK8963_REG_ASAX              0x10U

#define AK8963_MODE_POWER_DOWN       0x00U
#define AK8963_MODE_FUSE_ROM         0x0FU
#define AK8963_MODE_CONTINUOUS_100HZ 0x16U

static u8 g_i2c_addr = MPU9250_I2C_ADDR_LOW;
static MPU9250_Config g_config;
static u16 g_mag_adjust[3] = {256U, 256U, 256U};
static u8 g_mag_ready = 0U;

static s16 MPU9250_S16_BE(const u8 *buf)
{
    return (s16)(((u16)buf[0] << 8) | buf[1]);
}

static s16 MPU9250_S16_LE(const u8 *buf)
{
    return (s16)(((u16)buf[1] << 8) | buf[0]);
}

static u8 MPU9250_ReadReg(u8 reg, u8 *data)
{
    return I2C_ReadReg(g_i2c_addr, reg, data);
}

static u8 MPU9250_ReadRegs(u8 reg, u8 *buf, u8 len)
{
    return I2C_ReadMulti(g_i2c_addr, reg, buf, len);
}

static u8 MPU9250_WriteReg(u8 reg, u8 data)
{
    return I2C_WriteReg(g_i2c_addr, reg, data);
}

static u8 AK8963_ReadReg(u8 reg, u8 *data)
{
    return I2C_ReadReg(AK8963_I2C_ADDR, reg, data);
}

static u8 AK8963_ReadRegs(u8 reg, u8 *buf, u8 len)
{
    return I2C_ReadMulti(AK8963_I2C_ADDR, reg, buf, len);
}

static u8 AK8963_WriteReg(u8 reg, u8 data)
{
    return I2C_WriteReg(AK8963_I2C_ADDR, reg, data);
}

static u8 MPU9250_CheckConfig(const MPU9250_Config *config)
{
    if (config == 0)
    {
        return MPU9250_ERR_PARAM;
    }

    if ((config->i2c_addr != MPU9250_I2C_ADDR_LOW) &&
        (config->i2c_addr != MPU9250_I2C_ADDR_HIGH))
    {
        return MPU9250_ERR_PARAM;
    }

    if ((config->accel_range > MPU9250_ACCEL_RANGE_16G) ||
        (config->gyro_range > MPU9250_GYRO_RANGE_2000DPS) ||
        (config->gyro_dlpf_cfg > 7U) ||
        (config->accel_dlpf_cfg > 7U))
    {
        return MPU9250_ERR_PARAM;
    }

    return MPU9250_OK;
}

static u8 MPU9250_SetConfig(const MPU9250_Config *config)
{
    u8 result;

    result = MPU9250_CheckConfig(config);
    if (result != MPU9250_OK)
    {
        return result;
    }

    g_i2c_addr = config->i2c_addr;
    g_config = *config;

    if (MPU9250_WriteReg(MPU9250_REG_PWR_MGMT_1, 0x01U))
    {
        return MPU9250_ERR_I2C;
    }
    if (MPU9250_WriteReg(MPU9250_REG_PWR_MGMT_2, 0x00U))
    {
        return MPU9250_ERR_I2C;
    }
    if (MPU9250_WriteReg(MPU9250_REG_CONFIG, config->gyro_dlpf_cfg & 0x07U))
    {
        return MPU9250_ERR_I2C;
    }
    if (MPU9250_WriteReg(MPU9250_REG_SMPLRT_DIV, config->sample_rate_div))
    {
        return MPU9250_ERR_I2C;
    }
    if (MPU9250_WriteReg(MPU9250_REG_GYRO_CONFIG, (u8)config->gyro_range << 3))
    {
        return MPU9250_ERR_I2C;
    }
    if (MPU9250_WriteReg(MPU9250_REG_ACCEL_CONFIG, (u8)config->accel_range << 3))
    {
        return MPU9250_ERR_I2C;
    }
    if (MPU9250_WriteReg(MPU9250_REG_ACCEL_CONFIG2, config->accel_dlpf_cfg & 0x07U))
    {
        return MPU9250_ERR_I2C;
    }

    if (MPU9250_WriteReg(MPU9250_REG_USER_CTRL, 0x00U))
    {
        return MPU9250_ERR_I2C;
    }

    if (MPU9250_WriteReg(MPU9250_REG_INT_PIN_CFG, 0x02U))
    {
        return MPU9250_ERR_I2C;
    }

    delay_ms(10);
    return MPU9250_OK;
}

static u8 AK8963_Init(void)
{
    u8 chip_id;
    u8 asa[3];

    if (AK8963_ReadReg(AK8963_REG_WIA, &chip_id))
    {
        return MPU9250_ERR_I2C;
    }

    if (chip_id != AK8963_CHIP_ID_VALUE)
    {
        return MPU9250_ERR_MAG_ID;
    }

    if (AK8963_WriteReg(AK8963_REG_CNTL1, AK8963_MODE_POWER_DOWN))
    {
        return MPU9250_ERR_I2C;
    }
    delay_ms(10);

    if (AK8963_WriteReg(AK8963_REG_CNTL1, AK8963_MODE_FUSE_ROM))
    {
        return MPU9250_ERR_I2C;
    }
    delay_ms(10);

    if (AK8963_ReadRegs(AK8963_REG_ASAX, asa, sizeof(asa)))
    {
        return MPU9250_ERR_I2C;
    }

    g_mag_adjust[0] = (u16)asa[0] + 128U;
    g_mag_adjust[1] = (u16)asa[1] + 128U;
    g_mag_adjust[2] = (u16)asa[2] + 128U;

    if (AK8963_WriteReg(AK8963_REG_CNTL1, AK8963_MODE_POWER_DOWN))
    {
        return MPU9250_ERR_I2C;
    }
    delay_ms(10);

    if (AK8963_WriteReg(AK8963_REG_CNTL1, AK8963_MODE_CONTINUOUS_100HZ))
    {
        return MPU9250_ERR_I2C;
    }
    delay_ms(10);

    return MPU9250_OK;
}

static u8 AK8963_ReadRawMag(s16 mag[3])
{
    u8 status;
    u8 buf[7];
    u8 retry;

    for (retry = 0; retry < 10U; retry++)
    {
        if (AK8963_ReadReg(AK8963_REG_ST1, &status))
        {
            return MPU9250_ERR_I2C;
        }

        if (status & 0x01U)
        {
            break;
        }

        delay_ms(1);
    }

    if ((status & 0x01U) == 0U)
    {
        return MPU9250_ERR_MAG;
    }

    if (AK8963_ReadRegs(AK8963_REG_HXL, buf, sizeof(buf)))
    {
        return MPU9250_ERR_I2C;
    }

    if (buf[6] & 0x08U)
    {
        return MPU9250_ERR_MAG;
    }

    mag[0] = MPU9250_S16_LE(&buf[0]);
    mag[1] = MPU9250_S16_LE(&buf[2]);
    mag[2] = MPU9250_S16_LE(&buf[4]);

    return MPU9250_OK;
}

static s32 MPU9250_GetAccelSensitivity(void)
{
    static const s32 sensitivity[4] = {16384L, 8192L, 4096L, 2048L};

    return sensitivity[g_config.accel_range];
}

static s32 MPU9250_GetGyroSensitivityX10(void)
{
    static const s32 sensitivity_x10[4] = {1310L, 655L, 328L, 164L};

    return sensitivity_x10[g_config.gyro_range];
}

void MPU9250_GetDefaultConfig(MPU9250_Config *config)
{
    if (config == 0)
    {
        return;
    }

    config->i2c_addr = MPU9250_I2C_ADDR_LOW;
    config->accel_range = MPU9250_ACCEL_RANGE_2G;
    config->gyro_range = MPU9250_GYRO_RANGE_250DPS;
    config->sample_rate_div = 9U;
    config->gyro_dlpf_cfg = 3U;
    config->accel_dlpf_cfg = 3U;
}

u8 MPU9250_ReadID(u8 *chip_id)
{
    if (chip_id == 0)
    {
        return MPU9250_ERR_PARAM;
    }

    if (MPU9250_ReadReg(MPU9250_REG_WHO_AM_I, chip_id))
    {
        return MPU9250_ERR_I2C;
    }

    return MPU9250_OK;
}

u8 MPU9250_ReadMagID(u8 *chip_id)
{
    if (chip_id == 0)
    {
        return MPU9250_ERR_PARAM;
    }

    if (AK8963_ReadReg(AK8963_REG_WIA, chip_id))
    {
        return MPU9250_ERR_I2C;
    }

    return MPU9250_OK;
}

u8 MPU9250_MagIsReady(void)
{
    return g_mag_ready;
}

u8 MPU9250_Init(const MPU9250_Config *config)
{
    MPU9250_Config default_config;
    u8 chip_id;
    u8 result;

    I2C_BusRecover();
    delay_ms(2);

    if (config == 0)
    {
        MPU9250_GetDefaultConfig(&default_config);
        config = &default_config;
    }

    result = MPU9250_CheckConfig(config);
    if (result != MPU9250_OK)
    {
        return result;
    }

    g_i2c_addr = config->i2c_addr;

    if (MPU9250_WriteReg(MPU9250_REG_PWR_MGMT_1, 0x80U))
    {
        return MPU9250_ERR_I2C;
    }
    delay_ms(100);

    result = MPU9250_ReadID(&chip_id);
    if (result != MPU9250_OK)
    {
        return result;
    }

    if ((chip_id != MPU9250_COMPAT_CHIP_ID) &&
        (chip_id != MPU6500_CHIP_ID_VALUE) &&
        (chip_id != MPU9250_CHIP_ID_VALUE) &&
        (chip_id != MPU9255_CHIP_ID_VALUE))
    {
        return MPU9250_ERR_ID;
    }

    result = MPU9250_SetConfig(config);
    if (result != MPU9250_OK)
    {
        return result;
    }

    result = AK8963_Init();
    g_mag_ready = (result == MPU9250_OK) ? 1U : 0U;

    return MPU9250_OK;
}

u8 MPU9250_ReadRaw(MPU9250_RawData *raw)
{
    u8 buf[14];
    u8 result;

    if (raw == 0)
    {
        return MPU9250_ERR_PARAM;
    }

    if (MPU9250_ReadRegs(MPU9250_REG_ACCEL_XOUT_H, buf, sizeof(buf)))
    {
        return MPU9250_ERR_I2C;
    }

    raw->accel[0] = MPU9250_S16_BE(&buf[0]);
    raw->accel[1] = MPU9250_S16_BE(&buf[2]);
    raw->accel[2] = MPU9250_S16_BE(&buf[4]);
    raw->temperature = MPU9250_S16_BE(&buf[6]);
    raw->gyro[0] = MPU9250_S16_BE(&buf[8]);
    raw->gyro[1] = MPU9250_S16_BE(&buf[10]);
    raw->gyro[2] = MPU9250_S16_BE(&buf[12]);

    if (g_mag_ready)
    {
        result = AK8963_ReadRawMag(raw->mag);
        if (result != MPU9250_OK)
        {
            g_mag_ready = 0U;
            raw->mag[0] = 0;
            raw->mag[1] = 0;
            raw->mag[2] = 0;
        }
    }
    else
    {
        raw->mag[0] = 0;
        raw->mag[1] = 0;
        raw->mag[2] = 0;
    }

    return MPU9250_OK;
}

u8 MPU9250_ReadData(MPU9250_Data *data)
{
    MPU9250_RawData raw;
    s32 accel_sensitivity;
    s32 gyro_sensitivity_x10;
    u8 axis;
    u8 result;

    if (data == 0)
    {
        return MPU9250_ERR_PARAM;
    }

    result = MPU9250_ReadRaw(&raw);
    if (result != MPU9250_OK)
    {
        return result;
    }

    accel_sensitivity = MPU9250_GetAccelSensitivity();
    gyro_sensitivity_x10 = MPU9250_GetGyroSensitivityX10();

    for (axis = 0; axis < 3U; axis++)
    {
        data->accel_mg[axis] = ((s32)raw.accel[axis] * 1000L) / accel_sensitivity;
        data->gyro_dps_x100[axis] = ((s32)raw.gyro[axis] * 1000L) / gyro_sensitivity_x10;
        data->mag_ut_x100[axis] = ((s32)raw.mag[axis] * 15L * (s32)g_mag_adjust[axis]) / 256L;
    }

    data->temperature_x100 = (((s32)raw.temperature * 10000L) / 33387L) + 2100L;

    return MPU9250_OK;
}
