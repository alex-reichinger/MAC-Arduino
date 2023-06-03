
//#include <IRremote.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN            2

#define NUM_LEDS            10

#define MAX_POWER_MILLIAMPS 500
#define LED_TYPE            WS2812B
#define COLOR_ORDER         GRB
#define BRIGHTNESS          50
#define FRAMES_PER_SECOND   60
#define VOLTS               12
#define MAX_MA              4000
#define COOLING  55
#define SPARKING 120
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//IR
int irPin = 4; //Pin am Arduino Nano für die IR LED
int ledPin = 7; //Pin am Arduino Nano für die LED als Signalbestätigung
int led1Pin = 5; //Pin am Arduino Nano für LED 1
int led2Pin = 6; //Pin am Arduino Nano für LED 2
int a = 0;
boolean Hilfsvar = HIGH;
boolean Time_int = LOW;
int sleepPIN = 2;
long value;
int start = 0, state = 1, state2 = 1;
long currentTime, Time=0;
int R;
int G;
int B;
int FadeR = 0;
int FadeG = 0;
int FadeB = 70;
int FadeTime = 20;
boolean test = 0;
int sek = 0;
long interval;
long zeit=1500;
double analogwert;
boolean led1Status = false;
boolean led2Status = false;
boolean first=true;
boolean ausschalten = 0;
int Teilung = 5; //************************************************TEILUNG******************
int x = 0;
long timer = 0;
int patternaelter=99;
int patternalt=99;
int randNumber;  
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
//IRrecv irrecv(irPin); //Objekt initialisieren für die IR Übertragung
//decode_results results;
//////////////////////////////////////////////////////////////////////////
bool gReverseDirection = false;
CRGB leds[NUM_LEDS];

//Pin-Interrupt-Routine:
void impuls_isr() {

    if(first==1) {
      randomSeed(analogRead(A3));
      first=0;
    }
    
    x=1; 
    detachInterrupt(digitalPinToInterrupt(3));
}

void setup() {
  delay( 100); // 3 second delay for boot recovery, and a moment of silence
  
  mySoftwareSerial.begin(9600);
  myDFPlayer.begin(mySoftwareSerial); 
  myDFPlayer.volume(22);
  pinMode(3, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, INPUT);


  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
  .setCorrection( TypicalLEDStrip );


  FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
  FastLED.setBrightness( BRIGHTNESS );

  Serial.begin(115200); //Serielle kommunikation mit 9600 Baud beginnen.
  //attachInterrupt(digitalPinToInterrupt(3), impuls_isr, RISING);

  
  myDFPlayer.playFolder(13,1);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { Fire2012, Schimmer, rainbow, confetti, Fire2012, sinelon, juggle, bpm, Schimmer_blau};



void loop() //*********************************************************************LOOP*********
{

  if(millis()>=zeit+3200) {
    attachInterrupt(digitalPinToInterrupt(3), impuls_isr, RISING);
    x=0;
    
  }

  gPatterns[gCurrentPatternNumber]();

 
  FastLED.show();
 
  EVERY_N_MILLISECONDS( 60 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }

  //delay(10);
  //attachInterrupt(digitalPinToInterrupt(3), impuls_isr, RISING);

  if(x==1) {
      
      for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1)  {
            leds[whiteLed] = CRGB::Black;

      }
      FastLED.show();
      delay(200);
      
      for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1)  {
            leds[whiteLed] = CRGB::Yellow;
            
      }
      FastLED.show();
      delay(200);

            
      for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1)  {
            leds[whiteLed] = CRGB::Black;

      }
      FastLED.show();
      delay(200);
      
      for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1)  {
            leds[whiteLed] = CRGB::Yellow;
            
      }
      FastLED.show();
      delay(400);
      
      nextPattern();
      
      x=0;
      
  }
  

}//end loop *****************************************************END LOOP***********

void nextPattern()
{
  int val;

  //patternaelter=patternalt;
  patternalt=gCurrentPatternNumber;
  gCurrentPatternNumber = random(0,9);
  
  //Serial.println(gCurrentPatternNumber);
  
  if(gCurrentPatternNumber==patternalt) {
    gCurrentPatternNumber=0;
  }

  detachInterrupt(digitalPinToInterrupt(3));
  zeit=millis();
  myDFPlayer.volume(16);
  val = analogRead(A0); 

  if(val>765) {

    if(gCurrentPatternNumber==0 || gCurrentPatternNumber==4) {
      randNumber = random(3,6);
      myDFPlayer.playFolder(15,randNumber);
    
    }
  
    else {
      randNumber = random(6,15);
      myDFPlayer.playFolder(15,randNumber);
    }
    
    
  }

  else if(val>510 && val<=765) {
      randNumber = random(1,3);
      myDFPlayer.playFolder(14,randNumber);
      zeit=zeit-700;
  }

  else if(val>255 && val<=510) {
      myDFPlayer.volume(24);
      randNumber = random(1,27);
      myDFPlayer.playFolder(16,randNumber);
  }
  
  
    
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);

  }
}

void Schimmer() {
  EVERY_N_MILLISECONDS( 20) {
    pacifica_loop();
    a = 1;
  }
}//end Schimmer

