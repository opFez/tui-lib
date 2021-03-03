#ifndef TUI_H
#define TUI_H

#include <stdint.h>

#define TUI_DEFAULT 0x00
#define TUI_BLACK   0x01
#define TUI_RED     0x02
#define TUI_GREEN   0x03
#define TUI_YELLOW  0x04
#define TUI_BLUE    0x05
#define TUI_MAGENTA 0x06
#define TUI_CYAN    0x07
#define TUI_WHITE   0x08

#define TUI_DEFAULT_FG TUI_DEFAULT
#define TUI_DEFAULT_BG TUI_BLACK

#define TUI_KEY_CTRL_TILDE       0x00
#define TUI_KEY_CTRL_2           0x00
#define TUI_KEY_CTRL_A           0x01
#define TUI_KEY_CTRL_B           0x02
#define TUI_KEY_CTRL_C           0x03
#define TUI_KEY_CTRL_D           0x04
#define TUI_KEY_CTRL_E           0x05
#define TUI_KEY_CTRL_F           0x06
#define TUI_KEY_CTRL_G           0x07
#define TUI_KEY_BACKSPACE        0x08
#define TUI_KEY_CTRL_H           0x08
#define TUI_KEY_TAB              0x09
#define TUI_KEY_CTRL_I           0x09
#define TUI_KEY_CTRL_J           0x0A
#define TUI_KEY_CTRL_K           0x0B
#define TUI_KEY_CTRL_L           0x0C
#define TUI_KEY_ENTER            0x0D
#define TUI_KEY_CTRL_M           0x0D
#define TUI_KEY_CTRL_N           0x0E
#define TUI_KEY_CTRL_O           0x0F
#define TUI_KEY_CTRL_P           0x10
#define TUI_KEY_CTRL_Q           0x11
#define TUI_KEY_CTRL_R           0x12
#define TUI_KEY_CTRL_S           0x13
#define TUI_KEY_CTRL_T           0x14
#define TUI_KEY_CTRL_U           0x15
#define TUI_KEY_CTRL_V           0x16
#define TUI_KEY_CTRL_W           0x17
#define TUI_KEY_CTRL_X           0x18
#define TUI_KEY_CTRL_Y           0x19
#define TUI_KEY_CTRL_Z           0x1A
#define TUI_KEY_ESC              0x1B
#define TUI_KEY_CTRL_LSQ_BRACKET 0x1B
#define TUI_KEY_CTRL_3           0x1B
#define TUI_KEY_CTRL_4           0x1C
#define TUI_KEY_CTRL_BACKSLASH   0x1C
#define TUI_KEY_CTRL_5           0x1D
#define TUI_KEY_CTRL_RSQ_BRACKET 0x1D
#define TUI_KEY_CTRL_6           0x1E
#define TUI_KEY_CTRL_7           0x1F
#define TUI_KEY_CTRL_SLASH       0x1F
#define TUI_KEY_CTRL_UNDERSCORE  0x1F
#define TUI_KEY_SPACE            0x20
#define TUI_KEY_BACKSPACE2       0x7F
#define TUI_KEY_CTRL_8           0x7F

#define TUI_CTRL_KEY(k) ((k) & 0x1f)

typedef unsigned char byte;

struct cell {
	byte ch; // ascii only
	uint16_t fg;
	uint16_t bg;
};

struct cell_buffer {
	struct cell *cells;
	int height;
	int width;
};

void tui_init();
void tui_shutdown();

int tui_width();
int tui_height();

void tui_refresh();
void tui_clear();

void tui_set_cell();

#endif /* TUI_H */
