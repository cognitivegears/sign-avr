#include <Adafruit_NeoPixel.h>

// five strips, one each for F O R G E
#define STARTPIN 6
#define POWERTAILPIN 12
#define REEDSWITCHPIN 11

// O is one short pixels, so 119
int numLed[] = {120, 119, 120, 120, 120};

Adafruit_NeoPixel strip[5] = {
  Adafruit_NeoPixel(numLed[0],STARTPIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(numLed[1],STARTPIN+1, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(numLed[2],STARTPIN+2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(numLed[3],STARTPIN+3, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(numLed[4],STARTPIN+4, NEO_GRB + NEO_KHZ800)
};

int numStrips = sizeof(numLed) / sizeof(int);

// Set up some global vars
int redColor = 0;
int greenColor = 0;
int blueColor = 0;
int letterNum = 0;
int gearStop = 0;

// Serial variables
char cmd = NULL;
char firstChar = NULL;

void setup()
{
  // Start serial at 9600 baud
  Serial.begin(9600);
  for(int i=0; i<numStrips;i++) {
    strip[i].begin();
    strip[i].setBrightness(255);
  }
  pinMode(POWERTAILPIN, OUTPUT);
  pinMode(REEDSWITCHPIN, INPUT_PULLUP);
}
void gearOn() {
 gearStop = 0;
 digitalWrite(POWERTAILPIN, HIGH);
}
void gearOff() {
 gearStop = 1; 
}

void setStripColor() {
  for(int i=0; i<numLed[letterNum]; i++) {
    strip[letterNum].setPixelColor(i, redColor,greenColor,blueColor);
  }
  strip[letterNum].show();
  strip[letterNum].show();
  strip[letterNum].show();
  strip[letterNum].show();
}

void doFlash(int numTimes,int myDelay) {
 for(int i=0; i<numTimes;i++) {
   redColor = 255; greenColor = 255; blueColor = 255;
   for(int i=0; i<numStrips;i++) {
    letterNum = i;
    setStripColor();  
    delay(myDelay);
   }
   redColor = 0; greenColor = 0; blueColor = 0;
   for(int i=0; i<numStrips;i++) {
    letterNum = i;
    setStripColor();  
    delay(myDelay);
   }
 }
}

void allOn() {
   redColor = 255; greenColor = 255; blueColor = 255;
   for(int i=0; i<numStrips;i++) {
    letterNum = i;
    setStripColor();  
   }
}
void allOff() {
   redColor = 0; greenColor = 0; blueColor = 0;
   for(int i=0; i<numStrips;i++) {
    letterNum = i;
    setStripColor();  
   }
}

void doMarquee() {
  redColor = 255;
  greenColor = 255;
  blueColor = 255;
  for(int i=0; i<numStrips;i++) {
   letterNum = i;
   setStripColor();  
   delay(1000);
  }
  doFlash(3,500);
}

void runSpecial(int valType) {
  switch(valType)
  {
   case '00': doMarquee();break;
   case '01': doFlash(10,500); break;
   case '02': gearOn();break;
   case '03': gearOff();break;
   case '04': break;
  }
 
  for(int i=0; i<numLed[letterNum]; i++) {
    strip[letterNum].setPixelColor(i, redColor,greenColor,blueColor);
  }
  strip[letterNum].show();
}



void loop()
{
// allOn();
// delay(1000);
// allOff();
// delay(1000);
// return;
  if(gearStop == 1) {
   if(digitalRead(REEDSWITCHPIN) == LOW) {
     digitalWrite(POWERTAILPIN, LOW);
  } 
}
else {
gearOn();
}
 
  if(!Serial.available()) {
    return;
  }
 char readChar = Serial.read(); 
 if (readChar == ';') {
  firstChar = NULL;
  cmd = NULL;
  return;
 }

 if (!cmd) {
  cmd = readChar;
 }
 else {
   if (!firstChar) {
    firstChar = readChar;
   }
   else {
    char mybuf [3];
    sprintf(mybuf,"%c%c",firstChar,readChar);
    int value = strtol(mybuf,NULL,16);
    switch(cmd)
   {
    case 'r': redColor = value; setStripColor();break;
    case 'g': greenColor = value;  setStripColor();break;
    case 'b': blueColor = value;  setStripColor();break;
    case 'l': letterNum = value;  break;
    case 's': runSpecial(value); break;
//    case 's': gearOff(); break;
   }
   cmd = NULL;
   firstChar = NULL;
   }
 }
}

