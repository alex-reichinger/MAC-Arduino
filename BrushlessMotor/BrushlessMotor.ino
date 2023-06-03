#include <Servo.h>

Servo myservo; 
int val;

void setup() {
  myservo.attach(6,1000,2000); 
  Serial.begin(115200);
}

void loop() {
  val = analogRead(A1);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  myservo.write(val);     
  Serial.println(val);
  delay(50);       
      

}
