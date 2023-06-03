#include <Servo.h>
#include <PID_v2.h>

Servo Motor;  // create a servo object

double Setpoint, Input, Output;
double Kp=0.1, Ki=0.4, Kd=0.005;
double Fehler;
double Regelwert;
int Incoming_value;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  
  Serial.begin(9600);
  
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.SetSampleTime(10);
  myPID.SetOutputLimits(0,180);

  Motor.attach(5,1000,2000);

  Motor.write(180);
  delay(5000);
  Motor.write(0);
  delay(5000);
  
}

void loop() {
  
  Setpoint = Incoming_value;
  Input = analogRead(A1);

  Fehler=Setpoint-Input;

  myPID.Compute();

  Motor.write(Output);


  if(Serial.available()>0) {
    Incoming_value=Serial.read();
    Incoming_value*=2.5;
    Serial.println(Incoming_value);
  }

}
