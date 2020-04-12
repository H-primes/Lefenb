void pulseSensor()
{
  curTime += 2;//总时间+2ms
  Signal = analogRead(pulsePin);//读取Pulse Sensor的信号
  interval = curTime - lastTime;

  //记录峰值
  if(Signal > flagValue && Signal > P)
  {
    P = Signal;
  }
  //记录谷值
  if(Signal < flagValue && Signal < V && interval > (IBI*3/5))//尽量过滤干扰
  {
    V = Signal;
  }
  //2s未检测到心跳，恢复默认值
  if(interval > 2000)
  {
    flagValue = 525;
    P = 512;
    V = 512;
    lastTime = curTime;
    firstIBI = true;
  }
  //每次心跳完成
  if(Signal < flagValue && completed)
  {
    amp = P -V;
    flagValue = (P - V)/2 + V;
    completed = false;
    P = flagValue;
    V = flagValue;
    digitalWrite(beatPin,LOW);//此次心跳结束
  }


  //！计算心率！
  if(interval > 300)//认为每分钟心跳不超过60000/300 = 200次
  {
      if(Signal > flagValue && completed == false && interval > (IBI*3/5))
      {
        digitalWrite(beatPin,HIGH);//灯亮
        completed = true;//找到两个特征点之间的间隔
        lastTime = curTime;
        IBI = interval;
        int sumTime = 0;
        if(firstIBI)
        {
          for(int i=0;i<10;i++)
          {
            IBIS[i] = IBI;
            sumTime += IBI;
          }
          firstIBI = false;
        }else{
          for(int i=0;i<9;i++)
          {
            IBIS[i]=IBIS[i+1];
            sumTime += IBIS[i];
          }
          IBIS[9]=IBI;
          sumTime += IBI;
        }

        sumTime /= 10;
        BPM =60000/sumTime;
      }
  }
}
