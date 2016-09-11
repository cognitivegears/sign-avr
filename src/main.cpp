#include <Adafruit_NeoPixel.h>

// five strips, one each for F O R G E
#define STARTPIN 6
#define POWERTAILPIN 12
#define REEDSWITCHPIN 11

const int NUM_PIXELS  = 120; // 120 pixels per strip
const int NUM_STRIPS  = 5;   // one strip per letter
const int FULL_BRIGHT = 255; // 0-255
const int OFF         = 0;   // 0-255

// Letter O is one short pixels (oops), so we have to keep a list per letter
int numLed[] = {NUM_PIXELS, NUM_PIXELS - 1, NUM_PIXELS, NUM_PIXELS, NUM_PIXELS};


// We setup 5 strips, one for each letter.
Adafruit_NeoPixel strip[NUM_STRIPS] = {
  Adafruit_NeoPixel(numLed[0],STARTPIN, NEO_GRB + NEO_KHZ800),   // F
  Adafruit_NeoPixel(numLed[1],STARTPIN+1, NEO_GRB + NEO_KHZ800), // O
  Adafruit_NeoPixel(numLed[2],STARTPIN+2, NEO_GRB + NEO_KHZ800), // R
  Adafruit_NeoPixel(numLed[3],STARTPIN+3, NEO_GRB + NEO_KHZ800), // G
  Adafruit_NeoPixel(numLed[4],STARTPIN+4, NEO_GRB + NEO_KHZ800)  // E
};

// Set up some global vars
int redColor   = OFF;
int greenColor = OFF;
int blueColor  = OFF;
int currentLetterNum  = 0;

// Serial variables
char cmd = NULL;
char firstChar = NULL;

void setup()
{
  // Start serial at 38400 baud
  Serial.begin(38400);
  for(int i=0; i<NUM_STRIPS;i++) {
    strip[i].begin();
    strip[i].setBrightness(FULL_BRIGHT);
  }
}

void setStripColor() {
  for(int i=0; i<numLed[currentLetterNum]; i++) {
    strip[currentLetterNum].setPixelColor(i, redColor, greenColor, blueColor);
  }
  strip[currentLetterNum].show();
}
void allOn() {
  redColor = FULL_BRIGHT; greenColor = FULL_BRIGHT; blueColor = FULL_BRIGHT;
  for(int i=0; i<NUM_STRIPS;i++) {
    currentLetterNum = i;
    setStripColor();  
  }
}

void allOff() {
  redColor = OFF; greenColor = OFF; blueColor = OFF;
  for(int i=0; i<NUM_STRIPS;i++) {
    currentLetterNum = i;
    setStripColor();  
  }
}

void doFlash(int numTimes,int myDelay) {
 for(int i=0; i<numTimes;i++) {
    allOn();
    delay(myDelay);
    allOff();
    delay(myDelay);
  }
}


void doMarquee() {
  redColor = FULL_BRIGHT;
  greenColor = FULL_BRIGHT;
  blueColor = FULL_BRIGHT;

  for(int i=0; i<NUM_STRIPS;i++) {
   currentLetterNum = i;
   setStripColor();  
   delay(1000);
  }
  doFlash(3,500);
}

void runSpecial(int valType) {
  switch(valType)
  {
    case 0: doMarquee();break;
    case 1: doFlash(10,500); break;
    case 2: allOn(); break;
    case 3: allOff(); break;
  }

  // Reset lights on current letter
  for(int i=0; i<numLed[currentLetterNum]; i++) {
    strip[currentLetterNum].setPixelColor(i, redColor, greenColor, blueColor);
  }
  strip[currentLetterNum].show();
}



void loop()
{
  if(!Serial.available()) {
    return;
  }

  char readChar = Serial.read(); 

  // Reset whenever an end of command character ";" is read
  if (readChar == ';') {
    firstChar = NULL;
    cmd = NULL;
    return;
  }

  // First read a command, then a value.  If a command hasn't been read yet, 
  // then read it and store for next iteration of the loop.
  if (!cmd) {
    cmd = readChar;
  }
  else {
    // values are two characters, read the first character and store for next
    // iteration of the loop if one hasn't already been read
    if (!firstChar) {
      firstChar = readChar;
    }
    else {
      char mybuf [3];
      sprintf(mybuf,"%c%c",firstChar,readChar);
      int value = strtol(mybuf,NULL,16);

      // After getting the command and value, determine what to execute
      switch(cmd)
      {
        case 'r': redColor = value; setStripColor();break;
        case 'g': greenColor = value;  setStripColor();break;
        case 'b': blueColor = value;  setStripColor();break;
        case 'l': currentLetterNum = value;  break;
        case 's': runSpecial(value); break;
      }
      // After running command, clear it before next iteration
      cmd = NULL;
      firstChar = NULL;
    }
  }
}

