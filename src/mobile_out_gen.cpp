#include <ros/ros.h>
#include <beginner_tutorials/Controller.h>
#include <beginner_tutorials/Motor.h>
#include "std_msgs/Int32.h"
/*
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include <std_msgs/Int32MultiArray.h>
*/
beginner_tutorials::Motor msg;


bool CLAWLER_MODE = true;
bool emerge = false;
#define SPEED_TEST 14000//5710//1500
#define SPEED_TEST_F 7200
#define RISE_TIME 0.5//最大速度までの到達時間
#define UPDATE_TIME 100//ms Timerの更新周期

#define FLIP_FRONT_RIGHT 0
#define FLIP_FRONT_LEFT 1
#define FLIP_REAR_RIGHT 2
#define FLIP_REAR_LEFT 3
#define CRAWLER_RIGHT 4
#define CRAWLER_LEFT 5

#define MOTOR_NUM 6

#define FLIP_FRONT_RIGHT_ID 6//R
#define FLIP_FRONT_LEFT_ID 7//R
#define FLIP_REAR_RIGHT_ID 2//R
#define FLIP_REAR_LEFT_ID 3//R
#define CRAWLER_RIGHT_ID 4//FR/FL
#define CRAWLER_LEFT_ID 5//FL/FR

struct FLIP{
    bool Front_Right;
    bool Front_Left;
    bool Rear_Right;
    bool Rear_Left;
};
struct FLIP flip;

int32_t key;
int32_t motor_speed[MOTOR_NUM] = {0};
//現在値
float Right_C_Vel;
float Left_C_Vel;
//目標値
float Right_C_Vel_T;
float Left_C_Vel_T;

