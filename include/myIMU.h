#ifndef __MY_IMU__
#define __MY_IMU__

//#include<time.h>
#include<unistd.h>
#include<sys/time.h>

class MyIMU
{
    //所有的[3]都是x,y,z的格式。
    double filter[3];

    

    double a[3];
    double g[3];
    
    double v[3];//不算z轴  积分算速度
    
    double ds[3];//不算z轴 通过速度对一个循环的时间进行积分，来求一个循环的位移量
    
    //double global_v[3];
    double global_s[3];
    
    double yaw;
    
    // init=-1
    // no_global=0
    // caculate_global=1;
    int flag_caculate;
    
    int stop_timeout;
    int cur_stop_timeout;
    
    struct  timeval start;
    struct  timeval end;
public:
    MyIMU();
    
    void set_g(double gx,double gy,double gz);
    void clear_v();
    void reset_global_s(double x=0,double y=0,double z=0);
    void set_filter(double ax,double ay,double az);
    
    void set_stop_timeout(int times=10);
    
    //迭代计算v，然后通过v计算ds
    int caculate(double ax,double ay,double az);
    
    //迭代计算v，然后通过v计算ds,最后通过yaw计算global_s
    int caculate(double ax,double ay,double az,double yaw);
    
    
    int get_a(double &ax,double &ay,double &az);
    int get_v(double &vx,double &vy,double &vz);
    int get_ds(double &sx,double &sy,double &sz);
    
};









#endif // __MY_IMU__

