/*****************************
 * 
 * Marko WAV Player
 * 
 * Pero, Dec. 2016 
 * 
 */


#include <SD.h>
#define SD_ChipSelectPin 17  //using digital pin 4 on arduino nano 328, can use other pins
#include <TMRpcm.h>           //  also need to include this library...
#include <SPI.h>
#define F_CPU 8000000UL // 8 MHz clock 

TMRpcm tmrpcm;   // create an object for use in this sketch
uint8_t buttons = 0;
boolean play_flag = 0;


void setup(){
  /* Set system clock */
  CLKSEL0 = 0b00010101;   // Choose Crystal oscillator with BOD
  CLKSEL1 = 0b00001111;   // 8MHz
  CLKPR = 0b10000000;  // Change the clock prescaler
  CLKPR = 0;           // Prescaler is 1.

  MCUCR |= (1<<JTD);   // Disable JTAG interface on PORTF
  MCUCR |= (1<<JTD);   // Have to do it twice (datasheet page 328.)
  
  tmrpcm.speakerPin = 9; // PWM player output mono
  pinMode(10,OUTPUT);    // and stereo

  Serial.begin(9600);
  DDRF = 0x01; // Set all pins in PORTF as input except the last one where is LED
  
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    return;   // don't do anything more if not
  }
}



void loop(){  

 buttons = PINF;
 Serial.println(buttons, BIN);

 switch (buttons){

  case (1 << PINF7):         // "Play/Pause" button on PF7
       if (!play_flag){     // if it's not playing, make it play
          tmrpcm.play("PRINCE16.WAV");
          play_flag = HIGH;
       }
       else {
          tmrpcm.pause();
          play_flag = LOW;
       }

      toggle_LED();
       
       break;
       
  case (1 << PINF4):   // Volume up button on PF4
       tmrpcm.volume(1); 
       toggle_LED();
       break;
       
  case (1 << PINF1):    // Volume down button on PF1
       //tmrpcm.volume(0); 
       tmrpcm.play("PRINCE16.WAV");
       toggle_LED();
       break;
      
  default:
      break;
  }
  
 /*   switch(ser){
      case 'p':
          tmrpcm.play("PRINCE8.WAV");
          break;
      case 'i':
          tmrpcm.play("PRINCE11.WAV");
          break;
      case 'r':
          tmrpcm.play("PRINCE16.wav");
          break;
      case 'c':
          tmrpcm.play("PRINCE32.wav");
          break;
       case 'e':
          tmrpcm.play("PRINCE44.wav");
          break;
      default:
        delay(10);
  }*/

delay(500);
}

void toggle_LED(void){
       PORTF |= (1 << PINF0);
       delay(500);
       PORTF &= ~(1 << PINF0);
  
}

