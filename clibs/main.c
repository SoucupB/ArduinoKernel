#include <stdio.h>
#include "Kernel.h"
#include "ASMCompiler.h"

void firstTest() {
  processCommander(str_Init((char *)"ls"));
  processCommander(str_Init((char *)"mkdir NewDir"));
  processCommander(str_Init((char *)"ls"));
  processCommander(str_Init((char *)"cd NewDir"));
  processCommander(str_Init((char *)"touch test1.txt"));
  processCommander(str_Init((char *)"touch test2.txt"));
  processCommander(str_Init((char *)"touch test3.txt"));
  processCommander(str_Init((char *)"touch test4.txt"));
  processCommander(str_Init((char *)"mkdir Poze"));
  processCommander(str_Init((char *)"ls"));
}

void checkFileTest() {
  processCommander(str_Init((char *)"ls"));
  processCommander(str_Init((char *)"write Pancakes.txt 'new text ofcourse'"));
  processCommander(str_Init((char *)"check Pancakes.txt"));
}

void testCompiling() {
  processCommander(str_Init((char *)"ls"));
  processCommander(str_Init((char *)"touch main.asm"));
  processCommander(str_Init((char *)"write main.asm 'mov eax 15;mov edx 30;add eax edx;'"));
  processCommander(str_Init((char *)"ls"));
  processCommander(str_Init((char *)"compile main.asm"));
  processCommander(str_Init((char *)"ls"));
  processCommander(str_Init((char *)"run main.exe"));
}

int main() {
  setup();
  initKeyWords();
 // checkFileTest();
 // firstTest();
  testCompiling();
}