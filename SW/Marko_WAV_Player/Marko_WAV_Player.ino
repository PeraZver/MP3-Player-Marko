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
#include <string.h>
#define F_CPU 8000000UL // 8 MHz clock 

TMRpcm tmrpcm;          // create an object for use in this sketch
uint8_t buttons = 0;
boolean play_flag, start_playing = LOW;
char *song[5];        // Playlist stored as an array of strings i.e. pointers to the char
uint8_t songCtr = 0;  // Number of songs 
uint8_t currentSong = 0;  //Current song in the playlist, start from 0.

void setup(){
  /* Set system clock */
  CLKSEL0 = 0b00010101;   // Choose Crystal oscillator with BOD
  CLKSEL1 = 0b00001111;   // 8MHz
  CLKPR = 0b10000000;  // Change the clock prescaler
  CLKPR = 0;           // Prescaler is 1.

  /* Disable JTAG interface on PORTF */
  MCUCR |= (1<<JTD);   // 
  MCUCR |= (1<<JTD);   // Have to do it twice (datasheet page 328.)

  delay(5000);
  
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
  char i = 0;  // helpful counter
  find_music();
  // song = entry.name();          // get the 1st song name
  Serial.println("Playlist: ");
  for (i=0; i<5; i++)
   Serial.println(*(song+i));

  Serial.print("First song is: ");
  Serial.println(song[currentSong]);

  /* Set PWM output */  
  tmrpcm.speakerPin = 9; // PWM player output mono
  pinMode(10,OUTPUT);    // and stereo
  
}





void loop(){  

 buttons = PINF;
 Serial.println(buttons, BIN);

 switch (buttons){

  case (1 << PINF7):         // "Play/Pause" button on PF7
       if (!play_flag & !start_playing){      // if it's not playing, make it play
          tmrpcm.play(song[currentSong]);
          Serial.print("Started playing: ");
          Serial.println(song[currentSong]);
          play_flag = start_playing = HIGH;
       }
       else if (!play_flag & start_playing){
          tmrpcm.pause();
          Serial.println("unpause");
          play_flag = HIGH;
       }
       else {
          tmrpcm.pause();
          Serial.println("pause");
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

  case (1 << PINF5):    // Play next file
      toggle_LED();
      if (currentSong < songCtr){
          currentSong++;
          tmrpcm.play(song[currentSong]);
          Serial.print("Now playing: ");
          Serial.println(song[currentSong]);
      }
      break;

  case (1 << PINF6):    // Play previous file
      toggle_LED();
      if (currentSong > 0){
          currentSong--;
          tmrpcm.play(song[currentSong]);          
          Serial.print("Now playing: ");
          Serial.println(song[currentSong]);
      }
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
     if (entry.isDirectory()) {
       //Serial.println("/");
       //printDirectory(entry, numTabs+1);
     } else {
        song[songCtr] = strdup(entry.name());
        //Serial.println(song[i]);
        //Serial.println(song[2]);
        songCtr++;
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
     
   }
}

