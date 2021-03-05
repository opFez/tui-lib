#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "tui.h"

static struct termios orig_termios;
static struct termios raw;

const struct cell empty_cell = {
	' ',
	TUI_DEFAULT_FG,
	TUI_DEFAULT_BG
};

/* global cell buffer */
struct cell_buffer stdscr = {
	NULL,
	-1,
	-1
};

int cursor_visible = 0;

static void
die(const char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

static void
disable_raw_mode()
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

static void
enable_raw_mode()
{
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");

	raw = orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

static void
get_window_size(int *height, int *width)
{
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
		die("ioctl");

	if (height != NULL)
		*height = ws.ws_row;
	if (width != NULL)
		*width = ws.ws_col;
}

static void
init_cell_buffer(struct cell_buffer *cb)
{
	long ncells = cb->width * cb->height;
	assert(ncells > 0);
	cb->cells = malloc(ncells * sizeof(struct cell));
	for (long i = 0; i < ncells; i++)
		cb->cells[i] = empty_cell;
}

static void
free_cell_buffer(struct cell_buffer *cb)
{
	free(cb->cells);
}

static void
save_cursor()
{
	write(STDOUT_FILENO, "\033[s", 3);
}

static void
restore_cursor()
{
	write(STDOUT_FILENO, "\033[u", 3);
}

/* kr */
static void
reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* kr */
static void
itoa(int n, char out[])
{
	int i, sign;

	if ((sign = n) < 0) /* record sign */
		n = -n;           /* make n positive */
	i = 0;
	do {  /* generate digits in reverse order */
		out[i++] = n % 10 + '0';  /* get next digit */
	} while ((n /= 10) > 0);   /* delete it */
	if (sign < 0)
		out[i++] = '-';
	out[i] = '\0';
	reverse(out);
}

/****************************************************/

void
tui_init()
{
	enable_raw_mode();
	tui_set_cursor(0, 0);
	get_window_size(&stdscr.height, &stdscr.width);
	init_cell_buffer(&stdscr);
	tui_clear(&stdscr, empty_cell);
}

void
tui_shutdown()
{
	free_cell_buffer(&stdscr);
	disable_raw_mode();
	tui_set_cursor(0, 0);
	tui_clear_screen();
	if (!cursor_visible)
		tui_show_cursor();
}

int
tui_width()
{
	int width;
	get_window_size(NULL, &width);
	return width;
}

int
tui_height()
{
	int height;
	get_window_size(&height, NULL);
	return height;
}

void
tui_refresh(struct cell_buffer cb)
{
	if (cursor_visible)
		tui_hide_cursor();
	save_cursor();
	write(STDOUT_FILENO, "\033[0;0H", 6);
	for (int y = 0; y < cb.height; y++) {
		for (int x = 0; x < cb.width; x++) {
			// currently doesn't handle cell's fg and bg
			/* printf("\033[%d;%dm", */
			/* 	   cb->cells[current_cell].fg + 29, */
			/* 	   cb->cells[current_cell].bg + 39); */
			long current_cell = x + y * cb.width;
			write(STDOUT_FILENO, &cb.cells[current_cell].ch, 1);
		}
		if (y != cb.height - 1)
			write(STDOUT_FILENO, "\r\n", 2);
	}
	restore_cursor();
	if (cursor_visible)
		tui_show_cursor();
	fflush(stdout);
}

void
tui_refresh_cell(struct cell_buffer cb, int x, int y)
{
	tui_hide_cursor();
	save_cursor();

	tui_set_cursor(x, y);
	write(STDOUT_FILENO, &cb.cells[x + y * cb.width].ch, 1);

	restore_cursor();
	tui_show_cursor();
}

void
tui_clear(struct cell_buffer *cb, struct cell clear_cell)
{
	long ncells = cb->width * cb->height;
	for (long i = 0; i < ncells; i++) {
		cb->cells[i] = clear_cell;
	}
}

void
tui_clear_screen()
{
	write(STDOUT_FILENO, "\033[2J", 4);
}

void
tui_set_cell(struct cell_buffer *cb, int x, int y, struct cell c)
{
	if (x < 0 || y < 0 || x > cb->width || y > cb->height)
		return;
	cb->cells[x + y * cb->width] = c;
}

void
tui_hide_cursor()
{
	cursor_visible = 0;
	write(STDOUT_FILENO, "\033[?25l", 6);
}

void
tui_show_cursor()
{
	cursor_visible = 1;
	write(STDOUT_FILENO, "\033[?25h", 6);
}

void
tui_set_cursor(int x, int y)
{
	/* It looks like x and y has to be 1-indexed, so correct this by adding 1 to
	 * each of them. */
	char xcoord[8] = {0};
	itoa(x + 1, xcoord);
	char ycoord[8] = {0};
	itoa(y + 1, ycoord);

	write(STDOUT_FILENO, "\033[", 2);
	write(STDOUT_FILENO, ycoord, strlen(xcoord));
	write(STDOUT_FILENO, ";", 1);
	write(STDOUT_FILENO, xcoord, strlen(ycoord));
	write(STDOUT_FILENO, "H", 1);
}

/* can probably be simplified, but it works well */
struct event
tui_poll()
{
	char c;

	/* change attributes to make the program wait for input */
	raw.c_cc[VMIN] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");

	read(STDIN_FILENO, &c, 1);
	if (c == TUI_KEY_ESC) {
		read(STDIN_FILENO, &c, 1);

		/* reset attributes */
		raw.c_cc[VMIN] = 0;
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
			die("tcsetattr");

		return (struct event) {
			TUI_KEY_ESC, c
		};
	}
	else {
		/* reset attributes */
		raw.c_cc[VMIN] = 0;
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
			die("tcsetattr");

		return (struct event) {
			0, c
		};
	}
}

struct event
tui_peek()
{
	char c;

	read(STDIN_FILENO, &c, 1);
	if (c == TUI_KEY_ESC) {
		read(STDIN_FILENO, &c, 1);

		return (struct event) {
			TUI_KEY_ESC, c
		};
	}
	else {
		return (struct event) {
			0, c
		};
	}
}
