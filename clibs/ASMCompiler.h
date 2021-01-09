#pragma once

struct KeyWordsPair {
  struct StringElement *element;
  int8_t keyword;
};

void *compile(struct StringElement *buffer);
void run(void *byteStream);
void initKeyWords();