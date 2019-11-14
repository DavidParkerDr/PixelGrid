#include <Adafruit_NeoPixel.h>
#include "Pixel_Grid.h"
#include "LCD_Panel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2

#define PIXEL_BUFFER_SIZE 258
#define NUM_ROWS 20
#define NUM_COLUMNS 10

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_BUFFER_SIZE, PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(14, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
int16_t currentIndex = 0;
uint16_t tailLength = 25;
uint32_t colour;
uint16_t brightness;
int16_t brightnessDirection = 1;
uint16_t brightnessStep = 10;

bool pulse = true;

uint32_t colours[9];
uint32_t randomColours[PIXEL_BUFFER_SIZE];
const int ledPin = 2;

uint32_t pixelBuffer[PIXEL_BUFFER_SIZE];
uint16_t pixelGridConversionTable[PIXEL_BUFFER_SIZE];

int count;



unsigned long previousTime;
unsigned long timePassed;
unsigned long timePassed2;
unsigned long timePassed3;
unsigned long timePassed4;

class Shape {
  private:
    String mName;
    uint32_t mColour;
  public :  
    Shape(String pName, uint32_t pColour) {
      this->mName = pName;
      this->mColour = pColour;
    }

  String getName() {
    return this->mName;
  }

  uint32_t getColour() {
    return this->mColour;
  }
 static Shape* makeI(){
    return new Shape("I", strip.Color(0, 230, 254)); // light blue
  }
  static Shape* makeJ(){
    return new Shape("J", strip.Color(24, 1, 255)); // dark blue
  }
  static Shape* makeL(){
    return new Shape("L", strip.Color(255,115,8)); // orange
  }
  static Shape* makeO(){
    return new Shape("O", strip.Color(255, 222, 0)); // yellow
  }
  static Shape* makeS(){
    return new Shape("S", strip.Color(102,253,0)); // green
  }
  static Shape* makeT(){
    return new Shape("T", strip.Color(184,2,253)); // purple
  }
  static Shape* makeZ(){
    return new Shape("Z", strip.Color(254,16,20)); // red
  }
  
};

class BagOfSeven {
  private:
  static const uint16_t cSize = 7;
  Shape* mBag[cSize];
  uint16_t mRemainingShapes;
    
  public:
  BagOfSeven(){
    refreshBag();
  }
  void refreshBag() {
    mBag[0] = Shape::makeI();
    mBag[1] = Shape::makeJ();
    mBag[2] = Shape::makeL();
    mBag[3] = Shape::makeO();
    mBag[4] = Shape::makeS();
    mBag[5] = Shape::makeT();
    mBag[6] = Shape::makeZ();   
    mRemainingShapes = cSize; 
  }
  Shape * getNextShape() {
    uint16_t shapeIndex = random(0, mRemainingShapes);
    Shape* shape = mBag[shapeIndex];
    mRemainingShapes--;
    for(uint16_t i = shapeIndex; i < mRemainingShapes; i++) {
      mBag[i] = mBag[i+1];
    }
    if (mRemainingShapes <= 0) {
      refreshBag();
    }
    return shape;
  }

};


 




Shape * _currentShape;
LCD_Panel * lcdPanel;
Pixel_Grid * pixelGrid;
uint32_t currentDigit;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
 pinMode(PIN, OUTPUT);
  

count = 0;


Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  initialiseColours();
  lcdPanel = new LCD_Panel(&strip, 216, 6, strip.Color(255,0,0));
  pixelGrid = new Pixel_Grid(&strip, 0, 20, 10);
  currentDigit = 999990;
  lcdPanel->changeNumber(currentDigit);
  initialiseRandomColours(pixelGrid);
  pixelGrid->render();
  lcdPanel->render();
  strip.show();
  return;
  colour = getRandomColour();

//initialisePixelGridConversionTable();

  previousTime = micros();
  timePassed = 0;
  BagOfSeven* bag = new BagOfSeven();
  Serial.println("setup complete");
  for(uint16_t i = 0; i < 15; i++) {
    _currentShape = bag->getNextShape();
    //Serial.println(_currentShape->getName());
    delete _currentShape;
  }

  
  
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  unsigned long currentTime = micros();
  unsigned long deltaTime = currentTime - previousTime;
  
 // Serial.println("loop");
 // Serial.println(deltaTime);
 // tailAnimation(deltaTime);
 //
 // return;
 
  if(pulse)
  {
    //pulseAnimation(deltaTime);
   tailAnimation(deltaTime);
  }
  else
  {
  
    proximityAnimation(deltaTime);
  }
  
  countdownAnimation(deltaTime);
  pixelGrid->render();
  lcdPanel->render();
  strip.show();
  
previousTime = currentTime;
}
void countdownAnimation(unsigned long deltaTime)
{
  unsigned long oneSecond = 1000000;
  unsigned long timeBetweenUpdates = oneSecond /4;
   timePassed = timePassed + deltaTime;
  
  if(timePassed > timeBetweenUpdates)
  {    
    timePassed = 0;
    
    if(currentDigit < 999999) {
      currentDigit++;      
    }
    else {
      currentDigit = 0;
    }  
    lcdPanel->changeNumber(currentDigit);  
    lcdPanel->render();
    //Serial.println(currentDigit);
  }
}

void initialiseRandomColours(Pixel_Grid * pPixelGrid) {
  for(uint16_t row=0; row<NUM_ROWS; row++) {
    for(uint16_t column=0; column<NUM_COLUMNS; column++) {
      pPixelGrid->setGridCellColour(row, column, colours[random(2,9)]);
    }
  }
}



