/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */


#include <LiquidCrystal.h>
#include <EEPROM.h>

String parentDirectory = "C:/";

String instructions[] = {
  "cd",
  "ls",
  "touch",
  "compile"
};
int8_t instrLength = 4;

struct FileTree_t {
  String key;
  String value;
  struct FileTree_t* left;
  struct FileTree_t* right;
};

struct FileTree_t *addFileTree(struct FileTree_t* file, String fileName, String content) {
  if(!file) {
    struct FileTree_t *newFile = (struct FileTree_t*)malloc(
      							  sizeof(struct FileTree_t));
    file->key = fileName;
    file->value = content;
    file->left = 0;
    file->right = 0;
    return newFile;
  }
  return 0;
}

bool isInstructionAcceptable(String instr) {
  for(int8_t i = 0; i < instrLength; i++) {
    if(instructions[i] == instr) {
      return true;
    }
  }
  return false;
}

void saveString(String element, int offset) {
  EEPROM.write(offset, element.length());
  for(int8_t i = 0; i < element.length(); i++) {
    EEPROM.write(offset + i + 1, element[i]);
  }
}

String readStr(int offset) {
  String reference = "";
  int strSize = EEPROM.read(offset);
  for(int i = 0; i < strSize; i++) {
    reference += (char)EEPROM.read(offset + i + 1);
  }
  return reference;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started!");
 // saveString("AAA", 0);
 // readStr(0);
  int *parl;
  parl = (int* )malloc(sizeof(int) * 5);
  parl[0] = 1;
  parl[1] = 2;
  Serial.println(parl[0]);
  Serial.println(parl[1]);
 // Serial.println(readStr(0));
}

String readInstruction() {
  String response = "";
  if(Serial.available() > 0) {
  	response = Serial.readString();
  }
  return response;
}

void recieveInstruction(String instruction) {
  
}

void loop() {
  char incomingByte = 0;
  String response = readInstruction();
  if(response != "") {
    if(!isInstructionAcceptable(response)) {
      Serial.println("Unknown instruction: " + response);
    }
    else {
      Serial.println(response);
    }
  }
}
 