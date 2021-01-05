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
String parentDirectory = "C:/";

String instructions[] = {
  "cd",
  "ls",
  "touch",
  "compile",
  "mkdir",
  "write",
  "where"
};
int8_t instrLength = 7;

struct Vector {
  void *buffer;
  int32_t capacity;
  int32_t size;
  int8_t elementsSize;
};

struct FileTree_t {
  String folderName;
  String key;
  String value;
  struct FileTree_t* left;
  struct FileTree_t* right;
  struct Vector *child;
};

struct FileTree_t* currentFolder;
struct Vector *folderStack;

struct Vector *vct_Init(int8_t elementSize) {
  struct Vector *self = (struct Vector *)malloc(sizeof(struct Vector));
  self->buffer = malloc(elementSize);
  self->capacity = 1;
  self->size = 0;
  self->elementsSize = elementSize;
  return self;
}

void vct_PushElement(struct Vector *vct, void *element) {
  memcpy(vct->buffer + vct->size * vct->elementsSize, element, vct->elementsSize);
  vct->size++;
}

void vct_Push(struct Vector *vct, void *element) {
  if(vct->size >= vct->capacity) {
    vct->capacity <<= 1;
    void *buffer = malloc(vct->elementsSize * vct->capacity);
    memcpy(buffer, vct->buffer, vct->size * vct->elementsSize);
    free(vct->buffer);
    vct->buffer = buffer;
  }
  vct_PushElement(vct, element);
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
    newFolder->child = vct_Init(sizeof(struct FileTree_t*));
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

struct FileTree_t *fd_Init(String folderName) {
  Serial.println(folderStack->size);
  struct FileTree_t *newFolder = (struct FileTree_t*)malloc(
    sizeof(struct FileTree_t));
  newFolder->folderName = folderName;
  newFolder->left = 0;
  newFolder->right = 0;
  newFolder->child = vct_Init(sizeof(struct FileTree_t *));
  return newFolder;
}

void createSubfolder(struct FileTree_t* folder, String name) {
  struct FileTree_t *newFolder = fd_Init(name);
  vct_Push(folder->child, &newFolder);
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
  folderStack = vct_Init(sizeof(struct FileTree_t *));
  Serial.println(folderStack->size);
  currentFolder = fd_Init(parentDirectory);


  vct_Push(folderStack, &currentFolder);
  currentFolder = fd_AddFileTree(currentFolder, parentDirectory, "firstFile.txt", "Some content");
  currentFolder = fd_AddFileTree(currentFolder, parentDirectory, "secondFile.txt", "Some another content");
  Serial.println("Done");
 // Serial.println(fd_GetFileContent(currentFolder, "firstFile.txt"));
 // Serial.println(fd_GetFileContent(currentFolder, "secondFile.txt"));
}

String readInstruction() {
  String response = "";
  if(Serial.available() > 0) {
  	response = Serial.readString();
  }
  return response;
}

void recieveInstruction(String instruction) {
  if(instruction == "where") {
    struct FileTree_t **stack = ((struct FileTree_t **)folderStack->buffer);
    for(int32_t i = 0; i < folderStack->size; i++) {
      struct FileTree_t *cFolder = stack[i];

    }
  }
}

void loop() {
  char incomingByte = 0;
  String response = readInstruction();
  if(response != "") {
    if(!isInstructionAcceptable(response)) {
      Serial.println("Unknown instruction: " + response);
    }
    else {
      recieveInstruction(response);
    }
  }
}