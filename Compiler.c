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

struct KeyWordsPair {
  struct StringElement *element;
  int8_t keyword;
};

#define EAX 0x0
#define EDX 0x4
#define ECX 0x8
#define EBX 0xC
#define AX 0x10
#define BX 0x11
#define DX 0x12
#define FX 0x13
#define NUMBER 0x14
#define ADD 0x15
#define SUB 0x16
#define MOV 0x17
#define CALL 0x18
#define UNDEFINED -0x1


struct KeyWordsPair keywords[12];

void initKeyWords() {
  keywords[0] = (struct KeyWordsPair){.element = str_Init((char *)"eax"), .keyword = EAX};
  keywords[1] = (struct KeyWordsPair){.element = str_Init((char *)"edx"), .keyword = EDX};
  keywords[2] = (struct KeyWordsPair){.element = str_Init((char *)"ecx"), .keyword = ECX};
  keywords[3] = (struct KeyWordsPair){.element = str_Init((char *)"ebx"), .keyword = EBX};
  keywords[4] = (struct KeyWordsPair){.element = str_Init((char *)"ax"), .keyword = AX};
  keywords[5] = (struct KeyWordsPair){.element = str_Init((char *)"bx"), .keyword = BX};
  keywords[6] = (struct KeyWordsPair){.element = str_Init((char *)"dx"), .keyword = DX};
  keywords[7] = (struct KeyWordsPair){.element = str_Init((char *)"fx"), .keyword = FX};
  keywords[8] = (struct KeyWordsPair){.element = str_Init((char *)"bx"), .keyword = BX};
  keywords[9] = (struct KeyWordsPair){.element = str_Init((char *)"add"), .keyword = ADD};
  keywords[10] = (struct KeyWordsPair){.element = str_Init((char *)"sub"), .keyword = SUB};
  keywords[11] = (struct KeyWordsPair){.element = str_Init((char *)"mov"), .keyword = MOV};
}

int8_t isNumber(struct StringElement *element) {
  int8_t index = 0;
  while(index < element->sz) {
    if(!(element->buffer[index] >= '0' && element->buffer[index] <= '9')) {
      return UNDEFINED;
    }
    index++;
  }
  return NUMBER;
}

int8_t getKeyFromWord(struct StringElement element) {
  for(int8_t i = 0; i < 12; i++) {
    if(str_Equal(keywords[i].element, &element)) {
      return keywords[i].keyword;
    }
  }
  return isNumber(&element);
}

int16_t getNumber(struct StringElement *element) {
  int8_t index = 0;
  int16_t sum = 0;
  while(index < element->sz) {
    sum *= 10;
    sum += element->buffer[index] - '0';
    index++;
  }
  return sum;
}

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
  char *lineBuffer = malloc(64);
  memset(lineBuffer, 0, 64);
  int16_t ind = 0;
  if(buffer->sz >= 64) {
    printf("Bytes overflow!\n");
    return NULL;
  }
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

void addInstruction(int16_t *streamIndex, void *byteStream, int8_t instruction, char *buffer) {
  memcpy(byteStream + (*streamIndex), &instruction, sizeof(int8_t));
  (*streamIndex)++;
  if(instruction == NUMBER) {
    struct StringElement toString = (struct StringElement){.buffer = buffer, .sz = strlen(buffer)};
    int16_t number = getNumber(&toString);
    memcpy(byteStream + *streamIndex, &number, sizeof(int16_t));
    (*streamIndex) += 2;
  }
}

void *compile(struct StringElement *buffer) {
  struct Vector *lines = lineTranslation(buffer);
  const uint8_t maxByteStream = 255;
  void *byteStream = malloc(maxByteStream);
  int16_t streamIndex = 0;
  if(!lines) {
    printf("Syntax error!\n");
    return NULL;
  }
  struct StringElement **lns = lines->buffer;
  for(int8_t i = 0; i < lines->size; i++) {
    struct Vector *buffers = arguments(lns[i]);
    struct Vector **arge = buffers->buffer;
    for(int8_t i = 0; i < buffers->size; i++) {
      struct StringElement instruction = (struct StringElement){.buffer = (char *)arge[i]->buffer, .sz = strlen((char *)arge[i]->buffer)};
      addInstruction(&streamIndex, byteStream, getKeyFromWord( instruction ), (char *)arge[i]->buffer);
    }
  }
  ((char *)byteStream)[streamIndex] = -1;
  // for(int32_t i = 0; i < streamIndex; i++) {
  //   printf("%d ", ((int8_t *)byteStream)[i]);
  // }
  return byteStream;
}

