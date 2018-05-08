#include"myIMU.h"
#include<iostream>

MyIMU::MyIMU()
{
    for(int i=0;i<3;++i)
    {
        a[i]=0;
        g[i]=0;
        v[i]=0;
        ds[i]=0;
        global_s[i]=0;
        filter[i]=0;
    }
    
    yaw=0;
    flag_caculate=-1;
    stop_timeout=10;
    cur_stop_timeout=0;
}

void MyIMU::set_g(double gx,double gy,double gz)
{
    g[0]=gx;
    g[1]=gy;
    g[2]=gz;
}

void MyIMU::clear_v()
{
    v[0]=0;
    v[1]=0;
    v[2]=0;
}

void MyIMU::reset_global_s(double x,double y,double z)
{
    global_s[0]=x;
    global_s[1]=y;
    global_s[2]=z;
}

int MyIMU::get_a(double &ax,double &ay,double &az)
{    
    ax=a[0];
    ay=a[1];
    az=a[2];
    
    return flag_caculate;
}

int MyIMU::get_v(double &vx,double &vy,double &vz)
{    
    vx=v[0];
    vy=v[1];
    vz=v[2];
    
    return flag_caculate;
}
    
int MyIMU::get_ds(double &sx,double &sy,double &sz)
{
    sx=ds[0];
    sy=ds[1];
    sz=ds[2];
    
    return flag_caculate;
}


void MyIMU::caculate(double ax,double ay,double az)
{
    static bool flag = 0;
    
    
    if(flag==0)
    {
        gettimeofday(&start,NULL);
        flag=1;
        return;
    }
    
    
    
    gettimeofday(&end,NULL);
    
    double dt = (end.tv_sec-start.tv_sec)+ (end.tv_usec-start.tv_usec)/1000000.0;
    
    std::cout<<"dt:"<<dt<<std::endl;
    
    flag_caculate=0;
    
    double a_now[3];
    double v_now[3];
    
    std::cout<<"ax:"<<ax<<","<<"ay:"<<ay<<","<<"az:"<<az<<std::endl;
    
    //算a
    a_now[0]=ax-g[0];
    a_now[1]=ay-g[1];
    a_now[2]=az-g[2];
    
    
    
    double tmp[3];

    ///滤波，小于某个值的变化当作误差舍弃
    for(int i=0;i<3;i++)
    {
        if(a_now[i]<0)
            tmp[i]=-a_now[i];
        else
            tmp[i]=a_now[i];
            
        if(tmp[i]<filter[i])
        {
            a_now[i]=0;
        }
    }
    
    std::cout<<"a_now_x:"<<a_now[0]<<","<<"a_now_y:"<<a_now[1]<<","<<"a_now_z:"<<a_now[2]<<std::endl;
    //算v
    v_now[0]=v[0]+0.5*(a_now[0]+a[0])*dt;
    v_now[1]=v[1]+0.5*(a_now[1]+a[1])*dt;
    v_now[2]=v[2]+0.5*(a_now[2]+a[2])*dt;
    
    if(v_now[0]==v[0]&&v_now[1]==v[1]&&v_now[2]==v[2])
    {
        cur_stop_timeout++;
    }
    else
    {
        cur_stop_timeout=0;
    }
    
    
    
    std::cout<<"v_now_x:"<<v_now[0]<<","<<"v_now_y:"<<v_now[1]<<","<<"v_now_z:"<<v_now[2]<<std::endl;
    //算ds
    ds[0]=0.5*(v_now[0]+v[0])*dt;
    ds[1]=0.5*(v_now[1]+v[1])*dt;
    ds[2]=0.5*(v_now[2]+v[2])*dt;
    
    std::cout<<"ds_x:"<<ds[0]<<","<<"ds_y:"<<ds[1]<<","<<"ds_z:"<<ds[2]<<std::endl;
    
    //更新数据
    a[0]=a_now[0];
    a[1]=a_now[1];
    a[2]=a_now[2];
    
    v[0]=v_now[0];
    v[1]=v_now[1];
    v[2]=v_now[2];
    
    if(cur_stop_timeout>stop_timeout)
    {
        clear_v();
    }
    
    start=end;
    
    
}

void MyIMU::set_filter(double ax,double ay,double az)
{    
    filter[0]=ax;
    filter[1]=ay;
    filter[2]=az;
}

void MyIMU::set_stop_timeout(int times)
{
    stop_timeout=times;
}



