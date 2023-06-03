#include <PID_v1.h>

int sensorValue,i=0;
int nuller=0;
double Setpoint, Input, Output;
double Kp=0.0, Ki=5, Kd=0.0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {

  pinMode(3, OUTPUT);
  Serial.begin(9600);
  
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Kp,Ki,Kd);
  
}

void loop() {
  Setpoint = analogRead(1);


  Input = analogRead(0);
  Input=-Input+1023;
  myPID.Compute();
  analogWrite(3, Output);
  delay(10);

  Serial.print(0);  // To freeze the lower limit
  Serial.print(" ");
  Serial.print(300);  // To freeze the upper limit
  Serial.print(" ");
  Serial.print(Input);
  Serial.print(" ");
  Serial.println(Setpoint);
  
}
