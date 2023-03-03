#include <PID_v1.h>
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
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float32.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
#include "uxr/client/config.h"

#define LEDC_LOW_SPEED_MODE LEDC_LOW_SPEED_MODE
#define LEDC_LOW_SPEED_TIMER LEDC_TIMER_0
#define LEDC_LOW_SPEED_FREQ_HZ 5000
#define LEDC_LOW_SPEED_RESOLUTION LEDC_TIMER_10_BIT

// Define motor pin assignments
#define MOTOR_AOUT (gpio_num_t)18
#define MOTOR_AIN (gpio_num_t)19

// Define encoder pin assignments
#define ENCODER_A (gpio_num_t)35
#define ENCODER_B (gpio_num_t)34

static const char* TAG  = "MOTOR";

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rcl_subscription_t subscriber;
std_msgs__msg__Float32 recv_msg;
double setpoint = 0.0;

// Define PID constants
double Kp = 30;
double Ki = 500;
double Kd = 1.0;

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
    volatile int32_t a = gpio_get_level(ENCODER_A);
    volatile int32_t b = gpio_get_level(ENCODER_B);

    if (a == 1 && b == 0) {
        encoder.pos += encoder.dir;
    } else if (a == 1 && b == 1) {
        encoder.pos -= encoder.dir;
    } else if (a == 0 && b == 1) {
        encoder.pos += encoder.dir;
    } else if (a == 0 && b == 0) {
        encoder.pos -= encoder.dir;
    }}

extern "C" void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	RCLC_UNUSED(last_call_time);
	if (timer != NULL) {
        msg.data = encoder.pos;
		RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
        // vTaskDelay(pdMS_TO_TICKS(1000));
	}}

extern "C" void subscription_callback(const void * msgin)
{
	const std_msgs__msg__Float32 * msg = (const std_msgs__msg__Float32 *)msgin;
    setpoint = msg->data;
    // ESP_LOGI(TAG, "Setpoint: %f", setpoint);
}

// Task to control motor using PID
extern "C" void motorTask(void *pvParameters) {

  while (1) {
    
    // Set motor input to encoder value
    // ESP_LOGI(TAG, "encoderValue: %d", encoder.pos);
    motorSetpoint = setpoint;
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
extern "C" void wifiTask(void *pvParameters) {

	rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
	RCCHECK(rcl_init_options_init(&init_options, allocator));
	rmw_init_options_t* rmw_options = rcl_init_options_get_rmw_init_options(&init_options);

	// Static Agent IP and port can be used instead of autodisvery.
	RCCHECK(rmw_uros_options_set_udp_address(CONFIG_MICRO_ROS_AGENT_IP, CONFIG_MICRO_ROS_AGENT_PORT, rmw_options));
	//RCCHECK(rmw_uros_discover_agent(rmw_options));

	// create init_options
	RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

	// create node
	rcl_node_t node;
	RCCHECK(rclc_node_init_default(&node, "motor_info", "", &support));

	// create publisher
	RCCHECK(rclc_publisher_init_default(
		&publisher,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
		"motor_encoder"));

    // Create subscriber.
	RCCHECK(rclc_subscription_init_default(
		&subscriber,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
		"motor_setpoint"));    

	// create timer,
	rcl_timer_t timer;
	const unsigned int timer_timeout = 10;
	RCCHECK(rclc_timer_init_default(
		&timer,
		&support,
		RCL_MS_TO_NS(timer_timeout),
		timer_callback));

	// create executor
	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
    // unsigned int rcl_wait_timeout = 1000;   // in ms
	// RCCHECK(rclc_executor_set_timeout(&executor, RCL_MS_TO_NS(rcl_wait_timeout)));

	RCCHECK(rclc_executor_add_timer(&executor, &timer));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &recv_msg, &subscription_callback, ON_NEW_DATA));

	msg.data = 0;

	while(1){
		rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
		usleep(1000);
	}

	// free resources
    RCCHECK(rcl_subscription_fini(&subscriber, &node));
	RCCHECK(rcl_publisher_fini(&publisher, &node));
	RCCHECK(rcl_node_fini(&node));

  	vTaskDelete(NULL);
}

extern "C" void app_main() {

  #ifdef UCLIENT_PROFILE_UDP
    // Start the networking if required
    ESP_ERROR_CHECK(uros_network_interface_initialize());
 #endif  // UCLIENT_PROFILE_UDP

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

  
  // Initialize PID objects
  motorPID.SetMode(AUTOMATIC);
  motorPID.SetSampleTime(2);
  motorPID.SetOutputLimits(-1023, 1023);

    gpio_pad_select_gpio(ENCODER_A);
    gpio_pad_select_gpio(ENCODER_B);

    gpio_set_direction(ENCODER_B, GPIO_MODE_INPUT);
    gpio_set_direction(ENCODER_A, GPIO_MODE_INPUT);

    gpio_set_intr_type(ENCODER_A, GPIO_INTR_ANYEDGE);
    
    gpio_set_pull_mode(ENCODER_A, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(ENCODER_B, GPIO_PULLUP_ONLY);
   
    gpio_install_isr_service(0); 

    gpio_isr_handler_add(ENCODER_A, encoder_isr, (void*) ENCODER_A);
    
  xTaskCreate(motorTask, "motorTask", 10000, NULL, 20, NULL);
  xTaskCreate(wifiTask, "wifiTask", 5000, NULL, 0, NULL);
}
