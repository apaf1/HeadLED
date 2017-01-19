// Libraries
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__ //for neopixels
  #include <avr/power.h>
#endif

//Arduino pin definitions
int bRightPin = 8;
#define pixelsPIN  6
int arduinoLed = 13;

//Neopixel definitions
int NUMPIXELS=34;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, pixelsPIN, NEO_GRB + NEO_KHZ800);

//program state values
int modesChoice = 0;
int modesChoiceMax = 6;
boolean modeChanged = false;
boolean lowBrightness = true;

//define button values
long waitButton;
boolean dobbeltklikkValue = false;
boolean bRightValue =false;
boolean bRightAction = false;
boolean bRightLast = false;


//values used in pixel modes

//used in ledPulse()
float MAXPULSERATE = 0.003;
float MINPULSERATEFACTOR = 10;
float ledPulseRate[34];
float ledPulseState[34];
int LARGE_INT = 3.4028235E+38;

//used in standingWave()
int rWave[34];
float rbWave[34];

void setup() {
  //Serial.begin(9600); //for troubleshooting, talking to computer
  pinMode(bRightPin,INPUT);
  pinMode(arduinoLed,OUTPUT);
  digitalWrite(arduinoLed,digitalRead(bRightPin)); //Show buttonstate on arduinoLed
  
  pixels.begin(); //initialize neopixels
  
  lowBrightness=true;
  pixels.setBrightness(25); //start with low brightnes on LED
  
  randomSeed(millis());

  //Initialize array for ledPulse() function
  for(int i=0;i<NUMPIXELS;i++){
    ledPulseRate[i]=(( (float) (random((MAXPULSERATE/MINPULSERATEFACTOR)*LARGE_INT,MAXPULSERATE*LARGE_INT*2)))/(float) LARGE_INT )-(MAXPULSERATE/2);
    ledPulseState[i] = (float)  random((MAXPULSERATE*LARGE_INT),(1-MAXPULSERATE)*LARGE_INT)/(float)LARGE_INT;
  }

  //initialize array for standingWave() function
  rbWave[0]=0;
  for(int i=1;i<NUMPIXELS;i++){
    rbWave[i]=rbWave[i-1]+(6*3.14)/NUMPIXELS;
  }
}

//Main loop that runs program
void loop() {
  checkButtons();
  updateState();

  if(modesChoice == 0){
    ledPulse();
  }else if(modesChoice ==1){
    hjul();
  }else if(modesChoice ==2){
    rainbowCycle(0);
  }else if(modesChoice == 3){
    wave(100,255,0,255);
  }else if(modesChoice == 4){
    standingWave();
  }else if(modesChoice == 5){
    three(200);
  }else if(modesChoice == 6){
    blinking();
  }
  
  modeChanged = false;
}



//////////////////////////////////////////////////////////////////
//
// CONTROL CODE
//
//////////////////////////////////////////////////////////////////

//Checks buttons, but does not do anything with state of program
//Changes are done in updateState()
void checkButtons(){
  
  bRightValue = digitalRead(bRightPin);
  digitalWrite(arduinoLed,bRightValue);
  boolean buttonPressed = false;

  //if button-value has changed and some time has passed since last buttonpress, 
  //then make button action true and do something in updateState()
  //Also checks for double press of button in dobbeltklikk() function (norwegian for double press)
  if(bRightLast != bRightValue){
    bRightLast = bRightValue;
    if(bRightValue==HIGH && millis()>waitButton){
      dobbeltklikk();
      if(!dobbeltklikkValue){
        bRightAction=true;
        buttonPressed = true;
      }
    }else{
      buttonPressed = false;
    }
  }
  //wait-time to fix double jumping button
  if(buttonPressed){
    waitButton = millis() + 200;
  }
  
}

//function for checking if there is a double pressing of button
void dobbeltklikk(){
  delay(100);
  long waiting = millis()+1000;
  while(waiting>millis()){
    bRightValue = digitalRead(bRightPin);
    if(bRightLast != bRightValue){
      bRightLast = bRightValue;
      if(bRightValue==HIGH){
        dobbeltklikkValue=true;
        break;
      }
  }
  }
}

boolean updateState(){

  if(bRightAction){
    modeChanged = true;
    if(modesChoice>=modesChoiceMax){
        modesChoice = 0;
      }else{
        modesChoice = modesChoice + 1;
      }
  }

  if(dobbeltklikkValue){
    if(!lowBrightness){
      pixels.setBrightness(25);
      pixels.show();
      lowBrightness=true;
    }else{
      pixels.setBrightness(255);
      pixels.show();
      lowBrightness=false;
    }
  }

  clearButtonActionState(); 

}

void clearButtonActionState(){
  bRightAction = false; 
  dobbeltklikkValue = false;
}

//Pause while also updating state by checking buttons
boolean pause(int milliSeconds){
  long start = millis();
    checkButtons();
    updateState();
    while(millis()<(start+milliSeconds)){
      if(modeChanged){
        break; //end all pauses so program goes to next mode
      }
      checkButtons();
      updateState();
    }
}

//////////////////////////////////////////////////////////////////
//
// LED ANIMATION CODE
//
//////////////////////////////////////////////////////////////////

//turns all pixels off, but does not update the LEDs with state
void off(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(0,0,0));
  }
}

