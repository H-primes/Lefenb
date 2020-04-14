MAX30205 tempSensor;

void tempSetup()
{
  tempSensor.begin();
}
void getTemp()
{
  float temp = tempSensor.getTemperature();
  Serial.print("Temp is ");
  Serial.print(temp,2);
  Serial.println("°C");
}
