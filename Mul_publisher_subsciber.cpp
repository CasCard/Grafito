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
#include <std_msgs/msg/int32_multi_array.h>
#include <std_msgs/msg/float32.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
#include "uxr/client/config.h"

static const char* TAG  = "MOTOR";

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

rcl_publisher_t publisher;
std_msgs__msg__Int32MultiArray msg;

rcl_publisher_t publisher2;
std_msgs__msg__Float32 msg2;

int32_t data[2] = {0, 0}; 

rcl_subscription_t subscriber;
std_msgs__msg__Float32 recv_msg;

int NewSetpoint=0;

extern "C" void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	RCLC_UNUSED(last_call_time);
	if (timer != NULL) {
       msg.data.data[0]= 4;
       msg.data.data[1]= 5;
		RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
		
        // vTaskDelay(pdMS_TO_TICKS(1000));
	}}

extern "C" void timer_callback2(rcl_timer_t * timer2, int64_t last_call_time2)
{
	RCLC_UNUSED(last_call_time2);
	if (timer2 != NULL) {
       msg2.data = 0.005;
		RCSOFTCHECK(rcl_publish(&publisher2, &msg2, NULL));
		
        // vTaskDelay(pdMS_TO_TICKS(1000));
	}}

extern "C" void subscription_callback(const void * msgin)
{
	const std_msgs__msg__Float32 * msg = (const std_msgs__msg__Float32 *)msgin;
    NewSetpoint = (int) msg->data;
    ESP_LOGI(TAG, "NewSetpoint: %d", NewSetpoint);
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
	rcl_node_t node ;
	RCCHECK(rclc_node_init_default(&node, "motor_info", "", &support));

	// create publisher
	RCCHECK(rclc_publisher_init_default(
		&publisher,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
		"motor_encoder"));

	// create publisher
	RCCHECK(rclc_publisher_init_default(
		&publisher2,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
		"new_pub"));    

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

	rcl_timer_t timer2;
	const unsigned int timer_timeout2 = 10;
	RCCHECK(rclc_timer_init_default(
		&timer2,
		&support,
		RCL_MS_TO_NS(timer_timeout2),
		timer_callback2));    


	// create executor
	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 3, &allocator));
    // unsigned int rcl_wait_timeout = 1000;   // in ms
	// RCCHECK(rclc_executor_set_timeout(&executor, RCL_MS_TO_NS(rcl_wait_timeout)));

	RCCHECK(rclc_executor_add_timer(&executor, &timer));
  	RCCHECK(rclc_executor_add_timer(&executor, &timer2));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &recv_msg, &subscription_callback, ON_NEW_DATA));

	msg.data.size = 2;
    msg.data.data = data;	
    msg2.data = 0;

	while(1){
		rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
		usleep(1000);
	}

	// free resources
  RCCHECK(rcl_subscription_fini(&subscriber, &node));
	RCCHECK(rcl_publisher_fini(&publisher, &node));
  RCCHECK(rcl_publisher_fini(&publisher2, &node));
	RCCHECK(rcl_node_fini(&node));

  	vTaskDelete(NULL);
}

extern "C" void app_main() {

  #ifdef UCLIENT_PROFILE_UDP
    // Start the networking if required
    ESP_ERROR_CHECK(uros_network_interface_initialize());
 #endif  // UCLIENT_PROFILE_UDP
    
  xTaskCreate(wifiTask, "wifiTask", 5000, NULL, 1, NULL);
}