void Schimmer_blau() {
  EVERY_N_MILLISECONDS( 20) {
    pacifica_loop();
    a = 2;
  }
}//end Schimmer

void weiss() {

  EVERY_N_MILLISECONDS( 20 ) {
    for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1)  {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::White;

      FastLED.show();
      //delay(30);
    }
  }  
}//end weiss

void Fire2012()
{
  EVERY_N_MILLISECONDS( 50) {

    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for ( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for ( int k = NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if ( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160, 255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for ( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if ( gReverseDirection ) {
        pixelnumber = (NUM_LEDS - 1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
  }
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  EVERY_N_MILLISECONDS( 50 ) {
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( gHue + random8(64), 200, 255);
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 40;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void Lauflicht_horizontal()
{
  static uint8_t hue = 0;
  // First slide the led in one direction

  switch (state) {
      if (millis() <= 2500) currentTime = millis();
      if (digitalRead(2) == HIGH) goto OUT;
    case 1:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue++, 255, 255);
        FastLED.show();
        //fadeall();
        delay(30);
        if (digitalRead(2) == HIGH) goto OUT;
      }
      currentTime = millis();
      state = 2;
      break;

    case 2:
      if (millis() >= currentTime + 2000) state = 3;
      break;

    case 3:
      for (int i = (NUM_LEDS) - 1; i >= 0; i--) {
        leds[i] = CHSV(hue++, 255, 255);
        FastLED.show();
        //fadeall();
        delay(30);
        if (digitalRead(2) == HIGH) goto OUT;
      }
      currentTime = millis();
      state = 4;
      break;

    case 4:
      if (millis() >= currentTime + 2000) state = 1;
      break;

  }//endSwitch

  OUT:
  delay(1); 
}//end Lauflicht_hor


void Lauflicht_vertikal()
{

  static uint8_t hue = 0;
  // First slide the led in one direction

  switch (state) {
      if (millis() <= 1000) currentTime = millis();
      if (digitalRead(2) == HIGH) goto OUT;
    case 1:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue++, 255, 255);
        FastLED.show();
        //fadeall();
        delay(50);
        if (digitalRead(2) == HIGH) goto OUT;
      }
      currentTime = millis();
      state = 2;
      break;

    case 2:
      if (millis() >= currentTime + 1000) state = 3;
      break;

    case 3:
      for (int i = (NUM_LEDS) - 1; i >= 0; i--) {
        leds[i] = CHSV(hue++, 255, 255);
        FastLED.show();
        //fadeall();
        delay(50);
        if (digitalRead(2) == HIGH) goto OUT;
      }
      currentTime = millis();
      state = 4;
      break;

    case 4:
      if (millis() >= currentTime + 1000) state = 1;
      break;

  }//endSwitch
  OUT:
  delay(1);
}//end Lauflicht_ver



CRGBPalette16 pacifica_palette_1 =
{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50
};
CRGBPalette16 pacifica_palette_2 =
{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F
};
CRGBPalette16 pacifica_palette_3 =
{ 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
  0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF
};

void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
  sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
  sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
  sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

  // Clear out the LED array to a dim background blue-green
  if (a == 2) fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));
  // if (x == 2) fill_solid( leds, NUM_LEDS, CRGB( 0, 100, 0));
  //if (x == 3) fill_solid( leds, NUM_LEDS, CRGB( 139, 0, 0));
  //*********************************************************FADE-START*******************
  if (a == 1) {

    int warte = 8000;
    int Max = 70;

    switch (state2) {
      case 1:
        FadeG++;
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (FadeG >= Max) {
          state2++;
          currentTime = millis();
        }
        break;

      case 2:
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (millis() >= (currentTime + warte)) {
          state2++;
        }
        break;

      case 3:
        FadeB--;
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (FadeB <= 0) {
          state2++;
          currentTime = millis();
        }
        break;

      case 4:
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (millis() >= (currentTime + warte)) {
          state2++;
          break;
        }

      case 5:
        FadeR++;
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (FadeR >= Max) {
          state2++;
          currentTime = millis();
        }
        break;

      case 6:
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (millis() >= (currentTime + warte)) {
          state2++;
        }
        break;

      case 7:
        FadeG--;
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (FadeG <= 0) {
          state2++;
          currentTime = millis();
        }
        break;

      case 8:
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (millis() >= (currentTime + warte)) {
          state2++;
        }
        break;

      case 9:
        FadeB++;
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (FadeB >= Max) {
          state2++;
          currentTime = millis();
        }
        break;

      case 10:
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (millis() >= (currentTime + warte)) {
          state2++;
        }
        break;

      case 11:
        FadeR--;
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (FadeR <= 0) {
          state2++;
          currentTime = millis();
        }
        break;

      case 12:
        fill_solid( leds, NUM_LEDS, CRGB( FadeR, FadeG, FadeB));
        if (millis() >= (currentTime + warte)) {
          state2 = 1;
        }
        break;

    } //end switch
  } //end if
  //*********************************************************FADE-STOPP*******************

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0 - beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10, 38), 0 - beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10, 28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );

  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if ( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145);
    leds[i].green = scale8( leds[i].green, 200);
    leds[i] |= CRGB( 2, 5, 7);
  }
}
