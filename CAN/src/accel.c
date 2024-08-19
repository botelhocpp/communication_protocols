#include "accel.h"

#define	SMPLRT_DIV		0x19	
#define	CONFIG			0x1A	
#define	GYRO_CONFIG		0x1B	
#define	ACCEL_CONFIG	0x1C	
#define	ACCEL_XOUT_H	0x3B	
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B
#define	WHO_AM_I		0x75

bool accel_init(accel_t *accel, uint32_t timeout_ms) {
    if (!i2c_write(&accel->i2c, accel->addr, PWR_MGMT_1, 0x00, timeout_ms)) {
        return false;
    }
    if (!i2c_write(&accel->i2c, accel->addr, CONFIG, 0x07, timeout_ms)) {
        return false;
    }
    if (!i2c_write(&accel->i2c, accel->addr, GYRO_CONFIG, 0x00, timeout_ms)) {
        return false;
    }
    if (!i2c_write(&accel->i2c, accel->addr, ACCEL_CONFIG, 0x01, timeout_ms)) {
        return false;
    }
    return true;
}

void accel_deinit(accel_t *accel) {
    i2c_deinit(&accel->i2c);
}

axis_raw_t accel_read_axis_raw(accel_t *accel, uint32_t timeout_ms) {
    uint8_t data[6];
    i2c_read(&accel->i2c, accel->addr, ACCEL_XOUT_H, data, 6, timeout_ms);

    axis_raw_t axis = {
        .x = (data[0] << 8) | data[1],
        .y = (data[2] << 8) | data[3],
        .z = (data[4] << 8) | data[5]
    };

    return axis;
}
