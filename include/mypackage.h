#ifndef __MY_PACKAGE__
#define __MY_PACKAGE__



//收单片机的包
struct MyPackage_RecvSCM
{
    unsigned char data[65];
    enum{
        HEAD_index=0,
        HEAD_len=5,
        Data_type_index=5,
        Lenth_index=6,
        temperature_index=7,
        //......以后有需求再补吧
        
        SpeedLeftOdo_index=23,
        SpeedLeftOdo_len=2,
        
        SpeedRightOdo_index=25,
        SpeedRightOdo_len=2,
        
        Yaw_index=31,
        Yaw_len=2,
        
        Aacx_index=33,
        Aacx_len=2,
        
        Aacy_index=35,
        Aacy_len=2,
        
        Aacz_index=37,
        Aacz_len=2,
        
        //......
        
        CHKSM_index=57,
        CHKSM_len=1,
        //......
        
        
        
        
        package_length=65,
    };
};



#endif//__MY_PACKAGE__

