#include"mySCMdrive.h"
#include<math.h>

#define PI 3.14159265

MySCMDrive::MySCMDrive()
{
    m_port=0;
    
    m_pid=0;
    
    
    time_ms=0;
    
    SpeedLeftOdo=0;
    SpeedRightOdo=0;
    Yaw=0;    
    
    Aacx=0;
    Aacy=0;
    
    
    
// 计算获得的二次数据   

    vx=0;
    vy=0;
    
    dx=0;
    dy=0;
 
    delta_distance=0;//积分距离
    delta_x=0;//积分坐标x
    delta_y=0;//积分坐标y
    
    x=0;//当前坐标x
    y=0;//当前坐标y
    
    
    pthread_mutex_init(&mutex,0);
}

MySCMDrive::~MySCMDrive()
{
    pthread_mutex_destroy(&mutex);
}

bool MySCMDrive::init_port(char*port_name)
{
    m_port=new MySerialPort(port_name);
    if(m_port==0)
        return false;
    if(m_port->ec)
        return false;
    else
        return true;
}

int MySCMDrive::run()
{
    last_time=clock();
    if(pthread_create(&m_pid,NULL,process_thread,this) != 0) 
    {    
        return -1; 
    }    
    return 0;
}


void * MySCMDrive::process_thread(void *param)
{
    MySCMDrive*This=(MySCMDrive*)param;
    
    unsigned char queue[2048];
    memset(queue,0,2048);
    int index_begin=0;

    
    unsigned char buf[1024];
    
    printf("run SCMdrive thread\n");
    
    
    while(1)
    {
        
        memset(buf,0,1024);
        int n=This->m_port->my_read_some(buf,1024);
        if(This->m_port->ec==0)
        {
            if(n>0)
            {
                if((index_begin + n) < 2048)
                {
                    //printf("memcpy buffer to queue index_begin=%d,n=%d\n",index_begin,n);
                    memcpy(queue+index_begin,buf,n);
                    
                    index_begin=index_begin + n;
                    
                }
                else
                {
                    //收到大量乱码时可能会走到这里
                    fprintf(stderr,"error???");
                    
                    //重置队列
                    index_begin=0;
                    memset(queue,0,2048);
                }
            }
        }
        else
        {
            std::cout<<"error:"<<This->m_port->ec.message()<<std::endl;
        }
    
    
//---------------处理包    
        
        int index=This->find_pkg_in_queue(queue,2048);
        //printf("index=%d\n",index);
        if(index>=0)
        {
            MyPackage_RecvSCM msg;
            
            //复制队列中找到的包数据到包
            memcpy(msg.data,queue+index,MyPackage_RecvSCM::package_length);
            //重置队列
            index_begin=0;
            memset(queue,0,2048);
            //校验和
            //printf("check=%d\n",This->check_msg(msg));
            if(This->check_msg(msg))
            {
                memcpy(This->last_SCM_package.data,
                    msg.data,MyPackage_RecvSCM::package_length);
                    
                //处理
                This->data_process();
            }
        }
        else
        {
            //跑到这里说明数据没传完，包不全，直接过，接着去上面收数据
        }
    
        usleep(100000);
    
    }
    
}

bool MySCMDrive::check_msg(MyPackage_RecvSCM &msg)
{
    unsigned int sum=0;
    for(int i=MyPackage_RecvSCM::HEAD_index+MyPackage_RecvSCM::HEAD_len;
        i<MyPackage_RecvSCM::CHKSM_index;i++)
    {
        sum+=msg.data[i];
    }
    
    if(sum>0xff)
    {
        sum=~sum;
        sum+=1;
    }
    
    sum=sum&0xff;
        
    
    //printf("check: sum=%d msg_chksm=%d\n",sum,(unsigned int)msg.data[MyPackage_RecvSCM::CHKSM_index]);
    
    if((unsigned int)msg.data[MyPackage_RecvSCM::CHKSM_index]==sum)
        return true;
    else
        return false;
        
        
}

