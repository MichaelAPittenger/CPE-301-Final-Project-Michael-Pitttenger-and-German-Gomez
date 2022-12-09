/*
    Name: Michael Pittenger & German Gomez
    Assignment: Final Project
    Class: CPE301 Fall 2022
    Date: 11/18/2022
*/

#define RDA 0x80
#define TBE 0x20

// UART Pointers
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
// GPIO Pointers
volatile unsigned char* port_b = (unsigned char*) 0x25;
volatile unsigned char* ddr_b  = (unsigned char*) 0x24;
volatile unsigned char* pin_b  = (unsigned char*) 0x23;
// Timer Pointers
volatile unsigned char *myTCCR1A  = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B  = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C  = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1  = (unsigned char *) 0x6F;
volatile unsigned char *myTIFR1   = (unsigned char *) 0x36;
volatile unsigned int  *myTCNT1   = (unsigned  int *) 0x84;

byte in_char;

//initializing functions, would not run otherwise
int waterThresh;
int waterLevel;
int tempThresh;
int temp;

void fan(_Bool);
void LED(int);

void displayLCD();
void disabled();
void idle();
void isRunning();
void error();
//
//void U0init(int);
//void adc_init();
//unsigned char kbhit();
//unsigned char getChar();
//void putChar(unsigned char);

void setup() {
//  // setup the UART
//  U0init(9600);
//  // setup the ADC
//  adc_init();
//  //set PB7 to OUTPUT
//  *ddr_b |= 0x80;
}

void loop() {

//reads serial input like stop and start
//  if (kbhit())
//  {
//    // read the character
//    in_char = getChar();
//    // echo it back
//    putChar(in_char);
//    // if it's the quit character
//    if (in_char == 'stop' || in_char == 'STOP') {
//      disabled();
//    }

//check levels
    if (waterLevel < waterThresh) {
      error();
    }
    if (temp > tempThresh) {
      isRunning();
    }
    else {
      idle();
    }

  
}

//vent position adjustable in all states except for Error

//functions


//display temp and humidity
#include <LiquidCrystal.h>
const int rs = 44, en = 45, d4 = 46, d5 = 47, d6 = 48, d7 = 49;

void displayLCD() {
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
  lcd.begin(16, 2);
  lcd.print("First line");
  lcd.setCursor(0,1);
  lcd.print("Second line");
}

void fan(_Bool var) {
  if (var == true) {
    //fan on
  }
  else {
    //fan off
  }
}

void LED(int var) {
  switch (var) {
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

void disabled() {
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

void idle() {
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

void isRunning() {
  fan(true);
  LED(1);
}

//Error
//display error message: "Water level is too low"
//red LED ON

void error()
{
  LED(2);
}

//void adc_init()
//{
//  // setup the A register
//  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
//  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC trigger mode
//  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC interrupt
//  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to set prescaler selection to slow reading
//  // setup the B register
//  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
//  *my_ADCSRB &= 0b11111000;  // clear bit 2-0 to 0 to set free running mode
//  // setup the MUX Register
//  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
//  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
//  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
//  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
//}
//unsigned int adc_read(unsigned char adc_channel_num)
//{
//  // clear the channel selection bits (MUX 4:0)
//  *my_ADMUX  &= 0b11100000;
//  // clear the channel selection bits (MUX 5)
//  *my_ADCSRB &= 0b11110111;
//  // set the channel number
//  if (adc_channel_num > 7)
//  {
//    // set the channel selection bits, but remove the most significant bit (bit 3)
//    adc_channel_num -= 8;
//    // set MUX bit 5
//    *my_ADCSRB |= 0b00001000;
//  }
//  // set the channel selection bits
//  *my_ADMUX  += adc_channel_num;
//  // set bit 6 of ADCSRA to 1 to start a conversion
//  *my_ADCSRA |= 0x40;
//  // wait for the conversion to complete
//  while ((*my_ADCSRA & 0x40) != 0);
//  // return the result in the ADC data register
//  return *my_ADC_DATA;
//}
//void print_int(unsigned int out_num)
//{
//
//  // clear a flag (for printing 0's in the middle of numbers)
//  unsigned char print_flag = 0;
//  // if its greater than 1000
//  if (out_num >= 1000)
//  {
//    // get the 1000's digit, add to '0'
//    U0putchar(out_num / 1000 + '0');
//    // set the print flag
//    print_flag = 1;
//    // mod the out num by 1000
//    out_num = out_num % 1000;
//  }
//  // if its greater than 100 or we've already printed the 1000's
//  if (out_num >= 100 || print_flag)
//  {
//    // get the 100's digit, add to '0'
//    U0putchar(out_num / 100 + '0');
//    // set the print flag
//    print_flag = 1;
//    // mod the output num by 100
//    out_num = out_num % 100;
//  }
//  // if its greater than 10, or we've already printed the 10's
//  if (out_num >= 10 || print_flag)
//  {
//    U0putchar(out_num / 10 + '0');
//    print_flag = 1;
//    out_num = out_num % 10;
//  }
//  // always print the last digit (in case it's 0)
//  U0putchar(out_num + '0');
//  // print a newline
//  U0putchar('\n');
//
//
//}
//
//void U0init(int U0baud)
//{
//  unsigned long FCPU = 16000000;
//  unsigned int tbaud;
//  tbaud = (FCPU / 16 / U0baud - 1);
//  // Same as (FCPU / (16 * U0baud)) - 1;
//  *myUCSR0A = 0x20;
//  *myUCSR0B = 0x18;
//  *myUCSR0C = 0x06;
//  *myUBRR0  = tbaud;
//}
//unsigned char U0kbhit()
//{
//  return *myUCSR0A & RDA;
//}
//unsigned char U0getchar()
//{
//  return *myUDR0;
//}
//void U0putchar(unsigned char U0pdata)
//{
//  while ((*myUCSR0A & TBE) == 0);
//  *myUDR0 = U0pdata;
//}
