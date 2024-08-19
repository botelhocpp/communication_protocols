#include "i2c.h"

#define	MPU6050_DEFAULT_ADDR	0x68

typedef struct PACKED_ATTR {
    int16_t x;
    int16_t y;
    int16_t z;
} axis_raw_t;

typedef struct accel_t {
    uint8_t addr; 
    i2c_t i2c;
} accel_t;

bool accel_init(accel_t *accel, uint32_t timeout_ms);

void accel_deinit(accel_t *accel);

axis_raw_t accel_read_axis_raw(accel_t *accel, uint32_t timeout_ms);
