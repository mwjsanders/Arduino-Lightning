// Do not remove the include below
#include "lightning.h"
#include <LiquidCrystal.h>
#include <SPI.h>
#include <IRremote.h>


/*****************************************************
 * main project fot leight control
 *   -
 *
 *
 ******************************************************/
/*
 * used pin numbers
 * LED default led on the arduino board
 */
const int PN_LED = 13;
/* LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 */
const int PN_LCD_RS = 12;
const int PN_LCD_EN = 11;
const int PN_LCD_D4 =  5;
const int PN_LCD_D5 =  4;
const int PN_LCD_D6 =  3;
const int PN_LCD_D7 =  2;
/* LIGHTS clock to digital pin 10
 * LIGHTS latch to digital pin 9
 * LIGHTS data to digital pin 7
 */
const int PN_LGH_CL = 10;
const int PN_LGH_LT =  9;
const int PN_LGH_D0 =  7;
/* IR input to pin 6
 */
const int PN_IR_RCV =  6;

enum keys{KEY_BACK,KEY_BACK2,KEY_FORW,KEY_FORW2,KEY_PAUZE,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0};
enum effects{EFF_LOOP_1,EFF_LOOP_2,EFF_LOOP_3,EFF_LOOP_4,EFF_BLINK,EFF_RAND};

LiquidCrystal display(PN_LCD_RS, PN_LCD_EN, PN_LCD_D4, PN_LCD_D5, PN_LCD_D6, PN_LCD_D7);

IRrecv irRecv(PN_IR_RCV);
decode_results results;

// effects
int (*nextEffectStep)();
int effect = EFF_LOOP_1;
unsigned short swLights;
// loop
int loop_dir=1;
int loop_step=1;
int loop_delay=500;

 // initialize the library with the numbers of the interface pins
void setup(){
  // setup display
  Serial.begin(9600);
  display.begin(16, 2);
  display.print("** Lightning! **");
  // display.setCursor(0,1);
  display.print("*__________setup");

  // setup leights
  pinMode(PN_LGH_LT, OUTPUT);
  pinMode(PN_LGH_CL, OUTPUT);
  pinMode(PN_LGH_D0, OUTPUT);
  // output led
  pinMode(PN_LGH_D0, OUTPUT);
  // setupLoop 1
  setupLoop1();
  irRecv.enableIRIn();
}

void loop(){
  procesInput();
  delayTime = updateLights();
  updateDisplay();
  delay(delayTime);
}

/*****************************************************
 * IR functions
 ******************************************************/
void procesInput(){
  if (irRecv.decode(&results)) // have we received an IR signal?
  {
    Serial.println(results.value, HEX);
    switch(translateIR()){
      case KEY_BB:
        sw_dir=-1;
        break;
      case KEY_FF:
        sw_dir=1;
        break;
    }
    irRecv.resume(); // receive the next value
  }
}

int translateIR()
{
  switch(results.value)
  {
    case 0xFD20DF:
      return KEY_BB;
    case 0xFD609F:
      return KEY_FF;
    default:
      return 0;
  }
}


/*****************************************************
 * LCD Display functions
 ******************************************************/


void updateDisplay(){
  char ld[16] ="";
  unsigned short s=0x0001;
  for (int i=0; i<16; i++){
    if (swLights&s){
      strcat(ld,"*");
    }else{
      strcat(ld,"-");
    }
    s=s*2;
  }
  display.setCursor(0,1);
  display.print(ld);

  Serial.println(ld);
}



/*****************************************************
 * Lights functions
 ******************************************************/
int updateLights(){
  rotate(sw_dir);
  digitalWrite(PN_LGH_LT, LOW);
  shiftOut(PN_LGH_D0, PN_LGH_CL, MSBFIRST, lowByte(swLights));
  shiftOut(PN_LGH_D0, PN_LGH_CL, MSBFIRST, highByte(swLights));
  digitalWrite(PN_LGH_LT, HIGH);
  return 500;
}

/******************************************************
 * loop effects
 */
void setupLoop1(){
  swLights = 0x0001;
  loop_step = 1;
  loop_delay = 500;
}

void setupLoop2(){
  swLights = 0x0101;
  loop_step = 1;
  loop_delay = 500;
}

void setupLoop3(){
  swLights = 0x1111;
  loop_step = 1;
  loop_delay = 500;
}

void setupLoop(unsigned short lights){
  swLights=lights;
  effectNextStep=&effLoopNext();
}

int effLoopNext(){
	rotate(loop_dir*loop_step);
	return loop_delay;
}

void rotate(short i)
{
  if (i>0) {
    swLights= (swLights << i) | (swLights >> (16 - i));
  }
  else {
    swLights= (swLights >> -i) | (swLights << (16 + i));
  }
}
