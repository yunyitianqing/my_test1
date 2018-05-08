
#include <string>  
#include <vector>  
#include "myserialport.h"  
  
  
//Define Constructor function  
MySerialPort::MySerialPort( const any_type &port_name ):pSerialPort( NULL )  
{  
        pSerialPort = new serial_port( m_ios );  
    if ( pSerialPort ){  
        init_port( port_name, 8 );  
    }  
}   
  
//Define destructor function  
MySerialPort::~MySerialPort()  
{  
       if( pSerialPort )  
       {  
               delete pSerialPort;  
       }       
}  
  
  
//Initialize port  
bool MySerialPort::init_port( const any_type port, const unsigned int char_size )  
{  
    //New not success  
    if ( !pSerialPort ){  
        return false;  
    }  
  
        //Open Serial port object  
        pSerialPort->open( port, ec );  
      
  
    //Set port argument  
    pSerialPort->set_option( serial_port::baud_rate( 115200 ), ec );  
    //pSerialPort->set_option( serial_port::flow_control( serial_port::flow_control::none ), ec );  
    //pSerialPort->set_option( serial_port::parity( serial_port::parity::none ), ec );  
    //pSerialPort->set_option( serial_port::stop_bits( serial_port::stop_bits::one ), ec);  
    //pSerialPort->set_option( serial_port::character_size( char_size ), ec);  
  
    return true;  
}  
  
  
//Define wirte_to_serial to write data to serial  
void MySerialPort::write_to_serial( const any_type data )  
{  
        size_t len = write( *pSerialPort, buffer( data ), ec );  
        cout << len << "\t" << data << "\n";  
}  
  
void MySerialPort::handle_read( char buf[], boost::system::error_code ec,  
    std::size_t bytes_transferred )  
{  
    cout << "\nhandle_read: ";  
    cout.write(buf, bytes_transferred);  
}  
  
  
  
//Read data from the serial  
void MySerialPort::read_from_serial()  
{  
       char v[10];  
       async_read( *pSerialPort, buffer(v), boost::bind( &MySerialPort::handle_read, this, v, _1, _2) );  
       
       // 100ms后超时
       // deadline_timer timer(m_ios);
       // timer.expires_from_now(boost::posix_time::millisec(10));
        // 超时后调用sp的cancel()方法放弃读取更多字符
//timer.async_wait(boost::bind(&serial_port::cancel, boost::ref(*pSerialPort)));
       
}  
  
  
//Call handle_read function when async function read complete or come out exception  
void MySerialPort::call_handle()  
{  
        //There can use deadline_timer to cancle serial_port read data  
  
    //Wait for call callback function  
    m_ios.run();  
}  


int MySerialPort::my_read_some(unsigned char*data,int size)
{
    
      
    return pSerialPort->read_some(boost::asio::buffer(data,size),ec);
}


