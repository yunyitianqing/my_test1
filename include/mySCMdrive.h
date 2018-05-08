#ifndef __MY_SCM_DRIVE__
#define __MY_SCM_DRIVE__

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>


#include"mypackage.h"
#include"myserialport.h"



//用c库的time和linux下的pthread

class MySCMDrive
{
private:


    bool flush_flag;

    MyPackage_RecvSCM last_SCM_package;//上一帧接收完的包
    //MyPackage_RecvSCM new_SCM_package;//这一帧正在接收的包
   
    clock_t last_time;//接收完包时的时间
    
    ///暂时使用的是正向查找，也就是找出第一个包，-1表示没找到
    int find_pkg_in_queue(unsigned char*queue,int size);
    
    bool check_msg(MyPackage_RecvSCM &msg);
    
    MySerialPort *m_port;
    
    int data_process();
    
    pthread_t m_pid;    
    static void * process_thread(void *param); //静态成员函数
    pthread_mutex_t mutex;


    double time_ms;
    
    double SpeedLeftOdo;
    double SpeedRightOdo;
    double Yaw;    
    
    double Aacx;
    double Aacy;
    double Aacz;
// 计算获得的二次数据 


    
    double vx;
    double vy;
    
    double dx;
    double dy;

   
    double delta_distance;//积分距离
    double delta_x;//积分坐标x
    double delta_y;//积分坐标y
    
    double x;//当前坐标x,已废弃
    double y;//当前坐标y，已废弃

public:
    MySCMDrive();
    ~MySCMDrive();
    bool init_port(char*port_name);
    
    int run();
    int stop();
    
    int reset();
    
    
    
    
    double get_x();
    double get_y();
    double get_Yaw();
    
    //double get_ax();
    //double get_ay();
    
    void get_ax_ay_az(double&ax,double&ay,double&az);
    
    void get_dx_dy_Yaw(double&dx,double&dy,double&Yaw);
    
    

};











#endif //__MY_SCM_DRIVE__