void messageCb( const beginner_tutorials::Controller& controller_) {
    
 // ROS_INFO("data:[%d]", 1);
  if(controller_.L3 == true) CLAWLER_MODE = true;
  if(controller_.R3 == true) CLAWLER_MODE = false;
  if(!emerge){
    if(CLAWLER_MODE){
      //クローラー動作記述部
        float radian = atan2(controller_.LS_Up_Down, -controller_.LS_Left_Right);
        float value = sqrt((controller_.LS_Up_Down*controller_.LS_Up_Down) + (-controller_.LS_Left_Right*(-controller_.LS_Left_Right)) );
        float deg; 
        //不感帯
        if((fabs(controller_.LS_Up_Down) > 0.01) || (fabs(controller_.LS_Left_Right) > 0.01)){
          Right_C_Vel_T = sin(radian - (3.141592/4)) * SPEED_TEST * value * 1.4142;
          Left_C_Vel_T  = sin(radian + (3.141592/4)) * SPEED_TEST * value * 1.4142;
        }
        else{
          Right_C_Vel_T = 0;
          Left_C_Vel_T = 0;
        }
        //過剰値のカット
        if(SPEED_TEST < fabs(Right_C_Vel_T)){
          if(Right_C_Vel_T > 0)Right_C_Vel_T = SPEED_TEST;
          else Right_C_Vel_T = -SPEED_TEST;
        }  
        if(SPEED_TEST < fabs(Left_C_Vel_T)){
          if(Left_C_Vel_T > 0) Left_C_Vel_T = SPEED_TEST;
          else Left_C_Vel_T = -SPEED_TEST;
        }
        deg = radian * 180/3.141592;
        if(deg < 0)deg = 180 + (180 + deg);
        //高速低速モード対応
        int Vel_mode = 1;
        if(controller_.Circle)Vel_mode = 2;
        if(controller_.Square)Vel_mode = 0;
        if(Vel_mode == 0){
          Right_C_Vel_T *= 0.3;
          Left_C_Vel_T *= 0.3;
        }
        else if(Vel_mode == 1){
          Right_C_Vel_T *= 0.6;
          Left_C_Vel_T *= 0.6;
        }
        else{}
        //三角加速
        if(fabs(Right_C_Vel_T) < fabs(Right_C_Vel)) Right_C_Vel = Right_C_Vel_T;
        else if (fabs(Right_C_Vel_T) > fabs(Right_C_Vel)){
          if(Right_C_Vel_T >= 0) Right_C_Vel += SPEED_TEST / (RISE_TIME/(UPDATE_TIME/1000.0));
          else                   Right_C_Vel -= SPEED_TEST / (RISE_TIME/(UPDATE_TIME/1000.0));
          if(fabs(Right_C_Vel_T) < fabs(Right_C_Vel)) Right_C_Vel = Right_C_Vel_T;
        }
        else{}
        
        if(fabs(Left_C_Vel_T) < fabs(Left_C_Vel)) Left_C_Vel = Left_C_Vel_T;
        else if (fabs(Left_C_Vel_T) > fabs(Left_C_Vel)){
          if(Left_C_Vel_T >= 0)  Left_C_Vel += SPEED_TEST / (RISE_TIME/(UPDATE_TIME/1000.0));
          else                   Left_C_Vel -= SPEED_TEST / (RISE_TIME/(UPDATE_TIME/1000.0));
          if(fabs(Left_C_Vel_T) < fabs(Left_C_Vel))   Left_C_Vel = Left_C_Vel_T;
        }
        else{}
        ////////////////////////////////////////////////////////////////////////
        motor_speed[CRAWLER_LEFT] = (int)(Left_C_Vel_T);
        motor_speed[CRAWLER_RIGHT]= (int)(-Right_C_Vel_T);
        //-----------------------------------------------------------------------
        //動作フリッパー選択
        if(controller_.R1 == true)flip.Front_Right = true;
        else{
          flip.Front_Right = false;
          motor_speed[FLIP_FRONT_RIGHT]=0;
        }
        if(controller_.L1 == true)flip.Front_Left = true;
        else{
          flip.Front_Left = false;
          motor_speed[FLIP_FRONT_LEFT]=0;
        }
        if(controller_.R2 == true)flip.Rear_Right = true;
        else{
          flip.Rear_Right = false;
          motor_speed[FLIP_REAR_RIGHT]=0;
        }
        if(controller_.L2 == true)flip.Rear_Left = true;
        else{
          flip.Rear_Left = false;
          motor_speed[FLIP_REAR_LEFT]=0;
        }
      
        if(controller_.Cross){//フリッパーを上げる
          if(flip.Front_Right == true){
            motor_speed[FLIP_FRONT_RIGHT]+= SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_FRONT_RIGHT]) > SPEED_TEST_F)  motor_speed[FLIP_FRONT_RIGHT] = SPEED_TEST_F;
          }
          if(flip.Front_Left  == true){
            motor_speed[FLIP_FRONT_LEFT] += SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_FRONT_LEFT])  > SPEED_TEST_F)  motor_speed[FLIP_FRONT_LEFT]  = SPEED_TEST_F;
          }
          if(flip.Rear_Right  == true){
            motor_speed[FLIP_REAR_RIGHT] += SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_REAR_RIGHT])  > SPEED_TEST_F)  motor_speed[FLIP_REAR_RIGHT]  = SPEED_TEST_F;
          }
          if(flip.Rear_Left   == true){
            motor_speed[FLIP_REAR_LEFT]  += SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_REAR_LEFT])   > SPEED_TEST_F)  motor_speed[FLIP_REAR_LEFT]   = SPEED_TEST_F;
          }
        }
        
        else if(controller_.Triangle){//下げる
          if(flip.Front_Right == true){
            motor_speed[FLIP_FRONT_RIGHT]-= SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_FRONT_RIGHT]) > SPEED_TEST_F)  motor_speed[FLIP_FRONT_RIGHT] = -SPEED_TEST_F;
          }
          if(flip.Front_Left  == true){
            motor_speed[FLIP_FRONT_LEFT] -= SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_FRONT_LEFT])  > SPEED_TEST_F)  motor_speed[FLIP_FRONT_LEFT]  = -SPEED_TEST_F;
          }
          if(flip.Rear_Right  == true){
            motor_speed[FLIP_REAR_RIGHT] -= SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_REAR_RIGHT])  > SPEED_TEST_F)  motor_speed[FLIP_REAR_RIGHT]  = -SPEED_TEST_F;
          }
          if(flip.Rear_Left   == true){
            motor_speed[FLIP_REAR_LEFT]  -= SPEED_TEST_F / (RISE_TIME/(UPDATE_TIME/1000.0));
            if(fabs(motor_speed[FLIP_REAR_LEFT])   > SPEED_TEST_F)  motor_speed[FLIP_REAR_LEFT]   = -SPEED_TEST_F;
          }
        }
        else{
          motor_speed[FLIP_FRONT_RIGHT]  = 0;
          motor_speed[FLIP_FRONT_LEFT]   = 0;
          motor_speed[FLIP_REAR_RIGHT]   = 0;
          motor_speed[FLIP_REAR_LEFT]    = 0;
        }
    }
    else{
      motor_speed[CRAWLER_RIGHT]     = 0;
      motor_speed[CRAWLER_LEFT]      = 0;
      motor_speed[FLIP_FRONT_RIGHT]  = 0;
      motor_speed[FLIP_FRONT_LEFT]   = 0;
      motor_speed[FLIP_REAR_RIGHT]   = 0;
      motor_speed[FLIP_REAR_LEFT]    = 0;
    }
  }
  else{
    motor_speed[CRAWLER_RIGHT]     = 0;
    motor_speed[CRAWLER_LEFT]      = 0;
    motor_speed[FLIP_FRONT_RIGHT]  = 0;
    motor_speed[FLIP_FRONT_LEFT]   = 0;
    motor_speed[FLIP_REAR_RIGHT]   = 0;
    motor_speed[FLIP_REAR_LEFT]    = 0;
  }
  
    for(int idx=0; idx<MOTOR_NUM; idx ++)msg.motor_speed[idx] = motor_speed[idx];
  //msg.data.push_back(motor_speed);
}

void chatterCallback_key(const std_msgs::Int32& msg){
  key = msg.data;
  ROS_INFO("data:[%d]", key);
  const char *log;
  switch(key){
    case 111:
      emerge = true;
    break;
    case 112:
      emerge = false;
    break;
  }
}
int main(int argc, char **argv){
    ros::init(argc, argv, "arduino");    
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("Arduino", 1000, messageCb);
    ros::Subscriber sub_key = nh.subscribe("key", 1000, chatterCallback_key);
    ros::Publisher pub = nh.advertise<beginner_tutorials::Motor>("Motor_Con", 1000);
    ros::Rate loop_rate(10);
    //msg.data.clear();

    while(ros::ok()){
        pub.publish(msg);
        ros::spinOnce();//spinはアイドルループかも?
        loop_rate.sleep();
    }
}