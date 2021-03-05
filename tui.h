#ifndef TUI_H
#define TUI_H

#include <stdint.h>
#include <stddef.h>

/* color constants */
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

/* character constants */
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

/* macro function for getting the value of a key if the ctrl key is held down */
#define TUI_CTRL_KEY(k) ((k) & 0x1f)

typedef unsigned char byte;


/* Structs */

/* One character in the buffer is called a cell.
 * It has a character representation (currently only ascii characters are 
 * allowed), a foreground color and a background color.
 */
struct cell {
	char ch; // ascii only
	uint16_t fg;
	uint16_t bg;
};

/* Internal representation of the screen.
 * It holds an array of cells, of length height * width.
 */
struct cell_buffer {
	struct cell *cells;
	int height;
	int width;
};

/* Return type of polling events.
 * The prefix field will be filled with an eventual ^[ (escape key hit). This
 * happens if inputting the alt-key as a modifier. This unfortunately means you
 * have to hit the escape key two times to register the escape key by itself.
 */
struct event {
	char prefix;
	char key;
};


/* Variables */

/* The empty cell, mostly used for clearing the screen. It has a character 
 * representation of an ascii space (' ') and default foreground and background 
 * colors.
 */
extern const struct cell empty_cell;

/* The default cell buffer is stdscr and is exposed by the library.
 * This is the unitialized state.
 */
extern struct cell_buffer stdscr;

/* cursor_visible, set to 1 if cursor is visible, 0 if not. */
extern int cursor_visible;


/* Functions */

/* tui_init() initializes stdscr and initializes raw mode.
 * tui_shutdown() disables raw mode.
 */
void tui_init();
void tui_shutdown();

/* returns the width and height of the current terminal window. */
int tui_width();
int tui_height();

/* tui_refresh() displays the contents of the cell buffer provided as the 
 * argument.
 * tui_refresh_cell() displays of a specific cell in the cell buffer provided
 * as the first argument. The second and third argument is the x and y
 * coordinates of the cell to refresh. This function is provided so one has the
 * ability to refresh a piece of the window, without refreshing the entire cell
 * buffer, which can lead to some graphical glitches on some terminals.
 *   WARNING: tui_refresh_cell() doesn't work properly.
 * tui_clear() fills the entire cellbuffer provided in the first argument with
 * the cell provided in the cell provided in the second argument. You can use 
 * the empty_cell variable to clear the cell buffer with a cell representing a 
 * ' ' with default foreground and background colors.
 * tui_clear_screen() clears the screen fast without affecting any cell buffer.
 */
void tui_refresh(struct cell_buffer);
void tui_refresh_cell(struct cell_buffer, int, int);
void tui_clear(struct cell_buffer *, struct cell);
void tui_clear_screen();

/* tui_set_cell() replaces the cell located at index x + y * cb->width in the
 * cell buffer provided in the first argument, effectively changing the cell at 
 * those coordinates.
 */
void tui_set_cell(struct cell_buffer *, int, int, struct cell);


/* tui_hide_cursor() makes the cursor invisible. Useful for drawing the screen.
 * tui_show_cursor() makes the cursor visible again.
 * tui_set_cursor() sets the position of the cursor to those given as arguments.
 * The first argument is the x-value and the second value is the y-value.
 */
void tui_hide_cursor();
void tui_show_cursor();
void tui_set_cursor(int, int);

/* tui_poll() waits for input, and returns the event it recieved */
struct event tui_poll();

#endif /* TUI_H */
