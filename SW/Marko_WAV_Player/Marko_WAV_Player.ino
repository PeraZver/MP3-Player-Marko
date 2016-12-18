/****************************************
 * 
 * Marko WAV Player with an OLED display
 * 
 * Pero, Dec. 2016 
 * 
 ****************************************/

#include <SPI.h>
#include <Wire.h>
#include <SdFat.h>
#include <TMRpcm.h>           
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define F_CPU 8000000UL // 8 MHz clock 

//SPI digital pins (taken from Leonardo board)
#define OLED_DC    6
#define OLED_CS    4
#define OLED_RESET  12
#define SD_ChipSelectPin 17  

// defs for OLED
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// Classes for SD, TMRpcm player and OLED
SdFat sd;
TMRpcm tmrpcm;     
//Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

uint8_t buttons = 0;
boolean play_flag = LOW, start_playing = LOW;
char song_name[50]={'0'};   // Song name for OLED
uint8_t songCtr = 0;        // Number of songs 
uint8_t currentSong = 0;    // Current song in the playlist, start from 0.


void setup(){

  AVRSetup();

  Serial.begin(9600);
  delay(4000);  
  
  /* Initialize SD Card*/ 
  if (!sd.begin(SD_ChipSelectPin)) {    // see if the card is present and can be initialized
    Serial.println("SD Card Error");
    return;                             // don't do anything more if not
  }
  
  // Generate the OLED supply from the 3.3v line internally
/* display.begin(SSD1306_SWITCHCAPVCC);  // Show image buffer on the display hardware.
  // Show image buffer on the display hardware.
  display.display();
  delay(2000);
  display.clearDisplay();*/


  /* Make the playlist */
  NumberOfSongs();     // Counts the number of songs in wav file, stores in songCtr
  
  uint8_t i=0;
  Serial.println("Playlist: ");
  for (i=0; i<songCtr; i++){
     find_music(i);
     Serial.println(song_name);
  }

  Serial.print("Total No. of songs: ");
  Serial.println(songCtr);
  
  /* Set PWM output */  
  tmrpcm.speakerPin = 9; // PWM player output mono
  pinMode(10,OUTPUT);    // and stereo

  //tmrpcm.play(song_name);
}

void loop(){  


 buttons = PINF;
 /* Check the buttons first */
 switch (buttons){

  case (1 << PINF7):         // "Play/Pause" button on PF7


       
       if (!play_flag & !start_playing){      // if it's not playing, make it play

//          Serial.print("Counter: ");
//          Serial.println(songCtr);

          find_music(currentSong);         // find the 1st WAV file, currentSong is 0
//          PrintToOLED(song_name); 
          Serial.println(song_name);
          tmrpcm.play(song_name);
          play_flag = HIGH;
          start_playing = HIGH;
       }
       else if (!play_flag & start_playing){
//          Serial.print("Counter: ");
//          Serial.println(songCtr);
          tmrpcm.pause();
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

  case (1 << PINF5):    // Play next file
      toggle_LED();
      if (currentSong < songCtr){
          tmrpcm.disable();   // Disable so we can acces the SD card
          currentSong++;
//          Serial.print("Counter: ");
//          Serial.println(currentSong);
          find_music(currentSong);
//          PrintToOLED(song_name); 
          Serial.println(song_name);
          tmrpcm.play(song_name);
      }
      break;

  case (1 << PINF6):    // Play previous file
      toggle_LED();
      if (currentSong > 0){
          tmrpcm.disable();   // Disable so we can acces the SD card
          currentSong--;
//          Serial.print("Counter: ");
//          Serial.println(currentSong);
          find_music(currentSong);   
//          PrintToOLED(song_name);     
          Serial.println(song_name);
          tmrpcm.play(song_name);    
      }
      break;     

      
  default:
      break;
  }
  
 delay(100);

  /* Keep on with the playlist */
  if (start_playing & !tmrpcm.isPlaying()){  //If the playlist is started, and one song has finished, continue to the next song
   if (currentSong < songCtr){               // if the list has not come to an end
          currentSong++;
          find_music(currentSong);
//          PrintToOLED(song_name); 
          tmrpcm.play(song_name);
//          Serial.println(song_name);
      }
    else {
      currentSong = 0;                 // if the list has come to an end, reset the counter
      start_playing = LOW;             // and go stop playing mode
    }
    
  }

 
}

void toggle_LED(void){
/* Simple LED toggler for test purposes */ 
       PORTF |= (1 << PINF0);
       delay(500);
       PORTF &= ~(1 << PINF0);
  
}

void find_music(uint8_t songNumber){
/* This function searches the root folder of SD card 
 *  and gets the filename of the songNumber-th WAV file 
 */
  SdFile root;            // Grab the file system object
  SdFile entry;
  uint8_t lineCounter = 0;
  /* Get the filenames */
  root.open("/", O_READ  );  
  
  while(entry.openNext(&root, O_READ) && (lineCounter <= songNumber ) ) {     
        if (!entry.isSubDir() && !entry.isHidden()) {      
          entry.getName(song_name, 50);
          lineCounter++;
        }
        entry.close();
  }
root.close();
}

void NumberOfSongs(){
/* This function searches the root folder of SD card 
 *  and counts the wav files. 
 */
  SdFile root;            // Grab the file system object
  SdFile entry;
  uint8_t lineCounter = 0;
  /* Get the filenames */
  root.open("/", O_READ  );  
  
  while(entry.openNext(&root, O_READ)) {     
        if (!entry.isSubDir() && !entry.isHidden())      
          songCtr++;
        entry.close();
  }
root.close();
}

//void PrintToOLED(char* text) {
//  
//
//  display.clearDisplay(); 
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0,0);
//  display.clearDisplay();
//  display.println(text);
//  display.display();
//  //display.startscrollright(0x00, 0x0F);
//}

void AVRSetup(){
    /* Set system clock */
  CLKSEL0 = 0b00010101;   // Choose Crystal oscillator with BOD
  CLKSEL1 = 0b00001111;   // 8MHz
  CLKPR = 0b10000000;     // Change the clock prescaler
  CLKPR = 0;              // Prescaler is 1.

  /* Disable JTAG interface on PORTF */
  MCUCR |= (1<<JTD);   // 
  MCUCR |= (1<<JTD);   // Have to do it twice (datasheet page 328.)

  /* Set buttons */
  DDRF = 0x01; // Set all pins in PORTF as input except the last one where is LED
}