void initialiseColours() {
    uint32_t black = strip.Color(0, 0, 0);
    uint32_t white = strip.Color(255, 255, 255); 
    uint32_t light_blue = strip.Color(0, 230, 254);
    uint32_t dark_blue = strip.Color(24, 1, 255);
    uint32_t orange = strip.Color(255,115,8);
    uint32_t yellow = strip.Color(255, 222, 0);
    uint32_t green = strip.Color(102,253,0);
    uint32_t red = strip.Color(254,16,20);
    uint32_t purple = strip.Color(184,2,253);
    colours[0] = black;
    colours[1] = white;
    colours[2] = light_blue;
    colours[3] = dark_blue;
    colours[4] = orange;
    colours[5] = yellow;
    colours[6] = green;
    colours[7] = red;
    colours[8] = purple;
}
void proximityAnimation(unsigned long deltaTime)
{
  unsigned long oneSecond = 1000000;
  unsigned long timeBetweenUpdates = oneSecond;
timePassed2 = timePassed2 + deltaTime;
  
  if(timePassed2 > timeBetweenUpdates)
  {    
    timePassed2 = 0;
    pulse = true;
    Serial.println("switchProximity");
  }
}

void pulseAnimation(unsigned long deltaTime)
{
  unsigned long oneSecond = 1000000;
  unsigned long timeBetweenUpdates = oneSecond / 100;
timePassed3 = timePassed3 + deltaTime;
  if(timePassed3 > timeBetweenUpdates)
  {
    brightness = brightness + (brightnessDirection * brightnessStep);
    if(brightness >= 255 || brightness <= 0)
    {
      brightnessDirection = -brightnessDirection;
      
    }
    if(brightness <= 0)
      {
        
        pulse = false;
        pixelGrid->clear();
        initialiseRandomColours(pixelGrid);
        Serial.println("switchPulse");
        
      }
      else {
        for(uint16_t row=0; row<NUM_ROWS; row++) {
          for(uint16_t column=0; column<NUM_COLUMNS; column++) {
            uint32_t colour = pixelGrid->getGridCellColour(row, column);
            uint32_t newColour = adjustColourBrightness(colour, brightness);
            pixelGrid->setGridCellColour(row, column, newColour);
          }
        }
    
      }
    timePassed3 = 0;
  }
}

void tailAnimation(unsigned long deltaTime)
{
  unsigned long oneSecond = 1000000;
  unsigned long timeBetweenUpdates = oneSecond / 100;
timePassed4 = timePassed4 + deltaTime;
  if(timePassed4 > timeBetweenUpdates)
  {
    if(currentIndex == 0)
    {
      colour = colours[random(2,9)];
      pulse = false;
      pixelGrid->clear();
      initialiseRandomColours(pixelGrid);
      Serial.println("switchTail");
    }
    else {
      pixelGrid->clear();
      showTail(colour, 50);      
    }
    currentIndex = currentIndex + 1;
    currentIndex = correctIndex(currentIndex);
    timePassed4 = 0;
  }
}

uint32_t getRandomColour()
{
  uint8_t r = random(0,255);
  uint8_t g = random(0,255);
  uint8_t b = random(0,255);
  uint32_t newColour = strip.Color(r, g, b);  
  return newColour;
}


uint32_t adjustColourBrightness(uint8_t r, uint8_t g, uint8_t b, uint16_t brightness) 
{
  uint8_t newRed = brightness * r / 255;
  uint8_t newGreen = brightness * g / 255;
  uint8_t newBlue = brightness * b / 255;
  uint32_t newColour = strip.Color(newRed, newGreen, newBlue);  
  return newColour;
}

uint32_t adjustColourBrightness(uint32_t colour, uint16_t brightness) 
{
  uint8_t r = getPixelColour(colour,0);
  uint8_t g = getPixelColour(colour,1);
  uint8_t b = getPixelColour(colour,2);
  uint32_t newColour = adjustColourBrightness(r, g, b, brightness);
  return newColour;
}

uint8_t getPixelColour(uint32_t mergedColour, uint8_t index)
{
  if(index == 0)
  {
    return getRedComponent(mergedColour);
  } 
  else if(index == 1)
  {
    return getGreenComponent(mergedColour);
  }
  else if(index == 2)
  {
    return getBlueComponent(mergedColour);
  }
  return 0;
}

uint8_t getRedComponent(uint32_t mergedColour)
{
  uint8_t r = mergedColour >> 16;
  return r;
}

uint8_t getGreenComponent(uint32_t mergedColour)
{
  uint8_t g = mergedColour >> 8;
  return g;
}

uint8_t getBlueComponent(uint32_t mergedColour)
{
  uint8_t b = mergedColour;
  return b;
}

int16_t correctIndex(int16_t index)
{
  int16_t numPixels = pixelGrid->numPixels();
  if(index >= numPixels)
  {
    index = index - numPixels;
  }
  else if(index < 0)
  {
    index = index + numPixels;
  }
  return index;
}

// Turn all the dots off
void showTail(uint32_t colour, uint8_t wait) {
  uint16_t brightness = 255;
  
  for(uint16_t i=0; i<tailLength; i++) {
    int16_t ledIndex = currentIndex - i;
    ledIndex = correctIndex(ledIndex); 
    brightness = brightness / 2;  
    uint32_t newColour = adjustColourBrightness(colour, brightness);
    pixelGrid->setGridCellColour(ledIndex, newColour);
  }

}



