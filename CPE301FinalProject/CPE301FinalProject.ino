/*
 *  Name: Michael Pittenger & German Gomez
 *  Assignment: Final Project
 *  Class: CPE301 Fall 2022
 *  Date: 11/18/2022
 */

#define RDA 0x80
#define TBE 0x20

// UART Pointers
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
// GPIO Pointers
volatile unsigned char *portB     = (unsigned char*) 0x23; 
volatile unsigned char *portDDRB  = (unsigned char*) 0x24; 
// Timer Pointers
volatile unsigned char *myTCCR1A  = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B  = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C  = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1  = (unsigned char *) 0x6F;
volatile unsigned char *myTIFR1   = (unsigned char *) 0x36;
volatile unsigned int  *myTCNT1   = (unsigned  int *) 0x84;

int waterThresh;
int waterLevel;
int tempThresh;
int temp;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  if(waterLevel < waterThresh){
    error();
  }
  if(temp > tempThresh){
    isRunning();
  }
  else{
    idle();
  }

}

//vent position adjustable in all states except for Error

//functions

void fan(_Bool var){
  if(var == true){
    //fan on
  }
  else{
    //fan off
  }
}

void LED(int var){
  switch(var){
    case 1:
      //turn blue on
    case 2:
      //turn red on
    case 3:
      //turn yellow on
    case 4:
      //turn green on
    break;
  }
}

//Disabled
  //fan off
  //yellow LED ON
  //start --> Idle

void disabled(){
  fan(false);
  LED(3);
}

//Idle
  //fan off
  //green LED ON
  //temp and humidity displayed on LCD
  //stop --> Disabled
  //temp > threshold --> Running
  //water level =< threshold --> Error

void idle(){
  fan(false);
  displayLCD();
  LED(4);
}

//Running
  //on entry: start fan motor
  //on exit: stop fan motor
  //blue LED ON
  //temp and humidity displayed on LCD
  //temp =< threshold --> Idle
  //stop --> Disabled
  //water level < threshold --> Error

void isRunning(){
  fan(true);
  LED(1);
}

//Error
  //display error message: "Water level is too low"
  //red LED ON

void error(){
  LED(2);
}

//display temp and humidity

void displayLCD(){
  
}



  
