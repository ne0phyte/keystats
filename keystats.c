#include "keystats.h"

Display* display = NULL;
int min_keycode, max_keycode;

struct KeyCountElement* keycountHead = NULL;
struct KeyCountElement* keycountTail = NULL;

char keystate[256];

int main (int argc, char *argv[]) {
	int mouseFd, kbdFd;
	clock_t seconds, prevSeconds, diff;

	if ((getuid ()) != 0)
		printf ("You are not root! This may not work...\n");

  // open display
	display = XOpenDisplay(NULL);
	if (!display) {
		printf("Can't open display!\n");
		exit(0);
	}
  // get keycode range
	XDisplayKeycodes (display, &min_keycode, &max_keycode);


  //Open Devices
	if ((kbdFd = open (keyboardDevice, O_RDONLY | O_NONBLOCK)) == -1)
		printf ("%s is not a vaild device.\n", keyboardDevice);
	
	if ((mouseFd = open (mouseDevice, O_RDONLY | O_NONBLOCK)) == -1)
		printf ("%s is not a vaild device.\n", mouseDevice);

  //Print Device Name
	{
		char name[256] = "Unknown";
		ioctl (kbdFd, EVIOCGNAME (sizeof (name)), name);
		printf ("Reading from : %s (%s)\n", keyboardDevice, name);

		ioctl (mouseFd, EVIOCGNAME (sizeof (name)), name);
		printf ("Reading from : %s (%s)\n", mouseDevice, name);

		seconds = prevSeconds = time(NULL);
	}

	while (1){

		ProcessInputEvents(kbdFd, &HandleKeyboardInputEvent);
		// ProcessInputEvents(mouseFd, &HandleMouseInputEvent);

		sleep(1);

		seconds = time(NULL);
		diff = seconds - prevSeconds;
		// > interval reached
		if ( diff > SAVE_SECONDS_INTERVAL) {
			prevSeconds = seconds;
			// the time will be way over 60s after a resume from standby
			// prevents saving after standby
			if (diff - SAVE_SECONDS_INTERVAL < 5) {
				KeycountWrite();
				KeycountFree(); // remove all elements
			}
		
		}
	}

	XCloseDisplay(display);
	return 0;
}

void
ProcessInputEvents(int fd, void(*eventHandler)(struct input_event* event)) {
		struct input_event ev[64];
		int i, bytesRead;

		bytesRead = read(fd, ev, sizeof(ev));
		if (bytesRead == 0) {
			perror ("Unable to read() mouse event device!\n");
			exit(0);
		}

		for (i = 0; i < (int) (bytesRead / sizeof (struct input_event)); i++) {
			eventHandler(&ev[i]);
		}
}

void
HandleKeyboardInputEvent(struct input_event* event) {
	int keysym = GetKeysymFromKeycode(event->code);
	char* string = GetKeysymString(keysym);
	struct KeyCountElement* element = KeycountGet(keysym);

	// state changed
	if (event->value != keystate[event->code]) {
		keystate[event->code] = event->value;
		if (event->value == KEY_PRESS) {
			element->keyPresses++;

			printf("Key %s %d Sy %dz #%d\n", string, event->code, keysym, element->keyPresses);
		} else if (event->value == KEY_RELEASE) {
			// 
		}
	}
}

void
HandleMouseInputEvent(struct input_event* event) {
	printf("%d\n", event->value);
	if (event->value == KEY_PRESS) {
		//
	} else if (event->value == KEY_RELEASE) {
		//
	}
}

int
GetKeysymFromKeycode(int keycode) {
	int keysyms_per_keycode_return;

	KeySym *keysym = XGetKeyboardMapping(display,
		keycode + min_keycode,
		1,
		&keysyms_per_keycode_return);

	int key = keysym[0];
	XFree(keysym);

	return key;
}

char*
GetKeysymString(int keysym) {
	if (keysym) {
		char* string = XKeysymToString(keysym);
		return string;
	}
	return NULL;
}

// ----- Keycount linked list for storage -----

void
KeycountWrite() {
	FILE* file = NULL;
	int keycountsToWrite = KeycountGetNonZero();
	int timestamp = (int) time(NULL);
	struct KeyCountElement* element = keycountHead;

	printf("TO WRITE %d\n", keycountsToWrite);

	// header: amount of keycount sets, timestamp
	if ( (file = fopen("keystats.dat","a+")) == NULL) {
		return;
	}

	fwrite( (const void*)&keycountsToWrite, sizeof(int), 1, file);
	fwrite( (const void*)&timestamp, sizeof(int), 1, file);
	if (keycountsToWrite > 0) {
		while (element != NULL) {
			if (element->keyPresses > 0) {
				// save without KeyCountElement.next (linked list reference)
				fwrite( &(element->keysym), 
					sizeof(struct KeyCountElement) - sizeof(struct KeyCountElement*),
					1, file);
			}
			element = element->next;
		}
	}
	fclose(file);
}

struct KeyCountElement*
KeycountGet(KeySym key) {
	struct KeyCountElement* element = keycountHead;

	while (element != NULL) {
		if (element->keysym == key) {
			return element;
		}
		element = element->next;
	}

	if (element == NULL) {
		element = calloc(1, sizeof(struct KeyCountElement));
		element->keysym = key;
	}

	if (keycountHead == NULL) {
		keycountHead = keycountTail = element;
	} else {
		keycountTail->next = element;
		keycountTail = element;
	}
	return element;
}

int
KeycountGetNonZero() {
	int count = 0;
	struct KeyCountElement* element = keycountHead;

	while (element != NULL) {
		if (element->keyPresses > 0) {
			count++;
		}
		element = element->next;
	}
	return count;
}

void
KeycountSetZero() {
	struct KeyCountElement* element = keycountHead;

	while (element != NULL) {
		element->keyPresses = 0;
		element = element->next;
	}
}

void
KeycountFree() {
	struct KeyCountElement* element = keycountHead;
	struct KeyCountElement* temp = NULL;

	while (element != NULL) {
		temp = element->next;
		free(element);
		element = temp;
	}
	keycountHead = keycountTail = NULL;
}