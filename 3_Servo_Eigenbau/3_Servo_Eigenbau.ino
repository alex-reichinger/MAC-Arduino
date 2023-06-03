#include <PID_v2.h>

double Setpoint, Input, Output;
double Kp=2, Ki=0, Kd=0.02;
double Fehler;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  
  Serial.begin(9600);
  
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.SetSampleTime(10);
  myPID.SetOutputLimits(-255,255);

}

void loop() {
  
  Setpoint = analogRead(A1);
  delay(2);
  if (Setpoint<=50) Setpoint=50;
  if (Setpoint>=950) Setpoint=950;

  Input = analogRead(A0);
  delay(2);


  myPID.Compute();

  Fehler=Setpoint-Input;
  
  if (Setpoint>Input) {
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    if (Fehler>30)  {
         analogWrite(2, Output);
    }
    else analogWrite(2,0);
  }

    if(Setpoint<Input) {
      digitalWrite(4, HIGH);
      digitalWrite(3, LOW);
      if (Fehler<-30)  {
         analogWrite(2, abs(Output));
      }
      else analogWrite(2,0);
  }

      Serial.print(0);  // To freeze the lower limit
      Serial.print(" ");
      Serial.print(1300);  // To freeze the upper limit
      Serial.print(" "); 
      Serial.print(Input);
      Serial.print(" ");
      Serial.println(Setpoint);
}
