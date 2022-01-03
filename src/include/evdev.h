#include <limits.h>
#include <stdint.h>
#include <linux/input.h>

typedef struct {
	int fd;
	uint8_t pressedKeys[USHRT_MAX / 8];
} Keyboard;

Keyboard *grabKeyboard(char *path);
//returns NULL on error
void freeKeyboard(Keyboard *keyboard);
int isPressed(Keyboard *keyboard, unsigned short key);
