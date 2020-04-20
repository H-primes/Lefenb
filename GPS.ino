void get_gps_info() 
{
  //Serial.println("in get_gps_info()");
    while (GPSSerial.available()) //串口获取到数据开始解析
    {
       /* while(GPSSerial.peek() != '$'){
            GPSSerial.read();
        }*/
        char c = GPSSerial.read();  //读取一个字节获取的数据
        //Serial.print(c);
        if(c == '$')        //判断该字节的值,若是$，则说明是一帧数据的开始
        {
             GPSSerial.readBytesUntil('*', nmeaSentence, 67);    //读取接下来的数据，存放在nmeaSentence字符数组中，最大存放67个字节
             //Serial.println(nmeaSentence);
             latitude = parseGprmcLat(nmeaSentence); //获取纬度值
             longitude = parseGprmcLon(nmeaSentence);//获取经度值
             lndSpeed = parseGprmcSpeed(nmeaSentence);//获取速度值
             gpsTime = parseGprmcTime(nmeaSentence);//获取GPS时间 
              if(GPS_state){
                    if(latitude > "")   /*当不是空时候打印输出*/
                    {
                        //Serial.println("------------------------------------");
                        Serial.print("latitude: ");
                        Serial.println(latitude);
                    }

                    if(longitude > "")    /*当不是空时候打印输出*/
                    {
                        Serial.print("longitude: ");
                        Serial.println( longitude);
                    }  

                   if(lndSpeed > "")   /*当不是空时候打印输出*/
                    {
                        Serial.print("Speed (knots): ");
                        Serial.println(lndSpeed);
                    }

                    if(gpsTime > "")    /*当不是空时候打印输出*/
                    {
                        // Serial.println("gpsTime: " + gpsTime);
                        beiJingTime = getBeiJingTime(gpsTime);  //获取北京时间 
                        Serial.println("beiJingTime: " + beiJingTime);        
                    }
            }
             
             break;          
      }
    }
  
}

String getBeiJingTime(String s)
{
  int hour = s.substring(0,2).toInt();
  int minute = s.substring(2,4).toInt();
  int second = s.substring(4,6).toInt();

  hour += 8;

  if(hour > 24)
    hour -= 24;
  s = String(hour) +":"+String(minute) +":"+ String(second);
  return s;
}

String parseGprmcLat(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lat;
  
  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 5; i++)
    {
      if(i < 3) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        
      }
      if(i == 3)
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lat = s.substring(pLoc+1, lEndLoc);
        
      }
      else
      {
        dEndLoc = s.indexOf(',', lEndLoc+1);
        lat = lat + " " + s.substring(lEndLoc+1, dEndLoc);
       
      }
    }
   int indexx  = lat.indexOf('.');
   lat = lat.substring(0, indexx-2)+"."+lat.substring(indexx-2,indexx)+lat.substring(indexx+1,lat.length());
   return lat;
  }
}

//得到经度
String parseGprmcLon(String s)
{
  int pLoc = 0; 
  int lEndLoc = 0; 
  int dEndLoc = 0; 
  String lon;

  
  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 7; i++)
    {
      if(i < 5) 
      {
        pLoc = s.indexOf(',', pLoc+1);

      }
      if(i == 5)
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lon = s.substring(pLoc+1, lEndLoc);
      }
      else
      {
        dEndLoc = s.indexOf(',', lEndLoc+1);
        lon = lon + " " + s.substring(lEndLoc+1, dEndLoc);
      }
    }
    int indexx  = lon.indexOf('.');
    lon = lon.substring(0, indexx-2)+"."+lon.substring(indexx-2,indexx)+lon.substring(indexx+1,lon.length());
    return lon; 
  }
}

//得到速度
String parseGprmcSpeed(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lndSpeed;

  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 8; i++)
    {
      if(i < 7) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        
      }
      else
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lndSpeed = s.substring(pLoc+1, lEndLoc);
      }
    }
    return lndSpeed; 
  }
}

String parseGprmcTime(String s)
{
  int pLoc = 0; 
  int lEndLoc = 0;
  int dEndLoc = 0;
  String gpsTime;

  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 2; i++)
    {
      if(i < 1) 
      {
        pLoc = s.indexOf(',', pLoc+1);
      }
      else
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        gpsTime = s.substring(pLoc+1, lEndLoc);
      }
    }
    return gpsTime; 
  }
}

String charToString(char *c)
{

  String val = "";

  for(int i = 0; i <= sizeof(c); i++) 
  {
    val = val + c[i];
  }

  return val;
}
