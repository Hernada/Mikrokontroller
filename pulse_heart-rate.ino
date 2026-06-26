//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 01_Test_Pulse_Sensor_and_Get_Threshold_Value
#define PulseSensor_PIN 36 

int Signal; //--> Accommodates the signal value (ADC value) from the pulse sensor.
int UpperThreshold = 520;
int LowerThreshold = 500;

//________________________________________________________________________________VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); //--> Set's up Serial Communication at certain speed.
  Serial.println();
  delay(2000);

  // Set the ADC resolution. "analogReadResolution(10);" meaning the ADC resolution is set at 10 bits (the ADC reading value is from 0 to 1023).
  analogReadResolution(10);
}
//________________________________________________________________________________

//________________________________________________________________________________VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

  Signal = analogRead(PulseSensor_PIN); //--> Read the PulseSensor's value. Assign this value to the "Signal" variable.

  Serial.println(Signal); //--> Send the Signal value to Serial.

  delay(20);
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<