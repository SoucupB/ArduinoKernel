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
struct Vector {
  void *buffer;
  int8_t capacity;
  int8_t size;
  int8_t elementsSize;
};

struct StringElement {
  char *buffer;
  int8_t sz;
};
int8_t instrLength = 8;
struct StringElement *instructions[8];

struct FileContent_t {
  struct StringElement *key;
  struct StringElement *value;
  struct FileContent_t *left;
  struct FileContent_t *right;
};

struct FileTree_t {
  struct StringElement *folderName;
  struct FileContent_t *files;
  struct Vector *child;
};
struct FileTree_t *currentFolder;
struct Vector *folderStack;
uint8_t str_Equal(struct StringElement *a, struct StringElement *b);

struct Vector *vct_Init(int8_t elementSize) {
  struct Vector *self = (struct Vector *)malloc(sizeof(struct Vector));
  self->buffer = malloc(elementSize);
  self->capacity = 1;
  self->size = 0;
  self->elementsSize = elementSize;
  return self;
}

void str_Delelte(struct StringElement *self) {
  free(self->buffer);
  free(self);
}

struct StringElement *str_Init(char *buffer) {
  int8_t bfSize = strlen(buffer);
  struct StringElement *self = (struct StringElement *)malloc(sizeof(struct StringElement));
  self->buffer = (char *)malloc(bfSize + 1);
  self->buffer[bfSize] = 0;
  memcpy(self->buffer, buffer, bfSize);
  self->sz = bfSize;
  return self;
}

void str_Delete(struct StringElement *self) {
  free(self->buffer);
  free(self);
}

void initCommands() {
  instructions[0] = str_Init((char *)"cd");
  instructions[1] = str_Init((char *)"ls");
  instructions[2] = str_Init((char *)"touch");
  instructions[3] = str_Init((char *)"compile");
  instructions[4] = str_Init((char *)"mkdir");
  instructions[5] = str_Init((char *)"write");
  instructions[6] = str_Init((char *)"where");
  instructions[7] = str_Init((char *)"check");
}

void vct_PushElement(struct Vector *vct, void *element) {
  memcpy(vct->buffer + vct->size * vct->elementsSize, element, vct->elementsSize);
  vct->size++;
}

void vct_Push(struct Vector *vct, void *element) {
  if(vct->size >= vct->capacity) {
    vct->capacity++;
    void *buffer = malloc(vct->elementsSize * vct->capacity);
    memcpy(buffer, vct->buffer, vct->size * vct->elementsSize);
    free(vct->buffer);
    vct->buffer = buffer;
  }
  vct_PushElement(vct, element);
}

struct FileContent_t *tr_AddFile(struct FileContent_t *file, struct StringElement *fileName,
                  				       struct StringElement *content, int8_t *isModifiedValue, int8_t *isModifiedKey) {
  if(!file) {
    struct FileContent_t *self = (struct FileContent_t *)malloc
      							 (sizeof(struct FileContent_t));
    self->left = 0;
    self->right = 0;
    self->key = fileName;
    self->value = content;
    *isModifiedKey = 1;
    *isModifiedValue = 1;
    return self;
  }
  if(str_Equal(file->key, fileName)) {
    str_Delete(file->value);
    file->value = content;
    *isModifiedValue = 1;
    return file;
  }
  if(strcmp(file->key->buffer, fileName->buffer) > 0) {
    file->left = tr_AddFile(file->left, fileName, content, isModifiedValue, isModifiedKey);
  }
  else {
    file->right = tr_AddFile(file->right, fileName, content, isModifiedValue, isModifiedKey);
  }
  return file;
}

void fd_AddFileTree(struct FileTree_t* folder,
                    struct StringElement *fileName, struct StringElement *content) {
  int8_t isModifiedValue = 0;
  int8_t isModifiedKey = 0;
  folder->files = tr_AddFile(folder->files, fileName, content, &isModifiedValue, &isModifiedKey);
  if(!isModifiedValue) {
    str_Delete(content);
  }
  if(!isModifiedKey) {
    str_Delete(fileName);
  }
}

struct FileTree_t *fd_Init(struct StringElement *folderName) {
  struct FileTree_t *self = (struct FileTree_t*)malloc(
    						              sizeof(struct FileTree_t));
  self->folderName = folderName;
  self->files = 0;
  self->child = vct_Init(sizeof(struct FileTree_t *));
  return self;
}

struct FileTree_t *createSubfolder(struct FileTree_t* folder, struct StringElement *name) {
  struct FileTree_t *newFolder = fd_Init(name);
  vct_Push(folder->child, &newFolder);
  return newFolder;
}

struct FileContent_t *tr_Find(struct FileContent_t *self, struct StringElement *fileName) {
  if(!self) {
    struct StringElement *element = str_Init((char *)"No such file exists!");
    Serial.println(element->buffer);
    str_Delete(element);
    return NULL;
  }
  if(str_Equal(self->key, fileName)) {
    return self;
  }
  if(strcmp(self->key->buffer, fileName->buffer) > 0) {
    return tr_Find(self->left, fileName);
  }
  return tr_Find(self->right, fileName);
}

