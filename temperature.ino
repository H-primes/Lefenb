MAX30205 tempSensor;

void tempSetup()
{
  tempSensor.begin();
}
void getTemp(float* temp)
{
  *temp = tempSensor.getTemperature();
}