int MySCMDrive::find_pkg_in_queue(unsigned char*queue,int size)
{
    unsigned char head[5]={0XAA,0XAA,0X08,0X04,0X02};
    //char end[7]={0XAA 0XAA 0X08 0X04 0X01 0D 0A};
    unsigned char end[5]={0XAA,0XAA,0X08,0X04,0X01};
    
    
    
    int begin_idx=-1;
    int end_idx=-1;
    for(int i=0;i<size-5;i++)
    {
        int flag_head=0;
        int flag_end=0;
        
        for(int j=0;j<5;j++)
        {
            if(begin_idx==-1)
            {//找包头
                if(queue[i+j]==head[j])
                    flag_head++;
            }
            else
            {//找包尾
                if(queue[i+j]==end[j])
                   flag_end++;
            }
        }
        
        if(flag_head==5)
            begin_idx=i;
            
        if(flag_end==5)
        {
            end_idx=i;
            break;
        }
        
    }
    
    //printf("find_pkg_in_queue: begin_idx=%d,end_idx=%d\n",begin_idx,end_idx);
    
    if(end_idx-begin_idx==58)
        return begin_idx;
    else
        return -1;
    
}

int MySCMDrive::data_process()
{
    unsigned char* data=last_SCM_package.data;
    short tmpData=0;
    
    pthread_mutex_lock(&mutex);
    
    clock_t now_time=clock();
    double time_s= (double)(now_time-last_time)/CLOCKS_PER_SEC;
    last_time=now_time;
    //左轮odo
    
    tmpData |= data[MyPackage_RecvSCM::SpeedLeftOdo_index+1];
    tmpData = tmpData << 8;
    tmpData |= (data[MyPackage_RecvSCM::SpeedLeftOdo_index] & 0xff);
    SpeedLeftOdo=int(tmpData) / 100.0;
    tmpData=0;
    
    //右轮odo
    
    tmpData |= data[MyPackage_RecvSCM::SpeedRightOdo_index+1];
    tmpData = tmpData << 8;
    tmpData |= (data[MyPackage_RecvSCM::SpeedRightOdo_index] & 0xff);
    SpeedRightOdo=int(tmpData) / 100.0;
    tmpData=0;
    
    //Yaw
    
   	tmpData |= data[MyPackage_RecvSCM::Yaw_index+1];
 	tmpData = tmpData << 8;
 	tmpData |= (data[MyPackage_RecvSCM::Yaw_index] & 0xff);
	Yaw = tmpData / 100.0;
	tmpData = 0;
    
    //航向角是角度，左正右负，通过odo进行计算，已废弃
    //delta_distance=time_s*(SpeedLeftOdo+SpeedRightOdo)/2;
    //delta_x=delta_distance*sin(Yaw*PI/180.0);
    //delta_y=delta_distance*cos(Yaw*PI/180.0);
    //x+=delta_x;
    //y+=delta_y;
    
    //Ax
    tmpData |= data[MyPackage_RecvSCM::Aacx_index+1];
 	tmpData = tmpData << 8;
 	tmpData |= (data[MyPackage_RecvSCM::Aacx_index] & 0xff);
	Aacx = (tmpData / 16384.0) * 9.8;
	tmpData = 0;
	
	
	//Ay
	tmpData |= data[MyPackage_RecvSCM::Aacy_index+1];
 	tmpData = tmpData << 8;
 	tmpData |= (data[MyPackage_RecvSCM::Aacy_index] & 0xff);
	Aacy = (tmpData / 16384.0) * 9.8;
    tmpData = 0;
    
    //Az
	tmpData |= data[MyPackage_RecvSCM::Aacz_index+1];
 	tmpData = tmpData << 8;
 	tmpData |= (data[MyPackage_RecvSCM::Aacz_index] & 0xff);
	Aacz = (tmpData / 16384.0) * 9.8;
    tmpData = 0;
    
    //通过ax，ay来计算vx，vy。进一步计算dx,dy, 最后到xy, 废弃
    //vx+= Aacx*time_s;
    //vy+= Aacy*time_s;
    
    //dx= vx*time_s;
    //dy= vy*time_s;
    

    
    
    pthread_mutex_unlock(&mutex);
    
    return 0;
    
}

void MySCMDrive::get_dx_dy_Yaw(double&dx,double&dy,double&Yaw)
{
    pthread_mutex_lock(&mutex);
    
    dx=this->dx;
    dy=this->dy;
    Yaw=this->Yaw;
    
    pthread_mutex_unlock(&mutex);
}

void MySCMDrive::get_ax_ay_az(double&ax,double&ay,double&az)
{
    pthread_mutex_lock(&mutex);
    
    ax=this->Aacx;
    ay=this->Aacy;
    az=this->Aacz;
    
    pthread_mutex_unlock(&mutex);
}

double MySCMDrive::get_Yaw()
{
    return Yaw;
}