int16_t operation(void *byteStream, void *memory, int16_t index, int8_t registerInd, int8_t operation) {
  int8_t operand = ((int8_t *)byteStream)[index + 1];
  if(operand == NUMBER) {
    index++;
    if(registerInd < 0x10) {
      int32_t number = *((int16_t *)(byteStream + (++index)));
      int32_t registerContent = *(int32_t *)(memory + registerInd);
      int32_t regNumber;
      if(!operation) {
        regNumber = registerContent + number;
      }
      else {
        regNumber = registerContent - number;
      }
      memcpy(memory + registerInd, &regNumber, sizeof(int32_t));
    }
    else if(registerInd < 0x14) {
      int8_t number = *((int16_t *)(byteStream + (++index)));
      int8_t registerContent = *(int8_t *)(memory + registerInd);
      int8_t regNumber;
      if(!operation) {
        regNumber = registerContent + number;
      }
      else {
        regNumber = registerContent - number;
      }
      memcpy(memory + registerInd, &regNumber, sizeof(int8_t));
    }
    else {
      return UNDEFINED;
    }
    index += 2;
  }
  if(operand < 0x14) {
    if(registerInd < 0x10 && operand < 0x10) {
      int32_t firstReg = *(int32_t *)(memory + registerInd);
      int32_t secondReg = *(int32_t *)(memory + operand);
      int32_t regNumber;
      if(!operation) {
        regNumber = firstReg + secondReg;
      }
      else {
        regNumber = firstReg - secondReg;
      }
      memcpy(memory + registerInd, &regNumber, sizeof(int32_t));
      index += 2;
    }
    else if(registerInd >= 0x10 && operand >= 0x10) {
      int8_t firstReg = *(int8_t *)(memory + registerInd);
      int8_t secondReg = *(int8_t *)(memory + operand);
      int8_t regNumber;
      if(!operation) {
        regNumber = firstReg + secondReg;
      }
      else {
        regNumber = firstReg - secondReg;
      }
      memcpy(memory + registerInd, &regNumber, sizeof(int8_t));
      index += 2;
    }
    else {
      printf("Cannot cast registers of different types!\n");
      return UNDEFINED;
    }
  }
  return index;
}

int8_t mov(void *byteStream, void *memory, int16_t *index) {
  int16_t i = *index;
  int8_t instruction = ((int8_t *)byteStream)[i];
  if(instruction == MOV) {
    i++;
    int8_t fRegister = ((int8_t *)byteStream)[i];
    if(fRegister >= 0x14) {
      return UNDEFINED;
    }
    i++;
    int8_t instruction = ((int8_t *)byteStream)[i];
    if(instruction == NUMBER) {
      i++;
      if(fRegister < 0x10) {
        int32_t number = *(int16_t *)(byteStream + i);
        memcpy(memory + fRegister, &number, sizeof(int32_t));
      }
      else {
        int8_t number = *(int16_t *)(byteStream + i);
        memcpy(memory + fRegister, &number, sizeof(int8_t));
      }
      i += 2;
    }
    else {
      int8_t sRegister = ((int8_t *)byteStream)[i];
      if(fRegister < 0x10) {
        int32_t secondNumber = *(int32_t *)(memory + sRegister);
        memcpy(memory + fRegister, &secondNumber, sizeof(int32_t));
      }
      else {
        int8_t secondNumber = *(int8_t *)(memory + sRegister);
        memcpy(memory + fRegister, &secondNumber, sizeof(int8_t));
      }
      i += 2;
    }
  }
  *index = i;
  return 0;
}

int8_t additionSubstractionInstr(void *byteStream, void *memory, int16_t *index) {
  int16_t i = *index;
  int8_t instruction = ((int8_t *)byteStream)[i];
  if(instruction == ADD || instruction == SUB) {
    i++;
    int8_t operationType = (instruction == ADD ? 0 : 1);
    instruction = ((int8_t *)byteStream)[i];
    if(instruction == EAX || instruction == EDX || instruction == EBX || instruction == ECX ||
        instruction == AX || instruction == BX || instruction == DX || instruction == FX) {
        i = operation(byteStream, memory, i, instruction, operationType);
        if(i == UNDEFINED) {
          return UNDEFINED;
        }
    }
    else {
      printf("Seg fault can only write to registers!\n");
      return UNDEFINED;
    }
  }
  *index = i;
  return 0;
}

void printRegisterState(void *buffer) {
  printf("EAX %d\n", *(int32_t *)(buffer + EAX));
  printf("EDX %d\n", *(int32_t *)(buffer + EDX));
  printf("ECX %d\n", *(int32_t *)(buffer + ECX));
  printf("EBX %d\n", *(int32_t *)(buffer + EBX));
  printf("AX %d\n", *(int8_t *)(buffer + AX));
  printf("BX %d\n", *(int8_t *)(buffer + BX));
  printf("DX %d\n", *(int8_t *)(buffer + DX));
  printf("FX %d\n", *(int8_t *)(buffer + FX));
}

void run(void *byteStream) {
  int16_t i = 0;
  const int16_t memorySize = 400;
  void *memory = malloc(memorySize);
  memset(memory, 0, memorySize);
  for(;;) {
    int8_t lastI = i;
    int8_t response = additionSubstractionInstr(byteStream, memory, &i);
    if(response < 0) {
      free(memory);
      return ;
    }
    response = mov(byteStream, memory, &i);
    if(response < 0) {
      free(memory);
      return ;
    }
    int8_t instruction = ((int8_t *)byteStream)[i];
    if(instruction == UNDEFINED) {
      break;
    }
    if(lastI == i) {
      printRegisterState(memory);
      free(memory);
      return ;
    }
  }
  printRegisterState(memory);
  free(memory);
}

int main() {
  initKeyWords();
  void *byteStream = compile(str_Init((char *)"mov eax 15;mov edx 30;add eax edx;"));
  run(byteStream);
}