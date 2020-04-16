#include <FlexiTimer2.h>
#include <Wire.h>
#include "Protocentral_MAX30205.h"
#include <SoftwareSerial.h>

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
  GPSSerial.begin(115200);//软串口
  Wire.begin();//I2C通信,我是主机，我没参数，进入总线
  tempSetup();
  adxlSetup();
  FlexiTimer2::set(2, interrupt);//使用timer2定时器，每2ms进入一次中断
  FlexiTimer2::start();
  pinMode(beatPin,OUTPUT);
  Serial.begin(115200);
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
  /*static boolean OUT = H;
  digitalWrite(beatPin,OUT);
  OUT = !OUT;*/
  //Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
  pulseSensor();
  //getSteps();
  
}
unsigned long lastTime0 = 0;
float temp;
void SerialOut()
{
  Serial.println(millis()-lastTime0);
  if(millis()-lastTime0 > 300){
      Serial.print("BPM is ");
      Serial.println(BPM);
  
      Serial.print("STEP is ");
      Serial.println(step_cnt);
      //Serial.println(curTime);

      Serial.print("TEMP is ");
      Serial.print(temp,2);
      Serial.println("°C");
      lastTime0 = millis();
  }
}
void loop()
{ 
  get_gps_info();
  getTemp(&temp);
  getSteps();
  SerialOut();
}
