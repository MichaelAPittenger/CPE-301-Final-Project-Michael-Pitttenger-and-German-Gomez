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
//ADC
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

//variables
byte in_char;
int waterThresh;
int waterLevel;
int tempThresh;
int temp;
int counter;

//initialize functions

void setup() 
{                
  // set PB6 to output
  *portDDRB |= 0xFF;
  // set PB6 LOW
  *portB &= 0xBF;
  // Start the UART
  U0Init(9600);
  counter = 0;
}

void loop() 
{

  
  
  // if we recieve a character from serial
  if (kbhit()) 
  {
    // read the character
    in_char = getChar();
    // echo it back
    putChar(in_char);
    // 's' toggles between on/off
    if(in_char == 's'||in_char == 'S')
    {
      counter++;
    }
    
  }

  //check on/off, then check levels
  if(counter%2==0)
  {
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
  else
  {
    disabled();
  }
  delay(100);
}

//vent position adjustable in all states except for Error

//functions

//display temp and humidity

void displayLCD() {

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
     break;
    case 2:
     //turn red on
     break;
    case 3:
      //turn yellow on
      *portB &= 0b10111111;
      *portB |= 0b10000000;
     break;
    case 4:
      //turn green on
      *portB &= 0b01111111;
      *portB |= 0b01000000;
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
