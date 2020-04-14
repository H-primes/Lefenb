int xA, yA, zA;
int xMax, xMin, yMax, yMin, zMax, zMin;
void adxlSetup(){ 
    Wire.beginTransmission(ADDR);//传输数据到adxl345
    Wire.write(0x2C);//BW_RATE寄存器地址0x2C
    Wire.write(0x0A);//输出速率100Hz  （1010）
    Wire.endTransmission();//停止I2C通讯

    Wire.beginTransmission(ADDR);
    Wire.write(0x2D);//POWER_CTL寄存器
    Wire.write(0x08);//关闭自动休眠
    Wire.endTransmission();
    
    Wire.beginTransmission(ADDR);
    Wire.write(0x2D);//DATA_FORMAT寄存器(地址0x31)
    Wire.write(0x08);//关闭自检
    Wire.endTransmission();
    delay(500);
}
void getSteps(){
    getAccls();
    
}
void getAccls(){ 
    unsigned int data[6];
    for(int i = 0; i < 6; i++){
        Wire.beginTransmission(ADDR);
        Wire.write(50 + i);
        Wire.endTransmission();

        Wire.requestFrom(ADDR,1);//从adxl请求一个字节
        if(Wire.available() == 1){ 
            data[i] = Wire.read();
        }
    }

    //转化成10-bits
    xA = (((data[1] & 0x03) * 256) + data[0]);
    if(xA > 511)
    {
        xA -= 1024;
    }
    yA = (((data[3] & 0x03) * 256) + data[2]);
    if(yA > 511)
    {
        yA -= 1024;
    }
    zA = (((data[5] & 0x03) * 256) + data[4]);
    if(zA > 511)
    {
        zA -= 1024;
    }
}
