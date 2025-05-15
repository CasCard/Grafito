#include <micro_ros_arduino.h>
#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/u_int8.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

#define LED_PIN 13

rcl_publisher_t publisher;
std_msgs__msg__UInt8 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void setup() {
  // put your setup code here, to run once:
  set_microros_transports();
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,HIGH);

  allocator = rcl_get_default_allocator();
  
  Serial.println("Adafruit VL6180x test!");
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("Sensor found!");

  // create init_options
  RCCHECK(rclc_support_init(&support,0,NULL,&allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node,"tof_node","",&support));

  // create publisher

  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs,msg,UInt8),
    "tof_topic"
  ));
  
}

void loop() {
  // put your main code here, to run repeatedly:
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);

  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) {
    msg.data = range;
    RCSOFTCHECK(rcl_publish(&publisher,&msg,NULL));
  }
  delay(50);
  }
