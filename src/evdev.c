#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/ioctl.h>

#include <evdev.h>

Keyboard *grabKeyboard(char *path) {
	Keyboard *ret = malloc(sizeof(Keyboard));
	ret->fd = open(path, O_RDONLY | O_NONBLOCK);
	if (ret->fd < 0) {
		free(ret);
		return NULL;
	}
	if (ioctl(ret->fd, EVIOCGRAB, 1)) {
		close(ret->fd);
		free(ret);
		return NULL;
	}
	memset(ret->pressedKeys, 0, sizeof(ret->pressedKeys));
	return ret;
}

void freeKeyboard(Keyboard *keyboard) {
	close(keyboard->fd);
	free(keyboard);
}

int isPressed(Keyboard *keyboard, unsigned short key) {
	for (;;) {
		struct input_event event;
		long readInput = read(keyboard->fd, &event, sizeof(event));
		if (readInput < (long) sizeof(event))
			break;
		//sizeof() returns an unsigned value
		if (event.type == EV_SYN)
			break;
		if (event.type == EV_CNT)
			break;
		//printf("%d %d\n", event.type, EV_CNT);
		if (event.type != EV_KEY)
			continue;
		uint8_t mask = 1 << (event.code % 8);
		switch (event.value) {
			case 0:
				keyboard->pressedKeys[event.code / 8] ^= mask;
				break;
			case 1:
				keyboard->pressedKeys[event.code / 8] |= mask;
				break;
		}
	}
	return keyboard->pressedKeys[key / 8] & (1 << (key % 8));
}
