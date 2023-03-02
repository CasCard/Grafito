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

static const char* TAG = "VL6180X_TASK";
Adafruit_VL6180X vl = Adafruit_VL6180X();

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

rcl_publisher_t publisher;
std_msgs__msg__UInt8 msg;

extern "C" void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	RCLC_UNUSED(last_call_time);
	if (timer != NULL) {
        // if (! vl.begin()) {
         // ESP_LOGE(TAG, "Failed to find sensor");
         // vTaskDelete(NULL);
         //  }
        // ESP_LOGI(TAG, "Sensor found!");


        float lux = vl.readLux(VL6180X_ALS_GAIN_5);
        ESP_LOGI(TAG, "Lux: %f", lux);
        
        uint8_t range = vl.readRange();
        uint8_t status = vl.readRangeStatus();
        
        if (status == VL6180X_ERROR_NONE) {
            ESP_LOGI(TAG, "Range: %d", range);
        }
        else if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
            ESP_LOGE(TAG, "System error");
        }
        else if (status == VL6180X_ERROR_ECEFAIL) {
            ESP_LOGE(TAG, "ECE failure");
        }
        else if (status == VL6180X_ERROR_NOCONVERGE) {
            ESP_LOGE(TAG, "No convergence");
        }
        else if (status == VL6180X_ERROR_RANGEIGNORE) {
            ESP_LOGE(TAG, "Ignoring range");
        }
        else if (status == VL6180X_ERROR_SNR) {
            ESP_LOGE(TAG, "Signal/Noise error");
        }
        else if (status == VL6180X_ERROR_RAWUFLOW) {
            ESP_LOGE(TAG, "Raw reading underflow");
        }
        else if (status == VL6180X_ERROR_RAWOFLOW) {
            ESP_LOGE(TAG, "Raw reading overflow");
        }
        else if (status == VL6180X_ERROR_RANGEUFLOW) {
            ESP_LOGE(TAG, "Range reading underflow");
        }
        else if (status == VL6180X_ERROR_RANGEOFLOW) {
            ESP_LOGE(TAG, "Range reading overflow");
        }
       

        msg.data = range;
		RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
		
        // vTaskDelay(pdMS_TO_TICKS(1000));
	}}


extern "C" void micro_ros_task(void * arg)
{
    if (! vl.begin()) {
        ESP_LOGE(TAG, "Failed to find sensor");
        vTaskDelete(NULL);
         }
     ESP_LOGI(TAG, "Sensor found!");     

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
	RCCHECK(rclc_node_init_default(&node, "esp32_int32_publisher", "", &support));

	// create publisher
	RCCHECK(rclc_publisher_init_default(
		&publisher,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
		"freertos_int32_publisher"));

	// create timer,
	rcl_timer_t timer;
	const unsigned int timer_timeout = 0;
	RCCHECK(rclc_timer_init_default(
		&timer,
		&support,
		RCL_MS_TO_NS(timer_timeout),
		timer_callback));

	// create executor
	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
	RCCHECK(rclc_executor_add_timer(&executor, &timer));

	msg.data = 0;

	while(1){
		rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
		usleep(10);
	}

	// free resources
	RCCHECK(rcl_publisher_fini(&publisher, &node));
	RCCHECK(rcl_node_fini(&node));

  	vTaskDelete(NULL);
}

extern "C" void app_main()
{ 
#ifdef UCLIENT_PROFILE_UDP
    // Start the networking if required
    ESP_ERROR_CHECK(uros_network_interface_initialize());
 #endif  // UCLIENT_PROFILE_UDP

xTaskCreate(micro_ros_task, 
    "uros_task", 
    5000, 
    NULL,
    1, 
    NULL); 
}

