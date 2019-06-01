#include <pcmConfig.h>
#include <pcmRF.h>
#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
TMRpcm audio;

#define SD_ChipSelectPin 10 //Chip select is pin number 53

char mystr[2]; //Initialized variable to store recieved data

// ------------------------------------------------------------------------------------------------------------------------------



void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);

  audio.CSPin = SD_ChipSelectPin;
  audio.speakerPin = 9; //Audio out on pin 9
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }

  audio.setVolume(7);    //   0 to 7. Set volume level
  audio.quality(0);        //  Set 1 for 2x oversampling Set 0 for normal
  //audio.volume(0);        //   1(up) or 0(down) to control volume
  //audio.play("filename",30); plays a file starting at 30 seconds into the track

  //audio.play("1.wav");
}

// ------------------------------------------------------------------------------------------------------------------------------


void loop() {

  Serial.readBytes(mystr, 1); //Read the serial data and store in var


  if (mystr[0] == 'I') {
    Serial.println("play IN");
    audio.play("I.wav");
    Serial.println("play OUT");
  }

  if (mystr[0] == 'N') {
    Serial.println("STOP IN");
    audio.stopPlayback();
    Serial.println("STOP OUT");
  }

  if (mystr[0] == 'R') {
    Serial.println("startrecording IN");
    audio.startRecording("1.wav", 16000, A0);
    Serial.println("startrecording OUT");
  }

  if (mystr[0] == 'E') {
    Serial.println("stoprecording IN");
    audio.stopRecording("1.wav");
    Serial.println("stoprecording OUT");
  }

  if (mystr[0] == 'P') {
    Serial.println("play IN");
    audio.play("1.wav");
    Serial.println("play OUT");
  }

  if (mystr[0] == 'S') {
    Serial.println("STOP IN");
    audio.stopPlayback();
    Serial.println("STOP OUT");
  }
}
