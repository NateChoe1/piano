#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <sound.h>
#include <evdev.h>
#include <config.h>

#define LEN(array) (sizeof(array) / sizeof(array[0]))

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: piano [/dev/input/eventX]\n");
		exit(EXIT_FAILURE);
	}

	argv++;
	argc--;

	Keyboard **boards = malloc(sizeof(Keyboard *) * argc);

	for (int i = 0; i < argc; i++) {
		boards[i] = grabKeyboard(argv[i]);
		if (boards[i] == NULL) {
			fprintf(stderr, "Failed to grab device %s\n", argv[i]);
			exit(EXIT_FAILURE);
		}
	}

	unsigned short scale[]={
		KEY_TAB,
		KEY_1,
		KEY_Q,
		KEY_2,
		KEY_W,
		KEY_E,
		KEY_4,
		KEY_R,
		KEY_5,
		KEY_T,
		KEY_6,
		KEY_Y,
		KEY_U,
		KEY_8,
		KEY_I,
		KEY_9,
		KEY_O,
		KEY_P,
		KEY_MINUS,
		KEY_LEFTBRACE,
		KEY_EQUAL,
		KEY_RIGHTBRACE,
		KEY_BACKSPACE,
		KEY_BACKSLASH,
	};
	//

	long frame = 0;
	Speaker speaker = createSpeaker(argv[0], 44100, 2);
	for (;;) {
		int16_t moment = 0;
		for (int i = 0; i < argc; i++) {
			Keyboard *keyboard = boards[i];
			for (int j = 0; j < LEN(scale); j++) {
				if (isPressed(keyboard, scale[j])) {
					int note = j - 9 + (-argc / 2 + i) * LEN(scale);
					double freq = pow(2, (note) / 12.0) * 440;
					moment += sin(M_PI * 2 * frame * freq / 44100) * 1000;
				}
			}
		}
		Moment output = {moment, moment};
		sendSpeaker(speaker, &output, 1);
		frame++;
	}

	return 0;
}
