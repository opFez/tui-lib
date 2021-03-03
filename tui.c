#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "tui.h"

static struct termios orig_termios;

static const struct cell empty_cell = {
	' ',
	TUI_DEFAULT_FG,
	TUI_DEFAULT_BG
};

/* global cell buffer */
struct cell_buffer stdscr = {
	NULL,
	-1,  /* uninitialized */
	-1
};

static void
die(const char *s)
{
	perror(s);
	exit(1);
}

static void
tui_disable_raw_mode()
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

static void
tui_enable_raw_mode()
{
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");

	struct termios raw = orig_termios;
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

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

	if (height != NULL)
		*height = ws.ws_row;
	if (width != NULL)
		*width = ws.ws_col;
}

static void
init_cell_buffer(struct cell_buffer *cb)
{
	long ncells = cb->width * cb->height;
	cb->cells = malloc(ncells * sizeof(struct cell));
	for (long i = 0; i < ncells; i++)
		cb->cells[i] = empty_cell;
}

/****************************************************/

void
tui_init()
{
	tui_enable_raw_mode();
	get_window_size(&stdscr.height, &stdscr.width);
	init_cell_buffer(&stdscr);
	struct cell a = {
		'#',
		TUI_BLACK,
		TUI_WHITE
	};
	tui_clear(&stdscr, empty_cell);
	tui_clear(&stdscr, a);
	tui_refresh(&stdscr);
}

void
tui_shutdown()
{
	tui_disable_raw_mode();
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
tui_refresh(struct cell_buffer *cb)
{
	for (int y = 0; y < cb->height; y++) {
		for (int x = 0; x < cb->width; x++) {
			// currently doesn't handle cell's fg and bg
			long current_cell = x + y * cb->width;
			fprintf(stderr,
					"\033[mCell number %ld, fg: %d, bg: %d\r\n",
					current_cell,
					cb->cells[current_cell].fg,
					cb->cells[current_cell].bg);
			printf("\033[%d;%dm",
				   cb->cells[current_cell].fg + 29,
				   cb->cells[current_cell].bg + 39);
			byte buffer[1];
			buffer[0] = cb->cells[current_cell].ch;
			write(STDOUT_FILENO, buffer, 1);
		}
		write(STDOUT_FILENO, "\r\n", 2);
	}
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
tui_set_cell(struct cell_buffer *cb, int x, int y, struct cell c)
{
	if (x < 0 || y < 0 || x > cb->width || y > cb->height)
		return;
	cb->cells[x + y * cb->width] = c;
}
