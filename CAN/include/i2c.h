#ifndef ACCEL_H
#define ACCEL_H

#include <driver/gpio.h>
#include <driver/i2c.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct i2c_t {
    gpio_num_t scl;
    gpio_num_t sda;
    i2c_port_t port;
} i2c_t;

bool i2c_init(i2c_t *i2c);

void i2c_deinit(i2c_t *i2c);

bool i2c_write(i2c_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint8_t data, uint32_t timeout_ms);

bool i2c_read(i2c_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint32_t length, uint32_t timeout_ms);

uint8_t i2c_read_byte(i2c_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint32_t timeout_ms);

#endif
