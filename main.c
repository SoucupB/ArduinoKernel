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
  String folderName;
  String key;
  String value;
  struct FileTree_t* left;
  struct FileTree_t* right;
  struct FileTree_t** child;
};

struct Vector {
  void *buffer;
  int32_t capacity;
  int32_t size;
  int32_t elementsSize;
};

struct Vector *vct_Init(int32_t elementSize) {
  struct Vector *vct = (struct Vector *)malloc(elementSize);
  vct->buffer = malloc(elementSize);
  vct->capacity = 1;
  vct->size = 0;
  vct->elementsSize = elementSize;
  return vct;
}

void vct_Add(struct Vector *vct, void *element) {
  if(vct->size >= vct->capacity) {
    vct->capacity <<= 1;
    void *buffer = malloc(vct->elementsSize * vct->capacity);
    memcpy(buffer, vct->buffer, vct->size * vct->elementsSize);
  }
}

struct FileTree_t *fd_AddFileTree(struct FileTree_t* folder, String folderName,
                               String fileName, String content) {
  if(!folder) {
    struct FileTree_t *newFolder = (struct FileTree_t*)malloc(
      							  	sizeof(struct FileTree_t));
    newFolder->key = fileName;
    newFolder->value = content;
    newFolder->folderName = folderName;
    newFolder->left = 0;
    newFolder->right = 0;
    newFolder->child = 0;
    return newFolder;
  }
  if(folder->key > fileName) {
    folder->left = fd_AddFileTree(folder->left, folderName, fileName, content);
  }
  else {
    folder->right = fd_AddFileTree(folder->right, folderName, fileName, content);
  }
  return folder;
}

struct FileTree_t *fd_init(String folderName) {
  struct FileTree_t *newFolder = (struct FileTree_t*)malloc(
    sizeof(struct FileTree_t));
  newFolder->folderName = folderName;
  newFolder->left = 0;
  newFolder->right = 0;
  newFolder->child = 0;
  return newFolder;
}

void createSubfolder(struct FileTree_t* folder, struct FileTree_t* subFolder) {
  
}

String fd_GetFileContent(struct FileTree_t* file, String fileName) {
  if(!file) {
    return "";
  }
  if(file->key == fileName) {
    return file->value;
  }
  if(file->key > fileName) {
    return fd_GetFileContent(file->left, fileName);
  }
  if(file->key <= fileName) {
    return fd_GetFileContent(file->right, fileName);
  }
}

bool isInstructionAcceptable(String instr) {
  for(int8_t i = 0; i < instrLength; i++) {
    if(instructions[i] == instr) {
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started!");
 // saveString("AAA", 0);
 // readStr(0);
  struct FileTree_t* folder = 0;
  folder = fd_AddFileTree(folder, parentDirectory, "firstFile.txt", "Some content");
  folder = fd_AddFileTree(folder, parentDirectory, "secondFile.txt", "Some another content");
  Serial.println("Done");
  Serial.println(fd_GetFileContent(folder, "firstFile.txt"));
  Serial.println(fd_GetFileContent(folder, "secondFile.txt"));
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
 