//Turns all led blue
void blueLight(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(0,0,250));
    pixels.show();
  }
  pause(100);
}

//Turn LEDs sequentially a color, then wait. Repeat with different color. 
void pixeltest(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(250,0,0));
    pixels.show();
    pause(10);
  }
  pause(500);

  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(0,250,0));
    pixels.show();
    pause(10);
  }
  pause(500);

  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color(0,0,250));
    pixels.show();
    pause(10);
  }
  pause(500);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}



// Gives a rainbow
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    pause(wait);
    if(modeChanged){
      break;
    }
  }
}

//Shows sine wave that moves with rate x. 
//rbWave is the base wave that is constant
//rWave is the brightness of the pixel. 
//you can also change the wavelength but I don't remember how now
void standingWave(){
  float x=0;
  while(!modeChanged){
    for(int i=0;i<NUMPIXELS;i++){
      float a=255*((sin(rbWave[i]+x)));
      if(a<0){
        a=0;
      }
      rWave[i]=a;
      

    }
    x+=0.03;
    
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i,pixels.Color(rWave[i],0,0));
    }
    pixels.show();
    pause(10);
  }
}

//party mode!
void blinking(){
  int color = random(0,7);
  if(color==0){
    oneBlink(250,0,0);
  }else if(color==1){
    oneBlink(0,250,0);
  }else if(color==2){
    oneBlink(0,0,250);
  }else if(color==3){
    oneBlink(150,0,150);
  }else if(color==4){
    oneBlink(0,150,150);
  }else if(color==5){
    oneBlink(150,150,0);
  }else if(color>=6){
    oneBlink(150,150,1500);
  }
  
}

void oneBlink(int r, int g, int b){
  for(int i=0;i<3;i++){
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i,pixels.Color(r,g,b));
      }
      pixels.show();
      pause(70);
      off();
      pixels.show();
      pause(70);
    }
}

//Light up every third pixel in different color
void three(int waitTime){
  for(int j=0;j<3;j++){
    for(int i=0;i<NUMPIXELS;i++){
      if(i%3==j){
        pixels.setPixelColor(i,pixels.Color(0,0,255));
      }else {
        pixels.setPixelColor(i,pixels.Color(0,255,0));
      }
    }
    pixels.show();
    pause(waitTime);
  }
}

//Shitty wave function, standingWave() is cooler
void wave (int waitTime,double r,double g, double b){
  float x=0.1;
  int i=1;
  int j=2;
  int k=3;
  int l=4;
  int m=5;
  int n=6;
  int o=7;
  int p=8;
  int q=9;
  while(!modeChanged){
    
    off();
    pixels.setPixelColor(i,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    x=0.2;
    pixels.setPixelColor(j,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    x=0.4;
    pixels.setPixelColor(k,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    x=0.6;
    pixels.setPixelColor(l,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    pixels.setPixelColor(m,pixels.Color((int) (r),(int) (g),(int) (b)));
    x=0.6;
    pixels.setPixelColor(n,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    x=0.4;
    pixels.setPixelColor(o,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    x=0.2;
    pixels.setPixelColor(p,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    x=0.1;
    pixels.setPixelColor(q,pixels.Color((int) (r*x),(int) (g*x),(int) (b*x)));
    i-=1;
    j-=1;
    k-=1;
    l-=1;
    m-=1;
    n-=1;
    o-=1;
    p-=1;
    q-=1;

    if(i<0){
      i=NUMPIXELS+1;
    }
    if(j<0){
      j=NUMPIXELS+1;
    }
    if(k<0){
      k=NUMPIXELS+1;
    }
    if(l<0){
      l=NUMPIXELS+1;
    }
    if(m<0){
      m=NUMPIXELS+1;
    }
    if(n<0){
      n=NUMPIXELS+1;
    }
    if(o<0){
      o=NUMPIXELS+1;
    }
    if(p<0){
      p=NUMPIXELS+1;
    }
    if(q<0){
      q=NUMPIXELS+1;
    }

    pixels.show();
    
    pause(waitTime);
  }
  
}


//cycles through the colors, all LEDs in the same color
void hjul(){
  for(int j=0;j<255;j++){
    for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i,Wheel(j));
        pause(1);
    }
    pixels.show();
    if(modeChanged){
      break;
    }
  }
}

//Each LED change color with a random rate
//The rate of change sets to a new random value each time the brightness is below a threshold
void ledPulse(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,pixels.Color((int) (ledPulseState[i]*0),(int) (ledPulseState[i]*0),(int) (ledPulseState[i]*255)));
    //Serial.print((ledPulseState[i]));
    //Serial.print(" ");
  } 
  //Serial.println();
  pixels.show();
  
  for(int i=0;i<NUMPIXELS;i++){
    
      if(ledPulseState[i]>=(1-MAXPULSERATE)){
        ledPulseRate[i]=-ledPulseRate[i];
      
      }else if(ledPulseState[i]<=(0+MAXPULSERATE)){
        ledPulseRate[i]=(( (float) (random((MAXPULSERATE/MINPULSERATEFACTOR)*LARGE_INT,MAXPULSERATE*LARGE_INT*2)))/(float) LARGE_INT )-(MAXPULSERATE/2);
      }
    
      ledPulseState[i]=ledPulseState[i]+ledPulseRate[i];
    
  } 
}



