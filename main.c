#include <stdio.h>
#include "AsmCompiler.h"

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

int main() {
  setup();
  checkFileTest();
  firstTest();
}