/*
 * Copyright (C) 2008, Morgan Quigley and Willow Garage, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the names of Stanford University or Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// %Tag(FULLTEXT)%
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include "geometry_msgs/Twist.h"
#include "beginner_tutorials/Controller.h"
std::string msg_chatter = "ps2";
/*
float LS_Left_Right_axis;
float LS_Up_Down_axis;
float RS_Left_Right_axis;
float RS_Up_Down_axis;
float Left_Right_cross;
float Up_Down_cross
*/
/*
struct {
  float LS_Left_Right;
  float LS_Up_Down;
  float RS_Left_Right;
  float RS_Up_Down;
  float Left_Right;
  float Up_Down;

  bool Triangle;
  bool Circle;
  bool Square;
  bool Cross;
  bool R1;
  bool R2;
  bool L1;
  bool L2;
  bool Select;
  bool Start;
}Controller;
*/
  beginner_tutorials::Controller controller_;
/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
// %Tag(CALLBACK)%
//std_msgs::String::ConstPtr&
void chatterCallback(const sensor_msgs::Joy& msg)
{
  if(msg_chatter == "ps2"){
    controller_.LS_Left_Right = msg.axes[0];
    controller_.LS_Up_Down = msg.axes[1];
    controller_.RS_Left_Right = msg.axes[2];
    controller_.RS_Up_Down = msg.axes[3];
    controller_.Left_Right = msg.axes[4];
    controller_.Up_Down = msg.axes[5];

    controller_.Triangle = msg.buttons[0];
    controller_.Circle = msg.buttons[1];
    controller_.Cross = msg.buttons[2];
    controller_.Square = msg.buttons[3];
    controller_.L2 = msg.buttons[4];
    controller_.R2 = msg.buttons[5];
    controller_.L1 = msg.buttons[6];
    controller_.R1 = msg.buttons[7];
    controller_.Start = msg.buttons[8];
    controller_.Select = msg.buttons[9];
    controller_.L3 = msg.buttons[10];
    controller_.R3 = msg.buttons[11];
  }
  else if(msg_chatter == "logicool"){
    float l2 = 0;
    float r2 = 0;
    controller_.LS_Left_Right = msg.axes[0];
    controller_.LS_Up_Down = msg.axes[1];
    controller_.RS_Left_Right = msg.axes[3];
    controller_.RS_Up_Down = msg.axes[4];
    controller_.Left_Right = msg.axes[6];
    controller_.Up_Down = msg.axes[7];
    if(msg.axes[2] < 0)controller_.L2 = 1;
    else               controller_.L2 = 0;
    if(msg.axes[5] < 0)controller_.R2 = 1;
    else               controller_.R2 = 0;

    controller_.Triangle = msg.buttons[3];
    controller_.Circle = msg.buttons[1];
    controller_.Cross = msg.buttons[0];
    controller_.Square = msg.buttons[2];
    controller_.L1 = msg.buttons[4];
    controller_.R1 = msg.buttons[5];
    controller_.Start = msg.buttons[7];
    controller_.Select = msg.buttons[6];
    controller_.L3 = msg.buttons[9];
    controller_.R3 = msg.buttons[10];
  }

  //ROS_INFO("===========================================");
  
  for(int i = 0; i <= 5; i++){
    //ROS_INFO("data:[%f]", msg.axes[i]);
  }
  
  for(int j = 0; j <= 9; j++){
   // ROS_INFO("data:[%d]", msg.buttons[j]);
  }
  
}
// %EndTag(CALLBACK)%

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener");
  ros::NodeHandle n;
  ros::NodeHandle pn("~");
  ros::Subscriber sub = n.subscribe("joy", 1000, chatterCallback);
  ros::Publisher Controller_pub = n.advertise<beginner_tutorials::Controller>("Arduino", 1000);
  pn.getParam("cont",msg_chatter);
  ros::Rate loop_rate(10);

#define SPEED_TEST 100
while(ros::ok()){
  Controller_pub.publish(controller_);
  ros::spinOnce();//spinはアイドルループかも?
  loop_rate.sleep();
}
}
// %EndTag(FULLTEXT)%
