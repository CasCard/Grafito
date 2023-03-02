#include "driver/ledc.h"
#include "esp32-hal-ledc.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include <uros_network_interfaces.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/u_int8.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
#include "uxr/client/config.h"

#define LEDC_LOW_SPEED_MODE LEDC_LOW_SPEED_MODE
#define LEDC_LOW_SPEED_TIMER LEDC_TIMER_0
#define LEDC_LOW_SPEED_FREQ_HZ 5000
#define LEDC_LOW_SPEED_RESOLUTION LEDC_TIMER_10_BIT

#define MOTOR_AOUT (gpio_num_t)13
#define MOTOR_AIN (gpio_num_t)4

void motorTask(void *pvParameters) {

  gpio_pad_select_gpio(MOTOR_AOUT);
  gpio_pad_select_gpio(MOTOR_AIN);
  gpio_set_direction(MOTOR_AOUT, GPIO_MODE_OUTPUT);
  gpio_set_direction(MOTOR_AIN, GPIO_MODE_OUTPUT);

  // Configure low-speed LEDC channel
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_LOW_SPEED_RESOLUTION,
        .timer_num = LEDC_LOW_SPEED_TIMER,
        .freq_hz = LEDC_LOW_SPEED_FREQ_HZ,
    };
    ledc_timer_config(&ledc_timer);

    // Set duty cycle for low-speed LEDC channel
    ledc_channel_config_t ledc_channel0 = {
        .gpio_num = MOTOR_AIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_LOW_SPEED_TIMER,
        .duty = 0,
    };
    ledc_channel_config(&ledc_channel0);

    ledc_channel_config_t ledc_channel1 = {
        .gpio_num = MOTOR_AOUT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_LOW_SPEED_TIMER,
        .duty = 0,
        
    };
    ledc_channel_config(&ledc_channel1);
  while (1) {

    //  if (motorOutput > 0) {
      gpio_set_level(MOTOR_AOUT, 1);
      gpio_set_level(MOTOR_AIN, 0);
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 350);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    // }
    // else if (motorOutput < 0) {
    //   gpio_set_level(MOTOR_AOUT, 0);
    //   gpio_set_level(MOTOR_AIN, 1);
    //   ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (int)-motorOutput);
    //   ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    // }
    // else {
    //   gpio_set_level(MOTOR_AOUT, 0);
    //   gpio_set_level(MOTOR_AIN, 0);
    //   ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    //   ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    // }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
    
//   }
}

void app_main() {

  xTaskCreate(motorTask, "motorTask", 5000, NULL, 5, NULL);
}
