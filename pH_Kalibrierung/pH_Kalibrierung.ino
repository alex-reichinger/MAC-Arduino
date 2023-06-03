int pH_Value; 
float Voltage;
 
void setup() 
{ 
  Serial.begin(9600);
} 
 
void loop() 
{ 
  pH_Value = analogRead(A0); 
  Voltage = pH_Value * (5.0 / 1023.0); 
  Serial.println(Voltage); 
  delay(500); 
}
