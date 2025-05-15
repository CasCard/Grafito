#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <nvs_flash.h>

#include "VL6180X.h"

#include "esp_log.h"
#include "esp_system.h"

#include <uros_network_interfaces.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/u_int16.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
#include "uxr/client/config.h"

// #ifdef CONFIG_MICRO_ROS_ESP_XRCE_DDS_MIDDLEWARE
// #include <rmw_microros/rmw_microros.h>
// #endif

/* config */
#define I2C_PORT 0
#define GPIO_NUM_21 21
#define GPIO_NUM_22 22
#define FREQ_HZ 400000

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}


#include "app_log.h"


rcl_publisher_t publisher;
std_msgs__msg__UInt16 msg;

// create a timer call back to read the sensor data and publish it

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {

    std_msgs__msg__UInt16__init(&msg);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (1) {
        RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    } else {
        msg.data = 0;
        printf("Failed to measure :(\n");
    }
  }
  // add delay to avoid overflow
  
}



void micro_ros_task(void *arg)
{

  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;

  
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
	RCCHECK(rcl_init_options_init(&init_options, allocator));
  rmw_init_options_t* rmw_options = rcl_init_options_get_rmw_init_options(&init_options);
  RCCHECK(rmw_uros_options_set_udp_address(CONFIG_MICRO_ROS_AGENT_IP, CONFIG_MICRO_ROS_AGENT_PORT, rmw_options));

	// create init_options
	RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

  rcl_node_t node;
  // create node "sensors" with a topic for "tof_sensor_1"
  RCCHECK(rclc_node_init_default(&node, "sensors", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt16),
    "tof_sensor_1"));

  // create timer;
  rcl_timer_t timer;
  const unsigned int timer_timeout = 1;

  /* initialization */

  // send data to callback function
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create executor
  rclc_executor_t executor;
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));


  while (1) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
    // usleep(100);
  }

  // free resources
  RCCHECK(rcl_publisher_fini(&publisher, &node));
  RCCHECK(rcl_node_fini(&node));

  vTaskDelete(NULL);

}

void publish_sensor_data(void * vl) {
  // create a timer call back to read the sensor data and publish it
  while (1) {
    /* Measurement */
    std_msgs__msg__UInt16__init(&msg);
    uint16_t result_mm = 0;
    TickType_t tick_start = xTaskGetTickCount();
    bool res = VL6180X_read(vl, &result_mm);
    TickType_t tick_end = xTaskGetTickCount();
    int took_ms = pdTICKS_TO_MS((int)tick_end - tick_start);
    if (1) {
      msg.data = result_mm;
    } else {
      printf("Failed to measure :(\n");
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
  
}
  
  



void app_main() {

  /* NVS flash initialization */
  nvs_flash_init();

  
#ifdef UCLIENT_PROFILE_UDP
    // Start the networking if required
    ESP_ERROR_CHECK(uros_network_interface_initialize());
#endif  // UCLIENT_PROFILE_UDP

  VL6180X vl;
  vl.i2c_port = I2C_PORT;
  VL6180X_i2cMasterInit(&vl, GPIO_NUM_21, GPIO_NUM_22, FREQ_HZ);
  if (!VL6180X_init(&vl)) {
    loge;
    printf("Failed to initialize VL6180X\n");
    vTaskDelay(portMAX_DELAY);
  }else{
    printf("Sensor Initialised");
  }

  
  xTaskCreate(
    publish_sensor_data,
    "sensor_publisher_task",
    16000,
    (void *) &vl,
    1,
    NULL
  );
  
  xTaskCreate(
    micro_ros_task, 
    "micro_ros_task", 
    16000,
    NULL,
    1,
    NULL
    );
  
}

  
  


  /* Main Loop */
  /* Main Loop */
  // while (1) {
  //   /* Measurement */
  //   uint16_t result_mm = 0;
  //   TickType_t tick_start = xTaskGetTickCount();
  //   bool res = VL6180X_read(&vl, &result_mm);
  //   TickType_t tick_end = xTaskGetTickCount();
  //   int took_ms = pdTICKS_TO_MS((int)tick_end - tick_start);
  //   if (res) {
  //     logi;
  //     printf("Range: %d [mm] took %d [ms]\n", result_mm, took_ms);
  //   } else {
  //     loge;
  //     printf("Failed to measure :(\n");
  //   }
  // }

  // vTaskDelay(portMAX_DELAY);

