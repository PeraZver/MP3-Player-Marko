/*****************************
 * 
 * Marko WAV Player with an OLED display
 * 
 * Pero, Dec. 2016 
 * 
 */

#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <TMRpcm.h>           
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define F_CPU 8000000UL // 8 MHz clock 

//SPI digital pins (from Leonardo board)
#define OLED_DC    6
#define OLED_CS    4
#define OLED_RESET  12
#define SD_ChipSelectPin 17  //using digital pin 4 on arduino nano 328, can use other pins 

// Classes for OLED display and SD card
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
TMRpcm tmrpcm;   // create an object for use in this sketch

uint8_t buttons = 0;
boolean play_flag, start_playing = LOW;
char *song[10]={};        // Playlist stored as an array of strings i.e. pointers to the char00000
uint8_t songCtr = 0;      // Number of songs 
uint8_t currentSong = 0;  // Current song in the playlist, start from 0.

// defs for OLED
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

void setup(){
  /* Set system clock */
  CLKSEL0 = 0b00010101;   // Choose Crystal oscillator with BOD
  CLKSEL1 = 0b00001111;   // 8MHz
  CLKPR = 0b10000000;  // Change the clock prescaler
  CLKPR = 0;           // Prescaler is 1.

  /* Disable JTAG interface on PORTF */
  MCUCR |= (1<<JTD);   // 
  MCUCR |= (1<<JTD);   // Have to do it twice (datasheet page 328.)

  /* Set buttons */
  DDRF = 0x01; // Set all pins in PORTF as input except the last one where is LED

  /* Set serial communication*/
  Serial.begin(9600);

  /* Initialize SD Card*/ 
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
     Serial.println("SD kartica nevalja. Glupane. ");
    return;   // don't do anything more if not
  }

  // Generate the OLED supply from the 3.3v line internally
  // display.begin(SSD1306_SWITCHCAPVCC);  // Show image buffer on the display hardware.
  //display.display();
  /*delay(2000);
  display.clearDisplay();    */


  /* Make the playlist */
  uint8_t i = 0;  // helpful counter
  //find_music();
  // song = entry.name();          // get the 1st song name
  /*Serial.println("Playlist: ");
  for (i=0; i<5; i++)
   Serial.println(*(song+i));

  Serial.print("First song is: ");
  Serial.println(song[currentSong]);*/

  /* Set PWM output */  
  tmrpcm.speakerPin = 9; // PWM player output mono
  pinMode(10,OUTPUT);    // and stereo

  delay(3000);
  Serial.print("Number of songs: ");
  Serial.println(NumberOfSongs());
  char pjesma[20]={'0'};
  
  for (i = 1; i <= 5; i++){
      ReadLine(pjesma, i);      // in array pjesma store the i-th line in the textfile.
      Serial.print("Pjesma br.");
      Serial.print(i);
      Serial.print(" je: ");
      Serial.print(pjesma);
  }    
  free(pjesma);
  //ReadLine(2);
}





void loop(){  

 buttons = PINF;
 /* Check the buttons first */
// switch (buttons){
//
//  case (1 << PINF7):         // "Play/Pause" button on PF7
//       if (!play_flag & !start_playing){      // if it's not playing, make it play
//          tmrpcm.play(song[currentSong]);
//          /*Serial.print("Started playing: ");
//          Serial.println(song[currentSong]);  */
//          testscrolltext(song[currentSong]); // Scroll some text.
//          play_flag = start_playing = HIGH;
//       }
//       else if (!play_flag & start_playing){
//          tmrpcm.pause();
//         // Serial.println("unpause");
//          play_flag = HIGH;
//       }
//       else {
//          tmrpcm.pause();
//        //  Serial.println("pause");
//          play_flag = LOW;
//       }
//       toggle_LED();       
//       break;
//       
//  case (1 << PINF4):   // Volume up button on PF4
//       tmrpcm.volume(1); 
//       toggle_LED();
//       break;
//       
//  case (1 << PINF1):    // Volume down button on PF1
//       tmrpcm.volume(0); 
//       toggle_LED();
//       break;
//
//  case (1 << PINF5):    // Play next file
//      toggle_LED();
//      if (currentSong < songCtr){
//          currentSong++;
//          tmrpcm.play(song[currentSong]);
//          testscrolltext(song[currentSong]); // Scroll some text.
//          /*Serial.print("Now playing: ");
//          Serial.println(song[currentSong]);*/
//      }
//      break;
//
//  case (1 << PINF6):    // Play previous file
//      toggle_LED();
//      if (currentSong > 0){
//          currentSong--;
//          tmrpcm.play(song[currentSong]);     
//          testscrolltext(song[currentSong]); // Scroll some text.     
//          /*Serial.print("Now playing: ");
//          Serial.println(song[currentSong]);*/
//      }
//      break;     
//
//      
//  default:
//      break;
//  }
//  
// delay(500);

  /* Keep on with the playlist */
  if (start_playing & !tmrpcm.isPlaying()){  //If the playlist is started, and one song has finished, continue to the next song
   if (currentSong < songCtr){               // if the list has not come to an end
          currentSong++;
          tmrpcm.play(song[currentSong]);
          testscrolltext(song[currentSong]); // Scroll some text.
          /*Serial.print("Now playing: ");
          Serial.println(song[currentSong]);*/
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

void testscrolltext(char* text) {
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.clearDisplay();
  display.println(text);
  display.display();
  display.startscrollright(0x00, 0x0F);
}

uint8_t NumberOfSongs(){
  /* Function that finds the number of lines (songs) in the text file*/
    File playlist;
    playlist = SD.open("playlist.txt");
    
    uint8_t lineCounter = 0;
    char Ch;
   
    while (playlist.available()) {

        Ch = playlist.read();   // read char by char in file        
        if ((Ch == '\n'))        // when we reach the end of the line
            lineCounter++;               // count the line        
    }
    playlist.close();
    return lineCounter;    // Count the last row
}

char ReadLine(char *line, uint8_t lineNumber){

/* Function reads a line specified by the lineNumber in txt file */

    File playlist;
    playlist = SD.open("playlist.txt");

    //char line[20];
    uint8_t lineCounter = 0, i = 0;
   
    while (lineCounter < lineNumber) {

        *(line+i) = playlist.read();   // read char by char in file
        i++;
        
        if (line[i-1] == '\n'){        // when we reach the end of the line or file...
            lineCounter++;               // count the line
            *(line + i) = '\0';              // add string terminator
            i = 0;                       // reset char counter
            //Serial.println(line);
            //Serial.print("Line counter: ");
            //Serial.print(lineCounter);
        }
    }
    
    playlist.close();
    
}

