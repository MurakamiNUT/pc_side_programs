

#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include "geometry_msgs/Twist.h"

#include<stdio.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
int getch();

  std_msgs::Int32 msg;
int getch()
{
  static struct termios oldt, newt;
  tcgetattr( STDIN_FILENO, &oldt);           // save old settings
  newt = oldt;
  newt.c_lflag &= ~(ICANON);                 // disable buffering      
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);  // apply new settings

  int c = getchar();  // read character (non-blocking)

  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);  // restore old settings
  return c;
}
int main(int argc, char **argv){
  ros::init(argc, argv, "keyboard");
  ros::NodeHandle n;
  ros::Publisher Controller_pub = n.advertise<std_msgs::Int32>("key", 1000);
  ros::Rate loop_rate(10);
while(ros::ok()){
    int32_t key;
    key = getch();
    msg.data = key;
    ROS_INFO("data:[%d]",msg.data);
    Controller_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
}
}