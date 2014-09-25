#pragma once
#include "game.h"
#include "memory.h"
#include "collection.h"
#include "SDL_keycode.h"

/*#define INPUT_BITS_UP     1
#define INPUT_BITS_DOWN   2
#define INPUT_BITS_LEFT   4
#define INPUT_BITS_RIGHT  8
#define INPUT_BITS_SPACE  16*/

#define INPUT_OPERATE_UP_DOWN 201
#define INPUT_OPERATE_DOWN_DOWN 202
#define INPUT_OPERATE_LEFT_DOWN 203
#define INPUT_OPERATE_RIGHT_DOWN 204
#define INPUT_OPERATE_SPACE_DOWN 205
#define INPUT_OPERATE_UP_UP 211
#define INPUT_OPERATE_DOWN_UP 212
#define INPUT_OPERATE_LEFT_UP 213
#define INPUT_OPERATE_RIGHT_UP 214
#define INPUT_OPERATE_SPACE_UP 215

struct implOSM
{
	LinkedList *linkedList;
	unsigned long delayTime;
};

int IN_InitInput();
void IN_DestroyInput();
BOOL IN_KeyDown(SDL_Keycode keynum);
BOOL IN_KeyUp(SDL_Keycode keynum);
unsigned char IN_GetOperate();
void IN_UpdateInput();
void IN_Clear();
void IN_MouseDown(long x,long y);
BOOL IN_TextInputEnable(long x,long y,long w,long h);
void IN_TextInputDisable();
void IN_TextInputChar(char *str);
BOOL IN_TextInputChanged();
void IN_TextInputGet(char *dest);