void showFiles(struct FileContent_t* folder) {
  if(!folder) {
    return ;
  }
  showFiles(folder->left);
  Serial.print("File ");
  Serial.println(folder->key->buffer);
  showFiles(folder->right);
}

struct FileContent_t *fd_GetFileContent(struct FileTree_t* file, struct StringElement *fileName) {
  return tr_Find(file->files, fileName);
}

struct StringElement *strConcat(struct StringElement *src1, struct StringElement *src2) {
  return str_Init(strcat(src1->buffer, src2->buffer));
}

struct StringElement *getInstr(struct StringElement *instr) {
  char *tempBuffer = (char *)malloc(10);
  int8_t index = 0;
  memset(tempBuffer, 0, 10);
  for(int8_t i = 0; instr->buffer[i] != ' ' && i < instr->sz; i++) {
    tempBuffer[index++] = instr->buffer[i];
  }
  struct StringElement *resource = str_Init(tempBuffer);
  free(tempBuffer);
  return resource;
}

struct StringElement *getPhrase(struct StringElement *instruction, int8_t *index) {
  char *tempBuffer = (char *)malloc(80);
  memset(tempBuffer, 0, 80);
  if(*index < instruction->sz && instruction->buffer[*index] != 39) {
    free(tempBuffer);
    return NULL;
  }
  int8_t ind = 0;
  (*index)++;
  while(*index < instruction->sz && instruction->buffer[*index] != 39) {
    tempBuffer[ind++] = instruction->buffer[(*index)++];
  }
  (*index)++;
  struct StringElement *buffer = str_Init(tempBuffer);
  free(tempBuffer);
  return buffer;
}

struct Vector *arguments(struct StringElement *instruction) {
  struct Vector *argve = vct_Init(sizeof(struct Vector *));
  for(int8_t i = 0; i < instruction->sz; i++) {
    char *tempBuffer = (char *)malloc(16);
    int8_t index = 0;
    memset(tempBuffer, 0, 16);
    struct Vector *arg = vct_Init(sizeof(char *));
    struct StringElement *phrase = getPhrase(instruction, &i);
    struct StringElement *argument;
    if(phrase) {
      argument = phrase;
    }
    else {
      while(i < instruction->sz && instruction->buffer[i] != ' ') {
        tempBuffer[index++] = instruction->buffer[i];
        i++;
      }
      argument = str_Init(tempBuffer);
    }
    free(tempBuffer);
    char *heapAllocator = (char *)malloc(argument->sz + 1);
    memset(heapAllocator, 0, argument->sz + 1);
    memcpy(heapAllocator, argument->buffer, argument->sz);
    arg->buffer = heapAllocator;
    arg->size = argument->sz;
    arg->capacity = argument->sz;
    str_Delete(argument);
    vct_Push(argve, &arg);
  }
  return argve;
}

void showArguments(struct Vector *argve) {
  struct Vector **args = (struct Vector **)argve->buffer;
  for(int8_t i = 0; i < argve->size; i++) {
    char *buffer = (char *)args[i]->buffer;
    Serial.println(buffer);
  }
}

void processCd(struct Vector *argv, struct Vector *folders) {
  struct Vector **args = (struct Vector **)argv->buffer;
  if(argv->size != 2) {
    Serial.println("Invalid number of arguments for CD");
    return ;
  }
  char *buffer = (char *)args[1]->buffer;
  struct FileTree_t **fds = ((struct FileTree_t **)folders->buffer);
  for(int32_t i = 0; i < folders->size; i++) {
    if(!strcmp(fds[i]->folderName->buffer, buffer)) {
      vct_Push(folderStack, &fds[i]);
      currentFolder = fds[i];
      return ;
    }
  }
  Serial.println("No such directory exists!");
}

void touch(struct Vector *args) {
  if(args->size != 2) {
    Serial.println("Wrong number of args!");
  }
  else {
    struct Vector **arge = (struct Vector **)args->buffer;
    fd_AddFileTree(currentFolder, str_Init((char *)arge[1]->buffer), str_Init((char *)""));
  }
}

void mkdir(struct Vector *args) {
  if(args->size != 2) {
    Serial.println("Wrong number of args!\n");
  }
  else {
    struct Vector **arge = (struct Vector **)args->buffer;
    createSubfolder(currentFolder, str_Init((char *)arge[1]->buffer));
  }
}

String readInstruction() {
  String response = "";
  if(Serial.available() > 0) {
  	response = Serial.readString();
  }
  return response;
}

