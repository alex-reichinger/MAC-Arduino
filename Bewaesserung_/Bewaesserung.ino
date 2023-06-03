#define V_Glas        2
#define V_Mitte       3
#define V_Links       4
#define Selbsthalter  5
#define Alle          6
#define Stop          7
#define high          LOW
#define low           HIGH

int Z_Glas;
int Z_Mitte;
int Z_Links;

int zeit=0;
int timer=0;
int sek=0;
int minute=0;
int state=1;
int Umschalt=0;
int Programmstatus=0;
int a=0, b=0, c=0;


//Timer-Interrupt-Routine:
ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  timer++;
  if(timer>=1000) {
    timer=0;
    sek++;
    if(sek>=60) {
      sek=0;
      minute++;
    }
  }
}

void setup() {

 pinMode(V_Glas,OUTPUT);
 pinMode(V_Mitte,OUTPUT);
 pinMode(V_Links,OUTPUT);
 pinMode(Selbsthalter,OUTPUT);
 pinMode(Alle,INPUT);
 pinMode(Stop,INPUT);

 digitalWrite(V_Glas, low);
 digitalWrite(V_Mitte, low);
 digitalWrite(V_Links, low);
 digitalWrite(Selbsthalter, high);
 
 //Timer wird so eingestellt, dass alle 1ms der Timer-Interrupt ausgelöst wird.
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  //OCR0A=0xF9; //Value for ORC0A for 1ms 
  OCR0A=255; //Value for ORC0A for 1ms 
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  sei(); //Disable Global-Interrupt
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);

 Serial.begin(9600);


}

void loop() {


   Z_Glas=analogRead(A0);  
   Z_Mitte=analogRead(A1);  
   Z_Links=analogRead(A2);  

    Z_Glas = map(Z_Glas, 0, 1023, 0, 30);
    Z_Mitte = map(Z_Mitte, 0, 1023, 0, 30);
    Z_Links = map(Z_Links, 0, 1023, 0, 30)+zeit;


    Serial.println("Glas, Mitte, Links: ");
    Serial.print(Z_Glas);
    Serial.print(" ");
    Serial.print(Z_Mitte);
    Serial.print(" ");
    Serial.println(Z_Links);

    delay(100);


    //Z_Glas=Z_Glas*0.235;
    //Z_Mitte=Z_Mitte*0.235;
    //Z_Links=Z_Links*0.235;

    /*Serial.println(Glas);
    Serial.println(Mitte);
    Serial.println(Links);
    Serial.println(' ');*/
    
    if(digitalRead(Alle)==HIGH) {
  
      Z_Mitte=0;
      Z_Links=0;

    } 

  
    switch(state) {
      case 1: //Initialisierung
        
        timer=0;
        sek=0;
        minute=0;      
        Programmstatus=0; 
        a=0;
        b=0;
        c=0;
        digitalWrite(V_Glas, high);
        digitalWrite(V_Mitte, high);
        state=2;
      break; //end case 1

      case 2: //Programm 
      Serial.println(sek);
        if(minute>=Z_Glas and a==0){
          a=1;
          digitalWrite(V_Glas, low);
          Programmstatus++;
          if(Umschalt==0) {
            digitalWrite(V_Links, high);
            zeit=minute;
            Umschalt=1;
            Z_Links=Z_Links+minute;
          }
        }
        
        if(minute>=Z_Mitte and b==0) {
          b=1;
          digitalWrite(V_Mitte, low);
          Programmstatus++;
          if(Umschalt==0) {    
            digitalWrite(V_Links, high);  
            zeit=minute;
            Umschalt=1;
            Z_Links=Z_Links+minute;
          }
        }

        if(minute>=Z_Links and Umschalt==1 and c==0) {
          c=1;
          digitalWrite(V_Links, low); 
          Programmstatus++;
        }

        if(Programmstatus==3 || digitalRead(Stop)==HIGH) {
           state=0;
           digitalWrite(V_Links, low); 
           digitalWrite(V_Mitte, low);
           digitalWrite(V_Glas, low);
           digitalWrite(Selbsthalter, low);
        }
      break; //end case 2

    } //end switch
 } //end loop
