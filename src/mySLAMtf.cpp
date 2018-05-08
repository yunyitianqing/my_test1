#include"mySLAMtf.h"


MySLAMTf::MySLAMTf(ros::NodeHandle node))
{
    m_pid=0;
    m_node=node;
    
    pthread_mutex_init(&mutex,0);
}


int MySLAMTf::listener_run()
{

    if(pthread_create(&m_pid,NULL,process_thread,this) != 0) 
    {    
        return -1; 
    }    
    return 0;


}

int MySLAMTf::tf_publish_once(double x,double y,double Yaw)
{
    
    //if(flush_flag==1)
    
    
    transform_odom2laser_link.setOrigin( tf::Vector3(x, y, 0.0) );
    tf::Quaternion q;
    q.setRPY(0, 0, Yaw*PI/180.0);
    transform_odom2laser_link.setRotation(q);
    
        
        
    tf::Transform transform;
        
    transform.mult(this->transform_map2odom,this->transform_odom2laser_link);
        
    this->transform_odom2laser_link=transform;

    br.sendTransform(tf::StampedTransform(transform_odom2laser_link, ros::Time::now(), "odom", "laser_link"));
    
    
    
    
    
    
}



void * MySLAMTf::process_thread(void *param)
{
    MySLAMTf*This=(MySLAMTf*)param;
    
    tf::TransformListener listener;
    tf::StampedTransform transform1;
    tf::StampedTransform transform2;
    tf::Transform *p_transform=0;
    
    while (m_node.ok())
    {
        try{
          listener.lookupTransform("/map", "/odom",
                                   ros::Time(0), transform1);
                                   
          listener.lookupTransform("/map", "/laser_link",
                                   ros::Time(0), transform2);
          
          //tf::Quaternion q1=transform1.get;
          //tf::Quaternion q2;
          ROS_INFO("map2odom:[%lf,%lf]", transform1.getOrigin().x,transform1.getOrigin().y);
          ROS_INFO("map2laser_link:[%lf,%lf]",transform2.getOrigin().x,transform2.getOrigin().y);                         
          
        }
        catch (tf::TransformException &ex) {
          ROS_ERROR("%s",ex.what());
          ros::Duration(1.0).sleep();
          continue;
        }
        
        p_transform=&transform1;
        This->transform_map2odom=*p_transform;
        
        p_transform=&transform2;
        This->transform_map2laser_link=*p_transform;
        
        usleep(100000);
    
    
    }
}