void testing() {
  struct FileTree_t *folder = createSubfolder(currentFolder, str_Init((char *)"Programfiles"));
  fd_AddFileTree(currentFolder, str_Init((char *)"personal.txt"), str_Init((char *)"Go to buy some milk!"));
  createSubfolder(folder, str_Init((char *)"Projects"));
  createSubfolder(folder, str_Init((char *)"Disfunction"));
  createSubfolder(folder, str_Init((char *)"Maps"));
  fd_AddFileTree(currentFolder, str_Init((char *)"toDo.txt"), str_Init("Finish some homework!"));
  fd_AddFileTree(currentFolder, str_Init((char *)"Pancakes.txt"), str_Init("DDAA!"));
  createSubfolder(currentFolder, str_Init((char *)"Music"));
  struct FileTree_t *movies = createSubfolder(currentFolder, str_Init((char *)"Movies"));
  createSubfolder(movies, str_Init((char *)"Action"));
  createSubfolder(movies, str_Init((char *)"Romance"));
  createSubfolder(movies, str_Init((char *)"SF"));
}

uint8_t str_Equal(struct StringElement *a, struct StringElement *b) {
  return a->sz == b->sz && !strncmp(a->buffer, b->buffer, a->sz);
}

uint8_t isInstructionAcceptable(struct StringElement *instr) {
  struct StringElement *getArgInst = getInstr(instr);
  for(int8_t i = 0; i < instrLength; i++) {
    if(str_Equal(getArgInst, instructions[i])) {
      free(getArgInst);
      return 1;
    }
  }
  free(getArgInst);
  return 0;
}

void setup() {
  Serial.begin(9600);
  struct StringElement *parentDirectory = str_Init((char *)"C:");
  initCommands();
  Serial.println("Started!");
  folderStack = vct_Init(sizeof(struct FileTree_t *));
  currentFolder = fd_Init(parentDirectory);
  testing();
  vct_Push(folderStack, &currentFolder);
  Serial.println("Done");
}

void where() {
  struct FileTree_t **stack = ((struct FileTree_t **)folderStack->buffer);
  for(int32_t i = 0; i < folderStack->size; i++) {
    struct FileTree_t *cFolder = stack[i];
    Serial.print(cFolder->folderName->buffer);
    Serial.print("/");
  }
  Serial.println();
}

void ls() {
  struct Vector *folders = ((struct Vector *)currentFolder->child);
  struct FileTree_t **cfl = (struct FileTree_t **)((struct Vector *)folders)->buffer;
  for(int32_t i = 0; i < folders->size; i++) {
    struct FileTree_t *cFolder = cfl[i];
    Serial.print("Dir ");
    Serial.println(cFolder->folderName->buffer);
  }
  showFiles(currentFolder->files);
}

void write(struct Vector *args) {
  if(args->size != 3) {
    Serial.println("Wrong number of args!");
  }
  else {
    struct Vector **arge = (struct Vector **)args->buffer;
    fd_AddFileTree(currentFolder, str_Init((char *)arge[1]->buffer), str_Init((char *)arge[2]->buffer));
  }
}

void check(struct Vector *args) {
  if(args->size != 2) {
    Serial.println("Wrong number of args!");
  }
  else {
    struct Vector **arge = (struct Vector **)args->buffer;
    struct StringElement *elem = str_Init((char *)arge[1]->buffer);
    struct FileContent_t *ft = fd_GetFileContent(currentFolder, elem);
    if(ft) {
      Serial.println(ft->value->buffer);
    }
    else {
      Serial.println("File not found!");
    }
    str_Delete(elem);
  }
}

void args_Delete(struct Vector *args) {
  struct Vector **arge = (struct Vector **)args->buffer;
  for(int8_t i = 0; i < args->size; i++) {
    free(arge[i]->buffer);
  }
  free(args->buffer);
  free(args);
}

void recieveInstruction(struct StringElement *instruction, struct Vector *args) {
  struct Vector **arge = (struct Vector **)args->buffer;
  if(!strcmp((char *)arge[0]->buffer, "where")) {
    where();
  }
  if(!strcmp((char *)arge[0]->buffer, "ls")) {
    ls();
  }
  if(!strcmp((char *)arge[0]->buffer, "cd")) {
    processCd(args, currentFolder->child);
    where();
  }
  if(!strcmp((char *)arge[0]->buffer, "touch")) {
    touch(args);
  }
  if(!strcmp((char *)arge[0]->buffer, "mkdir")) {
    mkdir(args);
  }
  if(!strcmp((char *)arge[0]->buffer, "write")) {
    write(args);
  }
  if(!strcmp((char *)arge[0]->buffer, "check")) {
    check(args);
  }
  args_Delete(args);
}

void loop() {
  char incomingByte = 0;
  String response = readInstruction();
  if(response != "") {
    struct StringElement *buf = str_Init(&response[0]);
    if(!isInstructionAcceptable(buf)) {
      Serial.println("Unknown instruction: " + response);
    }
    else {
      Serial.println(response);
      recieveInstruction(buf, arguments(buf));
    }
    str_Delete(buf);
  }
}