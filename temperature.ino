#include <Wire.h>
#include "Protocentral_MAX30205.h"

MAX30205 tempSensor;

void tempSetup()
{
  Wire.begin();
  tempSensor.begin();
}
void getTemp()
{
  float temp = tempSensor.getTemperature();
  Serial.print("Temp is ");
  Serial.print(temp,2);
  Serial.println("°C");
}
