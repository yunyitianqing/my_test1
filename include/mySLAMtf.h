#ifndef __MY_SLAM_TF__
#define __MY_SLAM_TF__

#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include<pthread>

class MySLAMTf
{
    tf::Transform transform_map2odom;
    tf::Transform transform_odom2laser_link;
    
    //方案2
    tf::Transform transform_map2laser_link
    
    //--------------------------------
    
    //int get_odom2laser_link();
    
    pthread_t m_pid;
    tf::TransformBroadcaster br;    
    static void * TfListener_thread(void *param); //静态成员函数
    pthread_mutex_t mutex;
    
    ros::NodeHandle m_node;
    
public:

    MySLAMTf(ros::NodeHandle node);

    int listener_run();
    
    int tf_publish_once(double x,double y,double Yaw);
    
    //int set_x_y_yaw(double x,double y double yaw);


};


#endif//__MY_SLAM_TF__

