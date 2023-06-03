
double myArray[20]={125,163,198,226,243,249,243,226,198,163,125,86,51,23,6,1,6,23,51,86};
int i=0;
double last;

void setup() {
  int oldClkPr = CLKPR;  // save old system clock prescale
  CLKPR = 0x80;    // Tell the AtMega we want to change the system clock
  CLKPR = 0x04;    // 1/256 prescaler = 60KHz for a 16MHz crystal

  pinMode(2, OUTPUT);
  
}

void loop() {

    digitalWrite(2, HIGH);
    
    digitalWrite(2, LOW);
    
    
}
