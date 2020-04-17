#include <MsTimer2.h>
#include <Wire.h>
#include "Protocentral_MAX30205.h"
#include <SoftwareSerial.h>

bool BPM_state = false;
bool GPS_state = false;
bool TEMP_state = false;

int beatPin = 13;//随脉搏闪烁
int pulsePin = 0;//传感器连接模拟引脚A0
int BPM;//脉搏
int IBI;//间隔时间
bool detected = false;//ture当侦测到心跳
bool completed = false;//上一次心跳完成
#define H HIGH
#define L LOW

//ADXL345 I2C地址是0x53
#define ADDR 0x53
static int step_cnt = 0;
//------------------------------------
//gps
char nmeaSentence[68];
String latitude;    //纬度
String longitude;   //经度
String lndSpeed;    //速度
String gpsTime;     //UTC时间，本初子午线经度0度的时间，和北京时间差8小时
String beiJingTime;   //北京时间

SoftwareSerial GPSSerial(10, 11); // RX, TX
//--------------------------------------
void setup()
{
  GPSSerial.begin(9600);//软串口
  Wire.begin();//I2C通信,我是主机，我没参数，进入总线
  tempSetup();
  adxlSetup();
  MsTimer2::set(20, interrupt);//使用timer2定时器，每2ms进入一次中断
 // MsTimer2::start();
  pinMode(beatPin,OUTPUT);
  Serial.begin(9600);
}


int IBIS[10];//记录最近10次的IBI
unsigned long curTime = 0;//当前运行时间
unsigned long lastTime = 0;//上次的心跳
int Signal;//读取pulsePin的
int interval;//每次脉搏已读取的时间
int flagValue = 525;//特征值
int P = 512 ,V = 512 ,amp = 50;//峰，谷，振幅amplitude
bool firstIBI = true; 
void interrupt()
{
  //Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
 pulseSensor();
}
unsigned long lastTime0 = 0;
float temp;
void SerialOut()
{
  //Serial.println(millis()-lastTime0);
  if(millis()-lastTime0 > 600){
      if(BPM_state){
          Serial.print("BPM is ");
          Serial.println(BPM);
      }
        
      Serial.print("STEP is ");
      Serial.println(step_cnt);
      //Serial.println(curTime);

     if(TEMP_state){
         Serial.print("TEMP is ");
         Serial.print(temp,2);
         Serial.println("°C");
     }      
     if(GPS_state){
          if(latitude > "")   //当不是空时候打印输出
            {
                Serial.println("------------------------------------");
                Serial.println("latitude: " + latitude);
            }

            if(longitude > "")    //当不是空时候打印输出
            {
                Serial.println("longitude: " + longitude);
            }  

            if(lndSpeed > "")   //当不是空时候打印输出
            {
                Serial.println("Speed (knots): " + lndSpeed);
            }

            if(gpsTime > "")    //当不是空时候打印输出
            {
                Serial.println("gpsTime: " + gpsTime);
                beiJingTime = getBeiJingTime(gpsTime);  //获取北京时间 
                Serial.println("beiJingTime: " + beiJingTime);        
            }
     }
      lastTime0 = millis();
  }
}
char command;
void loop()
{

  //TODO:read!!!
  if(Serial.available()){
      command = Serial.read(); 
      Serial.println("++++++++++++++++++++++++++++++++++++++++++++");
      Serial.println(command);
  }
  if(command == '1'){//开始测心率
    if(!BPM_state){
        MsTimer2::start();
        BPM_state = true;
    }
  }else if(command == '2'){
    if(BPM_state){
        MsTimer2::stop();
        BPM_state = false;
    }     
  }

  if(command == '3'){//开始测体温
      TEMP_state = true;
  }else if(command == '4'){
      TEMP_state = false;
  }
  if(TEMP_state) {
      getTemp(&temp);
  }

  if(command == '5'){
      GPS_state = true;
  }else if(command == '6'){
      GPS_state = false;
  }
  get_gps_info();
  getSteps();
  SerialOut();
}
