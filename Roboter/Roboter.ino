#include <Servo.h>
Servo schwenken;  // create a servo object
Servo hauptarm;  // create a servo object
Servo knickarm;  // create a servo object

int angle_knick;
int angle_haupt;
int angle_schwenken;


void setup() {

  Serial.begin(115200);

  schwenken.attach(8); // attaches the servo on pin 9 to the servo object
  hauptarm.attach(9);
  knickarm.attach(10);

  schwenken.write(90);
  hauptarm.write(90);
  knickarm.write(90);
  delay(1000);
}

void loop() {


    angle_schwenken=analogRead(A0); 
    angle_schwenken=angle_schwenken/6;
    angle_haupt=analogRead(A1); 
    angle_haupt=angle_haupt/6;
    angle_knick=analogRead(A2); 
    angle_knick=angle_knick/6;

    schwenken.write(angle_schwenken);
    hauptarm.write(angle_haupt);
    knickarm.write(angle_knick);
  /*for(angle_knick=0; angle_knick<=180; angle_knick++) {
    knickarm.write(angle_knick);


    schwenken.write(angle_knick);
    hauptarm.write(angle_knick);
    //angle_haupt=50*sin(angle_knick)+90;
    //Serial.println(angle_haupt);
    
    delay(8);
   }

  for(angle_knick=180; angle_knick>=0; angle_knick--) {
    knickarm.write(angle_knick);
    schwenken.write(angle_knick);
    hauptarm.write(angle_knick);
    //angle_haupt=50*sin(angle_knick)+90;
    //Serial.println(angle_haupt);
    delay(8 );
   }   
  */
  
}
