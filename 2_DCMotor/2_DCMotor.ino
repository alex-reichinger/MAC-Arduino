#include <PID_v2.h>

//Variablen-Deklaration
double Setpoint, Input, Output;
double Kp=1, Ki=1, Kd=0.00;
double Analog;
double n=0,impuls=0;
int timer=0;


//PID-Regler Definieren
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


//Pin-Interrupt-Routine:
void impuls_isr() {
  impuls++;
}


//Timer-Interrupt-Routine:
ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  timer++;
}


void setup() {


  //Mehrere Pins gleichzeitig ansteuern/lesen (z.B. PortD=PINs 0-7; PortB=PINs 8-13,..)
  //DDRD = DDRD | B11111100; <--PortD-Definition; 0: Eingang; 1:Ausgang
  //PORTD = B00010000; <--PortD Ausgabe
  //Serial.println(PINB); <--PortB Pegel ausgeben /Lese-Modus

  
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);


  //Aktiviere RS232-Schnittstelle mit Baudrate 9600 (Symbole/Sek)
  Serial.begin(9600);


  //Timer wird so eingestellt, dass alle 1ms der Timer-Interrupt ausgelöst wird.
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  //OCR0A=0xF9; //Value for ORC0A for 1ms 
  OCR0A=255; //Value for ORC0A for 1ms 
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  sei(); //Enable Timer-Interrupt
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);


  //PID-Regler einstellen
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Kp,Ki,Kd);


  //aktiviere Pin-Interrupt 
  //1. Pin-Definition
  //2. Name der Interrupt-Routine
  //3. Rising --> bei steigender Flanke am Eingang (gibt auch FALLING, CHANGE,High,..)
  //https://www.arduino.cc/reference/de/language/functions/external-interrupts/attachinterrupt/
  attachInterrupt(digitalPinToInterrupt(3), impuls_isr, RISING);

  }


void loop() {
  
      //Der Timer0 löst jede 1ms aus und zählt eine Variable um eins hoch. 
      //Erreicht die Variable den Wert 100, sind exakt 100ms vergangen
      if (timer>=100) { 

      Setpoint = analogRead(0);
      Setpoint=Setpoint*1.3;
      //Setpoint = map(Analog, 0, 1023, 0, 255);
      impuls=impuls/16;
      n=impuls*60/0.1;
      Input=n;
      
      myPID.Compute();
      analogWrite(2, Output);
      
      Serial.print(0);  // To freeze the lower limit
      Serial.print(" ");
      Serial.print(1300);  // To freeze the upper limit
      Serial.print(" "); 
      Serial.print(Input);
      Serial.print(" ");
      Serial.println(Setpoint);
      
      impuls=0;
      timer=0;
      
    }

}
