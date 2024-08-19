#include "i2c.h"

bool i2c_init(i2c_t *i2c) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c->sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = i2c->scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
        .clk_flags = 0
    };
    
    if(i2c_param_config(i2c->port, &conf) != ESP_OK) {
        return false;
    }

    if(i2c_driver_install(i2c->port, conf.mode, 0, 0, 0) != ESP_OK) {
        return false;
    }

    return true;
}

void i2c_deinit(i2c_t *i2c) {
    i2c_driver_delete(i2c->port);
}

bool i2c_write(i2c_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint8_t data, uint32_t timeout_ms) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if(i2c_master_start(cmd) != ESP_OK) {
        return false;
    }
    if(i2c_master_write_byte(cmd, (slave_addr << 1), 1) != ESP_OK) {
        return false;
    }
    if(i2c_master_write_byte(cmd, reg_addr, 1) != ESP_OK) {
        return false;
    }
    if(i2c_master_write_byte(cmd, data, 1) != ESP_OK) {
        return false;
    }
    if(i2c_master_stop(cmd) != ESP_OK) {
        return false;
    }
    int ret = i2c_master_cmd_begin(i2c->port, cmd, pdMS_TO_TICKS(timeout_ms));
    i2c_cmd_link_delete(cmd);

    return ret != ESP_FAIL;
}

bool i2c_read(i2c_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint32_t length, uint32_t timeout_ms) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1), 1);
    i2c_master_write_byte(cmd, reg_addr, 1);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(i2c->port, cmd, pdMS_TO_TICKS(timeout_ms));
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return false;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | 1, 1);
    while(length) {
        i2c_master_read_byte(cmd, data, (length > 0) ? I2C_MASTER_ACK : I2C_MASTER_NACK);
        data++;
        length--;
    }
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c->port, cmd, pdMS_TO_TICKS(timeout_ms));
    i2c_cmd_link_delete(cmd);

    return ret != ESP_FAIL;
}

uint8_t i2c_read_byte(i2c_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint32_t timeout_ms) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1), 1);
    i2c_master_write_byte(cmd, reg_addr, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c->port, cmd, pdMS_TO_TICKS(timeout_ms));
    i2c_cmd_link_delete(cmd);

    uint8_t buffer = 0;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | 1, 1);
    i2c_master_read_byte(cmd, &buffer, (i2c_ack_type_t) 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c->port, cmd, pdMS_TO_TICKS(timeout_ms));
    i2c_cmd_link_delete(cmd);

    return buffer;
}
