/*
 * 计算步数
 * 一开始自己根据心率的写的一种效果太差
 * 虽然说和心率的思想差不多，但是确实复杂很多
 * 然后去学习了ADI公司的资料和若干个csdn大佬的方法
 * 主要分为五步
 * 一、得到各轴加速度
 * 二、均值滤波(没用啥厉害的滤波算法)，拿到多组三个轴的数据，求平均值
 * 三、选取动态阈值：更新MAX和MIN。每采样50次更新一次，动态阈值去(max + min)>>1
 * 四、
 * 五、
 */
//---------------------------------------------------------------
#define FILTER_CNT 4
#define SAMPLE_SIZE 50
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))

typedef struct _ACCL{
    int x;
    int y;
    int z;
}ACCL;
typedef struct _FILTER{
    ACCL accls[FILTER_CNT];
    short count;
}FILTER;


void filter_calculate(struct _FILTER *filter, struct _ACCL *sample){
    int xSum = 0;
    int ySum = 0;
    int zSum = 0;
    for(int i = 0;i < FILTER_CNT; i++){
        xSum += filter->accls[i].x;
        ySum += filter->accls[i].y;
        zSum += filter->accls[i].z;
    }
    sample->x = xSum / FILTER_CNT;
    sample->y = ySum / FILTER_CNT;
    sample->z = zSum / FILTER_CNT;
}
//-------------------------------------------------------------------------
typedef struct _PEAK{
    ACCL newmax;
    ACCL newmin;
    ACCL oldmax;
    ACCL oldmin;
}PEAK;

void peak_init(struct _PEAK *peak){
    peak->newmax.x = -10000;
    peak->newmax.y = -10000;
    peak->newmax.z = -10000;
    peak->newmin.x = 10000;
    peak->newmin.y = 10000;
    peak->newmin.z = 10000;
}
void peak_update(struct _PEAK *peak, struct _ACCL *cur_sample){
    static int sample_size = 0;
    sample_size ++;
    if(sample_size > SAMPLE_SIZE){
        sample_size = 1;
        peak->oldmax = peak->newmax;
        peak->oldmin = peak->newmin;

        peak_init(peak);//初始化
    }

    peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
    peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
    peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);
  
    peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
    peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
    peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);

}
//-----------------------------------------------------------------------------
#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
#define DYNAMIC_PRECISION 30  //动态精度
typedef struct _slid_reg{
    ACCL new_sample;
    ACCL old_sample;
}SLID_REG;

bool slid_update(struct _slid_reg *slid, struct _ACCL *cur_sample){
    bool res = false;
    if (ABS((cur_sample->x - slid->new_sample.x)) > DYNAMIC_PRECISION) {
        slid->old_sample.x = slid->new_sample.x;
        slid->new_sample.x = cur_sample->x;
        res = true;
     } else {
        slid->old_sample.x = slid->new_sample.x;
     }
     if (ABS((cur_sample->y - slid->new_sample.y)) > DYNAMIC_PRECISION) {
        slid->old_sample.y = slid->new_sample.y;
        slid->new_sample.y = cur_sample->y;
        res = true;
     } else {
        slid->old_sample.y = slid->new_sample.y;
     }
  
     if (ABS((cur_sample->z - slid->new_sample.z)) > DYNAMIC_PRECISION) {
        slid->old_sample.z = slid->new_sample.z;
        slid->new_sample.z = cur_sample->z;
        res = true;
     } else {
        slid->old_sample.z = slid->new_sample.z;
     }
     return res;
}
//----------------------------------------------------
#define MOST_ACTIVE_NULL 0 //未找到最活跃轴
#define MOST_ACTIVE_X 1    //最活跃轴X  
#define MOST_ACTIVE_Y 2    //最活跃轴Y 
#define MOST_ACTIVE_Z 3    //最活跃轴Z 
 
#define ACTIVE_PRECISION 60       //活跃轴最小变化值
 
/*判断当前最活跃轴*/
static char is_most_active(struct _PEAK *peak)
{
  char res = MOST_ACTIVE_NULL;
  short x_change = ABS((peak->newmax.x - peak->newmin.x));
  short y_change = ABS((peak->newmax.y - peak->newmin.y));
  short z_change = ABS((peak->newmax.z - peak->newmin.z));
  
  if (x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION) {
    res = MOST_ACTIVE_X;
  } else if (y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION) {
    res = MOST_ACTIVE_Y;
  } else if (z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION) {
    res = MOST_ACTIVE_Z;
  }
  return res;
}
 
/*判断是否走步*/
static void detect_step(struct _PEAK *peak, struct _slid_reg *slid, struct _ACCL *cur_sample)
{
    char res = is_most_active(peak);
    switch (res) {
        case MOST_ACTIVE_NULL: {
          //fix
          break;
        }
        case MOST_ACTIVE_X: {
            short threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
            if (slid->old_sample.x > threshold_x && slid->new_sample.x < threshold_x) {
                step_cnt ++;
            }
            break;
        }
        case MOST_ACTIVE_Y: {
            short threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
            if (slid->old_sample.y > threshold_y && slid->new_sample.y < threshold_y) {
                step_cnt ++;
            }
            break;
        }
        case MOST_ACTIVE_Z: {
            short threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
            if (slid->old_sample.z > threshold_z && slid->new_sample.z < threshold_z) {
              step_cnt ++;
            }
            break;
        }
  }
}


//-----------------------------------------------------------------------------
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

PEAK peak0;
SLID_REG slid0;
void getSteps(){
    static bool need_init = true;
    if(need_init){
         need_init = !need_init;
         peak_init(&peak0);
    }
    ACCL sample;
    FILTER filter;
    for(int i = 0; i < FILTER_CNT; i++){
          getAccls(&(filter.accls[i]));
    }
    filter_calculate(&filter,&sample);//sample即为滤波后的数据
    
    peak_update(&peak0,&sample);//更新动态阈值

    bool res = slid_update(&slid0, &sample);//更新动态变化精度

    detect_step(&peak0, & slid0, &sample);  
}



void getAccls(struct _ACCL *acc){ 
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

    int xA,yA,zA;
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
    acc->x = xA;
    acc->y = yA;
    acc->z = zA;
}
