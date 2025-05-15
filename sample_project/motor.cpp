#include <PID_v1.h>
#include "driver/ledc.h"
#include "esp32-hal-ledc.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Adafruit_VL6180X.h"
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

// #define MOTOR_PWM_FREQ 100
// #define MOTOR_PWM_TIMER LEDC_TIMER_0
// #define MOTOR_PWM_CHANNEL LEDC_CHANNEL_0

// Define motor pin assignments
#define MOTOR_AOUT (gpio_num_t)18
#define MOTOR_AIN (gpio_num_t)19

// Define encoder pin assignments
#define ENCODER_A (gpio_num_t)35
#define ENCODER_B (gpio_num_t)34

static const char* TAG  = "MOTOR";
// Define PID constants
double Kp = 30;
double Ki = 3000;
double Kd = 0.5;

// Define motor control variables
double motorInput, motorOutput, motorSetpoint; 

// Define PID objects
PID motorPID(&motorInput, &motorOutput, &motorSetpoint, Kp, Ki, Kd, DIRECT);

typedef struct {
    int16_t pos;
    int8_t dir;
} Encoder;
Encoder encoder = {0, 1};

void encoder_isr(void *arg) {
    volatile uint32_t a = gpio_get_level(ENCODER_A);
    volatile uint32_t b = gpio_get_level(ENCODER_B);

    if (a == 1 && b == 0) {
        encoder.pos += encoder.dir;
    } else if (a == 1 && b == 1) {
        encoder.pos -= encoder.dir;
    } else if (a == 0 && b == 1) {
        encoder.pos += encoder.dir;
    } else if (a == 0 && b == 0) {
        encoder.pos -= encoder.dir;
    }}
// Task to control motor using PID
extern "C" void motorTask(void *pvParameters) {

  // ledcSetup(LEDC_CHANNEL_0, MOTOR_PWM_FREQ, 10);
  // ledcAttachPin(MOTOR_AIN, LEDC_CHANNEL_0);
  // ledcSetup(LEDC_CHANNEL_1, MOTOR_PWM_FREQ, 10);
  // ledcAttachPin(MOTOR_AOUT, LEDC_CHANNEL_1);
  // Initialize motor pins
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
  
  
  // Set initial motor setpoint
  motorSetpoint = 1000.0;
  
  // Initialize PID objects
  motorPID.SetMode(AUTOMATIC);
  motorPID.SetSampleTime(2);
  motorPID.SetOutputLimits(-1023, 1023);

  while (1) {
    
    // Set motor input to encoder value
    ESP_LOGI(TAG, "encoderValue: %d", encoder.pos);
    motorInput = encoder.pos;
    
    // Compute PID output
    motorPID.Compute();
    ESP_LOGI(TAG, "Motor output: %f", motorOutput);
    // Set motor speed based on PID output
    if (motorOutput > 400) {
      gpio_set_level(MOTOR_AOUT, 1);
      gpio_set_level(MOTOR_AIN, 0);
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, (int)motorOutput);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    }
    else if (motorOutput < -400) {
      gpio_set_level(MOTOR_AOUT, 0);
      gpio_set_level(MOTOR_AIN, 1);
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (int)-motorOutput);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
    else {
      gpio_set_level(MOTOR_AOUT, 0);
      gpio_set_level(MOTOR_AIN, 0);
      ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

extern "C" void app_main() {

  
    gpio_pad_select_gpio(ENCODER_A);
    gpio_pad_select_gpio(ENCODER_B);

    gpio_set_direction(ENCODER_B, GPIO_MODE_INPUT);
    gpio_set_direction(ENCODER_A, GPIO_MODE_INPUT);

    gpio_set_intr_type(ENCODER_A, GPIO_INTR_ANYEDGE);
    
    gpio_set_pull_mode(ENCODER_A, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(ENCODER_B, GPIO_PULLUP_ONLY);
   
    gpio_install_isr_service(0); 

    gpio_isr_handler_add(ENCODER_A, encoder_isr, (void*) ENCODER_A);
  xTaskCreate(motorTask, "motorTask", 5000, NULL, 5, NULL);
}

