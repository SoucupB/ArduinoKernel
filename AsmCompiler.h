#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct Vector;
struct StringElement;
struct FileContent_t;
struct FileTree_t;

struct Vector *vct_Init(int8_t elementSize);
struct StringElement *str_Init(char *buffer);
uint8_t str_Equal(struct StringElement *a, struct StringElement *b);
void vct_Push(struct Vector *vct, void *element);
void str_Delete(struct StringElement *self);
struct Vector *arguments(struct StringElement *instruction);
uint8_t str_Equal(struct StringElement *a, struct StringElement *b);
void args_Delete(struct Vector *args);
void setup();
void processCommander(struct StringElement *element);
void vct_Delete(struct Vector *self);