#include <MsTimer2.h>
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
void setup()
{
  Wire.begin();//I2C通信,我是主机，我没参数，进入总线
  tempSetup();
  adxlSetup();
  MsTimer2::set(2, interrupt);//使用timer2定时器，每2ms进入一次中断
  MsTimer2::start();
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
  /*static boolean OUT = H;
  digitalWrite(beatPin,OUT);
  OUT = !OUT;*/
  //getTemp();
  pulseSensor();
}

void SerialOut()
{
  /*Serial.print("BPM is ");
  Serial.println(BPM);
  getTemp();*/
  Serial.print("STEP is ");
  Serial.println(step_cnt);
  //Serial.println(curTime);
}
void loop()
{
  getSteps();
  SerialOut();
  //delay(300);
}
