#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include<unistd.h>
#include <geometry_msgs/PointStamped.h>
#include<math.h>


//#include"myserialport.h"
#include"mySCMdrive.h"
#include"myIMU.h"



//#include <turtlesim/Pose.h>

#define PI 3.14159265

/*
void poseCallback(const turtlesim::PoseConstPtr& msg){
  static tf::TransformBroadcaster br;
  tf::Transform transform;
  transform.setOrigin( tf::Vector3(msg->x, msg->y, 0.0) );
  tf::Quaternion q;
  q.setRPY(0, 0, msg->theta);
  transform.setRotation(q);
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", turtle_name));
}
*/

int main(int argc, char** argv){
    ros::init(argc, argv, "my_tf_broadcaster");
  
    MySCMDrive scm;

    bool res=scm.init_port("/dev/ttyUSB0");
  
    if(res==false)
    {
        fprintf(stderr,"ttyUSB0 open error\n");
        exit(0);
    }
    
    MyIMU imu;
  
    scm.run();
    
    double ax=0;
    double ay=0;
    double az=0;
    
    double Yaw=0;
    
    double ds_x=0;
    double ds_y=0;
    double ds_z=9;
    
    double x=0;
    double y=0;
    
    
    double ds=0;
    
    sleep(1);

    static tf::TransformBroadcaster br;
  
    //tf::Transform transform;
    //transform.setOrigin( tf::Vector3(dx, dy, 0.0) );
    //tf::Quaternion q;
    //q.setRPY(0, 0, Yaw*PI/180.0);
    //transform.setRotation(q);
    
    //tf::Transform transform_base2laser;
    //transform_base2laser.setOrigin( tf::Vector3(0, 0, 0.0) );
    //tf::Quaternion q2;
    //q2.setRPY(0, 0, 0);
    //transform_base2laser.setRotation(q2);
    
    //初始化imu计算模块
    double a_min_max[6];//xmin,xmax,ymin,ymax,zmin,zmax
    scm.get_ax_ay_az(ax,ay,az);
    
    a_min_max[0]=ax;
    a_min_max[1]=ax;
    a_min_max[2]=ay;
    a_min_max[3]=ay;
    a_min_max[4]=az;
    a_min_max[5]=az;
    
    for(int i=0;i<100;i++)
    {
        scm.get_ax_ay_az(ax,ay,az);
        
        if(a_min_max[0]>ax)//xmin
            a_min_max[0]=ax;
        if(a_min_max[1]<ax)//xmax
            a_min_max[1]=ax;
        if(a_min_max[2]>ay)//ymin
            a_min_max[2]=ay;
        if(a_min_max[3]<ay)//ymax
            a_min_max[3]=ay;
        if(a_min_max[4]>az)//zmin
            a_min_max[4]=az;
        if(a_min_max[5]<az)//zmax
            a_min_max[5]=az;
        
        usleep(100000);
    }
    
    double gx,gy,gz;
    double fx,fy,fz;
    
    gx=(a_min_max[0]+a_min_max[1])/2;
    gy=(a_min_max[2]+a_min_max[3])/2;
    gz=(a_min_max[4]+a_min_max[5])/2;
    imu.set_g(gx,gy,gz);
    
    fx=(a_min_max[1]-a_min_max[0])*1.75;
    fy=(a_min_max[3]-a_min_max[2])*1.75;
    fz=(a_min_max[5]-a_min_max[4])*1.75;
    
    imu.set_filter(fx,fy,fz);
    
    imu.set_stop_timeout(7);
    //～初始化imu计算模块
    
  
    int i=0;
    ros::Rate loop_rate(10);
    while(ros::ok())
    {
        //获取串口数据
        Yaw=scm.get_Yaw();
        scm.get_ax_ay_az(ax,ay,az);
        
        //获取imu数据
        imu.caculate(ax,ay,az);
        imu.get_ds(ds_x,ds_y,ds_z);
        
        ds=sqrt(ds_x*ds_x+ds_y*ds_y);
        
        x+=ds*sin(Yaw*PI/180.0);
        y+=ds*cos(Yaw*PI/180.0);
        
        //tf_publish_once(double x,double y,double Yaw);
  
        tf::Transform transform;
        transform.setOrigin( tf::Vector3(x, y, 0.0) );
        tf::Quaternion q;
        q.setRPY(0, 0, Yaw*PI/180.0);
        transform.setRotation(q);
    
  
        br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "odom", "laser_link"));
        //br.sendTransform(tf::StampedTransform(transform_base2laser, ros::Time::now(), "base_link", "laser_link"));
     
        ROS_INFO("run a time%d", i);
     
        usleep(10000);
        i++;
     
    }




/*    MySerialPort myserialport("/dev/ttyUSB0");
    
    while(1)
    {
        char buf[1024];
        memset(buf,0,1024);  
        int n=myserialport.my_read_some(buf,1024);  
        if(n>0)  
        {  
            cout<<"recv data:"<<buf<<n<<endl;  
        }
        usleep(20000);
    }
*/

/*    MySCMDrive scm;

    scm.init_port("/dev/ttyUSB0");
    scm.run();
    
    double x,y,Yaw;
    

    while(1)
    {
        scm.get_x_y_Yaw(x,y,Yaw);
        std::cout<<x<<","<<y<<","<<Yaw<<std::endl;
        sleep(1);
    }
    
    */
    return 0;
};

