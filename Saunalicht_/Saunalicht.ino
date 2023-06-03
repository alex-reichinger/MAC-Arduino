int R=9;
int G=10;
int B=11;
int P1=A0;
int P2=A1;
int FadeR=255;
int FadeG=0;
int FadeB=0;
int FadeTime=20;
int MerkerR=0;
int MerkerG=0;
int MerkerB=0;
int y,stellung;
int timer=0,wechsel=0, sek=0, state=1;
double zeit;



//Timer-Interrupt-Routine:
ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  timer++;
  if(timer>=1000) {
    timer=0;
    sek++;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);

  //Timer wird so eingestellt, dass alle 1ms der Timer-Interrupt ausgelöst wird.
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  //OCR0A=0xF9; //Value for ORC0A for 1ms 
  OCR0A=255; //Value for ORC0A for 1ms 
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  sei(); //Enable Global-Interrupt
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);
}

void loop() {

  //Serial.println(wechsel);

  stellung=analogRead(P2);
  if(stellung<100) {
        wechsel=0;
        
  }

  else {
       wechsel=1;
       goto Sprung;
  }

  zeit=analogRead(P1);
  zeit=zeit/1023*179+1;
  zeit=(int)zeit;
  

  if(wechsel==0) {

    if(sek>=zeit) {
      state++;
      sek=0;
    }
  

    switch(state) {
    case 1:
      for(FadeG=0; FadeG<=255; FadeG++) {
        analogWrite(G,FadeG);
        stellung=analogRead(P2);
        if(stellung>=100) {
          wechsel=1;
          goto Sprung;
        }
        delay(FadeTime);
      }
      state++;
      sek=0;
    break;
      
    case 3:
      for(FadeR=255; FadeR>=0; FadeR--) {
        analogWrite(R,FadeR);
        stellung=analogRead(P2);
        if(stellung>=100) {
          wechsel=1;
          goto Sprung;
        }
        delay(FadeTime);
      }
      state++;
      sek=0;
    break;

    case 5:
      for(FadeB=0; FadeB<=255; FadeB++) {
        analogWrite(B,FadeB);
        stellung=analogRead(P2);
        if(stellung>=100) {
          wechsel=1;
          goto Sprung;
        }
        delay(FadeTime);
      }  
      state++;
      sek=0;
    break;

    case 7:
      for(FadeG=255; FadeG>=0; FadeG--) {
        analogWrite(G,FadeG);
        stellung=analogRead(P2);
        if(stellung>=100) {
          wechsel=1;
          goto Sprung;
        }
        delay(FadeTime);
      }
      state++;
      sek=0;
    break;

    case 9:
      for(FadeR=0; FadeR<=255; FadeR++) {
        analogWrite(R,FadeR);
        stellung=analogRead(P2);
        if(stellung>=100) {
          wechsel=1;
          goto Sprung;
        }
        delay(FadeTime);
      }
      state++;
      sek=0;
    break;

    case 11:
      for(FadeB=255; FadeB>=0; FadeB--) {
        analogWrite(B,FadeB);
        stellung=analogRead(P2);
        if(stellung>=100) {
          wechsel=1;
          goto Sprung;
        }
        delay(FadeTime);
      }
      state=0;
      sek=0;
    break;
      
    } //end switch
  } //end if


  Sprung:
  
  if(wechsel==1) {
    state=1;
    sek=0;
    y=analogRead(P2);
    y=y*6.5/1023;
  
    switch(y){
      case 1:
        digitalWrite(R,HIGH);
        digitalWrite(G,LOW);
        digitalWrite(B,LOW);
      break;
      
      case 2:
        digitalWrite(R,HIGH);
        digitalWrite(G,HIGH);
        digitalWrite(B,LOW);
      break;
  
      case 3:
        digitalWrite(R,LOW);
        digitalWrite(G,HIGH);
        digitalWrite(B,LOW);
      break;
  
      case 4:
        digitalWrite(R,LOW);
        digitalWrite(G,HIGH);
        digitalWrite(B,HIGH);
      break;
  
      case 5:
        digitalWrite(R,LOW);
        digitalWrite(G,LOW);
        digitalWrite(B,HIGH);
      break;
  
      case 6:
        digitalWrite(R,HIGH);
        digitalWrite(G,LOW);
        digitalWrite(B,HIGH);
      break;
    } //end switch
  } //end if
  
} //end loop
 
