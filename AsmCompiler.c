#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define Serialprintln(a) printf("%s\n", a);

struct Vector {
  void *buffer;
  int32_t capacity;
  int32_t size;
  int8_t elementsSize;
};

struct StringElement {
  char *buffer;
  int8_t sz;
};
int8_t instrLength = 7;
struct StringElement *instructions[7];

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

void init() {
  instructions[0] = str_Init((char *)"cd");
  instructions[1] = str_Init((char *)"ls");
  instructions[2] = str_Init((char *)"touch");
  instructions[3] = str_Init((char *)"compile");
  instructions[4] = str_Init((char *)"mkdir");
  instructions[5] = str_Init((char *)"write");
  instructions[6] = str_Init((char *)"where");
}

void vct_PushElement(struct Vector *vct, void *element) {
  memcpy(vct->buffer + vct->size * vct->elementsSize, element, vct->elementsSize);
  vct->size++;
}

void vct_Push(struct Vector *vct, void *element) {
  if(vct->size >= vct->capacity) {
    vct->capacity += 5;
    void *buffer = malloc(vct->elementsSize * vct->capacity);
    memcpy(buffer, vct->buffer, vct->size * vct->elementsSize);
    free(vct->buffer);
    vct->buffer = buffer;
  }
  vct_PushElement(vct, element);
}

struct FileContent_t *tr_AddFile(struct FileContent_t *file, struct StringElement *fileName,
                  				       struct StringElement *content) {
  if(!file) {
    struct FileContent_t *self = (struct FileContent_t *)malloc
      							 (sizeof(struct FileContent_t));
    self->left = 0;
    self->right = 0;
    self->key = fileName;
    self->value = content;
    return self;
  }
  if(file->key > fileName) {
    file->left = tr_AddFile(file->left, fileName, content);
  }
  else {
    file->right = tr_AddFile(file->right, fileName, content);
  }
  return file;
}

void fd_AddFileTree(struct FileTree_t* folder,
                    struct StringElement *fileName, struct StringElement *content) {
  folder->files = tr_AddFile(folder->files, fileName, content);
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

struct StringElement *tr_Find(struct FileContent_t *self, struct StringElement *fileName) {
  if(!self) {
    return str_Init((char *)"No such file exists!");
  }
  if(!strcmp(self->key->buffer, fileName->buffer)) {
    return self->value;
  }
  if(self->key > fileName) {
    return tr_Find(self->left, fileName);
  }
  return tr_Find(self->right, fileName);
}

void showFiles(struct FileContent_t* folder) {
  if(!folder) {
    return ;
  }
  showFiles(folder->left);
  printf("%s\n", folder->key->buffer);
  showFiles(folder->right);
}

struct StringElement *fd_GetFileContent(struct FileTree_t* file, struct StringElement *fileName) {
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

struct Vector *arguments(struct StringElement *instruction) {
  struct Vector *argve = vct_Init(sizeof(struct Vector *));
  for(int8_t i = 0; i < instruction->sz; i++) {
    char *tempBuffer = (char *)malloc(16);
    int8_t index = 0;
    memset(tempBuffer, 0, 16);
    struct Vector *arg = vct_Init(sizeof(char *));
    while(i < instruction->sz && instruction->buffer[i] != ' ') {
      tempBuffer[index++] = instruction->buffer[i];
      i++;
    }
    struct StringElement *argument = str_Init(tempBuffer);
    free(tempBuffer);
    char *heapAllocator = (char *)malloc(argument->sz + 1);
    memset(heapAllocator, 0, argument->sz + 1);
    memcpy(heapAllocator, argument->buffer, argument->sz);
    arg->buffer = heapAllocator;
    arg->size = argument->sz;
    arg->capacity = argument->sz;
    vct_Push(argve, &arg);
  }
  return argve;
}

void showArguments(struct Vector *argve) {
  struct Vector **args = (struct Vector **)argve->buffer;
  for(int8_t i = 0; i < argve->size; i++) {
    char *buffer = (char *)args[i]->buffer;
    Serialprintln(buffer);
  }
}

void processCd(struct Vector *argv, struct Vector *folders) {
  struct Vector **args = (struct Vector **)argv->buffer;
  if(argv->size != 2) {
    Serialprintln("Invalid number of arguments for CD");
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
  Serialprintln("No such directory exists!");
}

void testing() {
  struct FileTree_t *folder = createSubfolder(currentFolder, str_Init((char *)"Programfiles"));
  fd_AddFileTree(currentFolder, str_Init((char *)"personal.txt"), str_Init((char *)"Go to buy some milk!"));
  createSubfolder(folder, str_Init((char *)"Projects"));
  createSubfolder(folder, str_Init((char *)"Disfunction"));
  createSubfolder(folder, str_Init((char *)"Maps"));
  fd_AddFileTree(currentFolder, str_Init((char *)"toDo.txt"), str_Init("Finish some homework!"));
  fd_AddFileTree(currentFolder, str_Init((char *)"Pancakes.txt"), str_Init("DDAA!"));
  //printf("%s\n", (fd_GetFileContent(currentFolder, str_Init("Pancakes.txt")))->buffer);
  createSubfolder(currentFolder, str_Init((char *)"Music"));
  struct FileTree_t *movies = createSubfolder(currentFolder, str_Init((char *)"Movies"));
  createSubfolder(movies, str_Init((char *)"Action"));
  createSubfolder(movies, str_Init((char *)"Romance"));
  createSubfolder(movies, str_Init((char *)"SF"));
}

uint8_t str_Compare(struct StringElement *a, struct StringElement *b) {
  return a->sz == b->sz && !strncmp(a->buffer, b->buffer, a->sz);
}

uint8_t isInstructionAcceptable(struct StringElement *instr) {
  struct StringElement *getArgInst = getInstr(instr);
  for(int8_t i = 0; i < instrLength; i++) {
    if(str_Compare(getArgInst, instructions[i])) {
      free(getArgInst);
      return 1;
    }
  }
  free(getArgInst);
  return 0;
}

void setup() {
  //Serial.begin(9600);
  struct StringElement *parentDirectory = str_Init((char *)"C:");
  init();
  Serialprintln("Started!");
  folderStack = vct_Init(sizeof(struct FileTree_t *));
  currentFolder = fd_Init(parentDirectory);
  testing();
  vct_Push(folderStack, &currentFolder);
  Serialprintln("Done");
}

void where() {
  struct FileTree_t **stack = ((struct FileTree_t **)folderStack->buffer);
  for(int32_t i = 0; i < folderStack->size; i++) {
    struct FileTree_t *cFolder = stack[i];
    printf("%s", cFolder->folderName->buffer);
    printf("/");
  }
  printf("\n");
}

void ls() {
  struct Vector *folders = ((struct Vector *)currentFolder->child);
  struct FileTree_t **cfl = (struct FileTree_t **)((struct Vector *)folders)->buffer;
  for(int32_t i = 0; i < folders->size; i++) {
    struct FileTree_t *cFolder = cfl[i];
    printf("%s\n", cFolder->folderName->buffer);
  }
  showFiles(currentFolder->files);
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
}

int main() {
  setup();
  struct StringElement *argument = str_Init((char *)"where");
  if(isInstructionAcceptable(argument)) {
    recieveInstruction(argument, arguments(argument));
  }
  else {
    printf("Not good!\n");
  }
  //struct StringElement *arg = str_Init((char *)"ls");
 // recieveInstruction(arg, arguments(arg));
}