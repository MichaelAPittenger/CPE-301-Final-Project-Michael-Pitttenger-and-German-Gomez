#define RDA 0x80
#define TBE 0x20
#include <DHT.h>
#include <RTClib.h>

// UART Pointers
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
// GPIO Pointers
volatile unsigned char *pinB     = (unsigned char*) 0x22; 
volatile unsigned char *portB    = (unsigned char*) 0x23; 
volatile unsigned char *portDDRB = (unsigned char*) 0x24;

volatile unsigned char *pinC     = (unsigned char*) 0x26;
volatile unsigned char *portC    = (unsigned char*) 0x28; 
volatile unsigned char *portDDRC = (unsigned char*) 0x27; 
//ADC
volatile unsigned char* my_ADMUX   = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB  = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA  = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

//variables
byte in_char;
int waterThresh = 200;
int tempThresh = 80;
float temp;
float humi;
int counter = 0;
int stepNumber = 0;
int state = 0;
int lastState = 0;

//initialize functions

DHT dht(2,DHT11);
RTC_DS1307 rtc;

void setup() 
{     
  // SETUP RTC MODULE
  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
//    Serial.flush();
    while (1);
  }
  dht.begin();
  // automatically sets the RTC to the date & time on PC this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //set PB4 to input
  *portDDRB &= 0b00101111;           
  // set the rest to output
  *portDDRB |= 0b00101111;
  *portDDRC |= 0xFF;
  // set all to LOW
  *portB &= 0xFF;
  // Start the UART
  U0Init(9600);
  // setup the ADC
  adc_init();
  counter = 0;
}

void loop() 
{
  delay(1000);
  
  humi = dht.readHumidity();
  temp = dht.readTemperature(true);
   
  // get the reading from the ADC
  unsigned int waterLevel = adc_read(0);
  // get the reading from the ADC
  unsigned int potentiometer = adc_read(1);

  ventAngle(potentiometer);


  //check on/off, then check levels
  if(*portB & 0x10)
  {
    counter++;
  }

  if(counter%2 != 0)
  {
    //check levels
    if (waterLevel < waterThresh) {
      error();
    }
    else if (temp > tempThresh) {
      isRunning();
    }
    else {
      idle();
    }
  }
  else
  {
    disabled();
  }

  if(lastState != state){
    displayRTC();
  }
  lastState = state;
  
}

//vent position adjustable in all states except for Error

//functions

//display temp and humidity
#include <LiquidCrystal.h>
const int rs = 48, en = 49, d4 = 46, d5 = 47, d6 = 44, d7 = 45;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void displayLCD(String firstLine, String secondLine) {
  lcd.begin(16, 2);
  lcd.print(firstLine);
  lcd.setCursor(0,1);
  lcd.print(secondLine);
}

void ventAngle(int dir){
  int var = 0;
  for(int i=0;i<30;i++){
    if(dir > 600){
      switch(var){
        case 0:
          *portC &= 0xF0;
          *portC |= 0b00000001;
          delay(5);
        case 1:
          *portC &= 0xF0;
          *portC |= 0b00000010;
          delay(5);
        case 2:
          *portC &= 0xF0;
          *portC |= 0b00000100;
          delay(5);
        case 3:
          *portC &= 0xF0;
          *portC |= 0b00001000;
          delay(5);
        break;
      }
    }
    else if(dir < 200){
      switch(var){
        case 0:
          *portC &= 0xF0;
          *portC |= 0b00001000;
          delay(5);
        case 1:
          *portC &= 0xF0;
          *portC |= 0b00000100;
          delay(5);
        case 2:
          *portC &= 0xF0;
          *portC |= 0b00000010;
          delay(5);
        case 3:
          *portC &= 0xF0;
          *portC |= 0b00000001;
          delay(5);
          break;
      }
    }
    stepNumber++;
    if(stepNumber > 3){
      stepNumber = 0;
    }
  }
}

void displayRTC(){
  DateTime now = rtc.now();
  putChar('T');
  putChar('I');
  putChar('M');
  putChar('E');
  putChar(':');

  unsigned int year = (now.year());
  print_int(year);
  putChar('M');
  putChar(':');
  unsigned int month = (now.month());
  print_int(month);
  putChar('D');
  putChar(':');
  unsigned int day = (now.day());
  print_int(day);
  putChar('H');
  putChar(':');
  unsigned int hour = (now.hour());
  print_int(hour);
  putChar('M');
  putChar('i');
  putChar('n');
  putChar(':');
  unsigned int minute = (now.minute());
  print_int(minute);
  putChar('S');
  putChar(':');
  unsigned int second = (now.second());
  print_int(second);
}

void fan(_Bool var) {
  *portB &= 0xDF;
  if (var == true) {
    *portB |= 0b00100000;
  }
}

void LED(int var) {

  *portB &= 0x2F;
  
  switch (var) {
    case 1:
    //turn yellow on
      *portB |= 0b00000001;
     break;
    case 2:
    //turn green on
      *portB |= 0b00000010;
     break;
    case 3:
     //turn blue on
      *portB |= 0b00000100;
     break;
    case 4:
     //turn red on
      *portB |= 0b00001000;
      break;
  }
}

//Disabled
//fan off
//yellow LED ON
//start --> Idle

void disabled() {
  state = 1;
  fan(false);
  LED(1);
  String tempRead = "Temp: " + String(temp) + " F";
  String humiRead = "Humi: " + String(humi) + " %";
  displayLCD(tempRead, humiRead); 
}

//Idle
//fan off
//green LED ON
//temp and humidity displayed on LCD
//stop --> Disabled
//temp > threshold --> Running
//water level =< threshold --> Error

void idle() {
  state = 2;
  fan(false);
  LED(2);
  String tempRead = "Temp: " + String(temp) + " F";
  String humiRead = "Humi: " + String(humi) + " %";
  displayLCD(tempRead, humiRead); 
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
  state = 3;
  fan(true);
  LED(3);
  String tempRead = "Temp: " + String(temp) + " F";
  String humiRead = "Humi: " + String(humi) + " %";
  displayLCD(tempRead, humiRead); 
}

//Error
//display error message: "Water level is too low"
//red LED ON

void error(){
  state = 4;
  LED(4);
  displayLCD("Error","");
}

/*
 * UART FUNCTIONS
 */
void U0Init(int U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
unsigned char kbhit()
{
  return *myUCSR0A & RDA;
}
unsigned char getChar()
{
  return *myUDR0;
}
void putChar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}

//ADC FUNCTIONS
void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000;  // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}
unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}
void print_int(unsigned int out_num)
{

  // clear a flag (for printing 0's in the middle of numbers)
  unsigned char print_flag = 0;
  // if its greater than 1000
  if(out_num >= 1000)
  {
    // get the 1000's digit, add to '0' 
    putChar(out_num / 1000 + '0');
    // set the print flag
    print_flag = 1;
    // mod the out num by 1000
    out_num = out_num % 1000;
  }
  // if its greater than 100 or we've already printed the 1000's
  if(out_num >= 100 || print_flag)
  {
    // get the 100's digit, add to '0'
    putChar(out_num / 100 + '0');
    // set the print flag
    print_flag = 1;
    // mod the output num by 100
    out_num = out_num % 100;
  } 
  // if its greater than 10, or we've already printed the 10's
  if(out_num >= 10 || print_flag)
  {
    putChar(out_num / 10 + '0');
    print_flag = 1;
    out_num = out_num % 10;
  } 
  // always print the last digit (in case it's 0)
  putChar(out_num + '0');
  // print a newline
  putChar('\n');
}
