/*****************************
 * 
 * Marko WAV Player
 * 
 * Pero, Dec. 2016 
 * 
 */


#include <SD.h>
#define SD_ChipSelectPin 17   //using digital pin 4 on arduino nano 328, can use other pins
#include <TMRpcm.h>           //  also need to include this library...
#include <SPI.h>
#define F_CPU 8000000UL // 8 MHz clock 

TMRpcm tmrpcm;          // create an object for use in this sketch
uint8_t buttons = 0;
boolean play_flag = LOW;
char * song= "";

void setup(){
  /* Set system clock */
  CLKSEL0 = 0b00010101;   // Choose Crystal oscillator with BOD
  CLKSEL1 = 0b00001111;   // 8MHz
  CLKPR = 0b10000000;  // Change the clock prescaler
  CLKPR = 0;           // Prescaler is 1.

  /* Disable JTAG interface on PORTF */
  MCUCR |= (1<<JTD);   // 
  MCUCR |= (1<<JTD);   // Have to do it twice (datasheet page 328.)

  delay(2000);
  
  /* Set buttons */
  DDRF = 0x01; // Set all pins in PORTF as input except the last one where is LED

  /* Set serial communication*/
  Serial.begin(9600);

  /* Initialize SD Card*/ 
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD kartica nevalja. Glupane. ");
    return;   // don't do anything more if not
  }

  /* Make the playlist */
  find_music();
 // song = entry.name();          // get the 1st song name
  song = "FRANK002.WAV";
  Serial.print("First song is: ");
  Serial.println(song);

  /* Set PWM output */  
  tmrpcm.speakerPin = 9; // PWM player output mono
  pinMode(10,OUTPUT);    // and stereo
  
}





void loop(){  

 buttons = PINF;
 Serial.println(buttons, BIN);

 switch (buttons){

  case (1 << PINF7):         // "Play/Pause" button on PF7
       if (!play_flag){      // if it's not playing, make it play
          tmrpcm.play(song);
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
       tmrpcm.volume(0); 
       toggle_LED();
       break;

  case (1 << PINF6):    // Play next file
      toggle_LED();
     /* entry =  root.openNextFile();
      song = entry.name();
      Serial.print("Now playing: ");
      Serial.println(song);
      tmrpcm.play(song);*/
      break;
      
  default:
      break;
  }
  
delay(500);
}

void toggle_LED(void){
/* Simple LED toggler for test purposes */ 
       PORTF |= (1 << PINF0);
       delay(500);
       PORTF &= ~(1 << PINF0);
  
}

void find_music(){
/* This function searches the root folder of SD card 
 *  and gets the filenames of WAV files.
 */

  File root;        // Grab the file system object
  /* Get the filenames */
  root = SD.open("/");  
  while(true) {
     
     File entry =  root.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       //printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

