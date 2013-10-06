#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xresource.h>

#define SAVE_SECONDS_INTERVAL 60 // save every N seconds

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2

#define sizeof_member(type, member) sizeof(((type *)0)->member)

struct KeyCountElement {
	KeySym keysym;
	int    keyPresses;
	struct KeyCountElement* next;
};

void 	ProcessInputEvents(int fd, void(*eventHandler)(struct input_event* event));

void 	HandleKeyboardInputEvent(struct input_event* event);
void 	HandleMouseInputEvent(struct input_event* event);

int 	GetKeysymFromKeycode(int keycode);
char* 	GetKeysymString(int keycode);

void 					KeycountWrite();
struct KeyCountElement* KeycountGet(KeySym key);
int 					KeycountGetNonZero();
void 					KeycountSetZero();
void 					KeycountFree();

// const char* keyboardDevice = "/dev/input/event16";
// const char* mouseDevice = "/dev/input/event18";
const char* keyboardDevice = "/dev/input/event0";
const char* mouseDevice = "/dev/input/event20";