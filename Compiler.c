#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "AsmCompiler.h"

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

#define EAX 0x0
#define EDX 0x4
#define ECX 0x8
#define EFX 0xC
#define AX 0x10
#define BX 0x11
#define DX 0x12
#define FX 0x13

void getData(struct StringElement *buffer) {
  struct Vector *vector = arguments(buffer);
  struct Vector **arge = (struct Vector **)vector->buffer;
  for(int8_t i = 0; i < vector->size; i++) {
    printf("%s\n", (char *)arge[i]->buffer);
  }
}

void removeSpaces(struct StringElement *buffer, int16_t *index) {
  while(*index < buffer->sz && buffer->buffer[*index] == ' ') {
    (*index)++;
  }
}

struct StringElement *line(struct StringElement *buffer, int16_t *index) {
  char *lineBuffer = malloc(32);
  memset(lineBuffer, 0, 32);
  int16_t ind = 0;
  removeSpaces(buffer, index);
  while(*index < buffer->sz && buffer->buffer[*index] != ';') {
    lineBuffer[ind++] = buffer->buffer[*index];
    (*index)++;
  }
  struct StringElement *tempBuffer = str_Init(lineBuffer);
  free(lineBuffer);
  return tempBuffer;
}

struct Vector *lineTranslation(struct StringElement *buffer) {
  struct Vector *lines = vct_Init(sizeof(struct StringElement *));
  for(int16_t i = 0; i < buffer->sz; i++) {
    struct StringElement *ln = line(buffer, &i);
    vct_Push(lines, &ln);
  }
  return lines;
}

void *compile(struct StringElement *buffer) {
  struct Vector *lines = lineTranslation(buffer);
  if(!lines) {
    printf("Syntax error!\n");
    return NULL;
  }
  struct StringElement **lns = lines->buffer;
  for(int8_t i = 0; i < lines->size; i++) {
    printf("%s\n", lns[i]->buffer);
  }
}


int main() {
  compile(str_Init((char *)"add eax 343;sub eax edx;"));
}