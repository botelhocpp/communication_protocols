#include <string.h>

#include "driver/gpio.h"
#include "driver/twai.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c.h"
#include "accel.h"

#define DEBUG_USE (1)

#if (DEBUG_USE == 1)
#define xLog(...) printf(__VA_ARGS__)
#else
#define xLog(...) ;
#endif

#define TWAI_RX_PIN             (GPIO_NUM_13)
#define TWAI_TX_PIN             (GPIO_NUM_14)
#define ACCEL_MSG_ID            (121)
#define DISPLAY_MSG_ID          (120)

#define I2C_PORT                (I2C_NUM_0)
#define I2C_TIMEOUT             (1000)
#define I2C_SDA_PIN             (GPIO_NUM_21)
#define I2C_SCL_PIN             (GPIO_NUM_22)
#define	MPU6050_ADDR	        (0x68)

void vAccelTask(void *pvParameters) {
    twai_message_t message = {
        .identifier = ACCEL_MSG_ID,
        .extd = 0,
        .self = 0,
        .data_length_code = sizeof(axis_raw_t),
        .rtr = 0
    };

    accel_t accel = {
        .addr = MPU6050_ADDR,
        .i2c = {
            .port = I2C_PORT,
            .scl = I2C_SCL_PIN,
            .sda = I2C_SDA_PIN,
        }
    };

    i2c_init(&accel.i2c);

    while(1) {
        if(accel_init(&accel, I2C_TIMEOUT)) {
            axis_raw_t axis = accel_read_axis_raw(&accel, I2C_TIMEOUT);

            xLog("x = %d y = %d z = %d\n", axis.x, axis.y, axis.z);

            memcpy(message.data, &axis, sizeof(axis_raw_t));

            if (twai_transmit(&message, pdMS_TO_TICKS(1000)) != ESP_OK) {
                xLog("Failed to queue message for transmission\n");
                continue;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vDisplayTask(void *pvParameters) {
    twai_message_t message;

    while(1) {
        if (twai_receive(&message, pdMS_TO_TICKS(1000)) != ESP_OK) {
            xLog("Failed to receive message\n");
            continue;
        }

        // if (!message.rtr && message.identifier == DISPLAY_MSG_ID) {}
        xLog("[%03lX] ", message.identifier);

        for(int i = 0; i < message.data_length_code; i++) {
            xLog("%02X ", message.data[i]);
        }
        xLog("\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

BaseType_t xTwaiSetup(void) {
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TWAI_TX_PIN, TWAI_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        xLog("Failed to install driver\n");
        return pdFALSE;
    }

    if (twai_start() != ESP_OK) {
        xLog("Failed to start driver\n");
        return pdFALSE;
    }

    return pdTRUE;
}

void app_main() {
    while(!xTwaiSetup());

    xTaskCreate(
        vAccelTask,
        "Accelerometer Task", 
        10*configMINIMAL_STACK_SIZE, 
        NULL, 
        1, 
        NULL
    );

    xTaskCreate(
        vDisplayTask, 
        "Display Task", 
        10*configMINIMAL_STACK_SIZE, 
        NULL, 
        1, 
        NULL
    );
}
