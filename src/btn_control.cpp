#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include "geometry_msgs/Twist.h"
#include <sstream>

using namespace std;

class Turtle
{
  private:
    ros::NodeHandle n;
    geometry_msgs::Twist twist;
    ros::Timer timer;
    ros::Publisher vel_pub;
    ros::Subscriber joy_sub;

  public:
    float vel = 1;
    double angle, ang_vel;
    bool cw, isForward;
    Turtle()
  {
    vel_pub = n.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1);
    joy_sub = n.subscribe<sensor_msgs::Joy>("joy", 10, &Turtle::joyCallback, this);
  }
  
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
  {
    if(joy->buttons[0]==1)  //forward
    {
      isForward = true;
      timer = n.createTimer(ros::Duration(0.1), &Turtle::callback, this);
    }
    else if(joy->buttons[1]==1) //right
    {
      rotate(1,1.57,true);
    }
    else if(joy->buttons[2]==1) //back
    {
      rotate(1,3.14,true);
    }
    else if(joy->buttons[3]==1) //left
    {
      rotate(1,1.57,false);
    }
    else if(joy->buttons[9]==1) //stop
    {
      isForward = false;
      rotate(0,0,false);
    }
    else if(joy->buttons[6]==1) //- speed
    {
      if(vel > 1){vel--;}
      else{vel = 1;}
    }
    else if(joy->buttons[7]==1) //+ speed
    {
      vel++;
    }
    cout<<vel<<endl;
  }

  void rotate(double ang_vel, double angle, bool cw)
  {
    twist.linear.x = 0;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    if(cw)
    {
      twist.angular.z = ang_vel;
    }
    else
    {
      twist.angular.z = ang_vel;
    }
    
    double t0 = ros::Time::now().toSec();
    double current_ang = 0.0;
    ros::Rate loop_rate(1000);
    do
    {
      vel_pub.publish(twist);
      double t1 = ros::Time::now().toSec();
      current_ang = ang_vel * (t1 - t0);
      ros::spinOnce();
      loop_rate.sleep();
      cout<<current_ang<<endl;
    } while (current_ang < angle);
    twist.angular.z = 0;
  }

  void callback(const ros::TimerEvent &e)
  {
    if(isForward && twist.angular.z == 0)
    {
      twist.linear.x = vel;
    }
    else
    {
      twist.linear.x = 0;
    }

    vel_pub.publish(twist);
    ros::spinOnce();
  }
};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");
  Turtle turtle_obj;
  ros::spin();

  return 0;
}
