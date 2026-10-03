#ifndef TCTL_H
#define TCTL_H

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace tc {

inline void init(void)
{
#ifdef _WIN32
	HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (h_out != INVALID_HANDLE_VALUE) {
		DWORD mode = 0;
		if (GetConsoleMode(h_out, &mode))
			SetConsoleMode(h_out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
#endif
}

inline void flush(void)
{
	fflush(stdout);
}

inline void write(const char *s)
{
	fputs(s, stdout);
}

inline void write_char(char c)
{
	fputc(c, stdout);
}

inline void seq(const char *s)
{

	fputs(s, stdout);
}

inline void seqn(const char *prefix, int n, char suffix)
{
	char buf[32];
	snprintf(buf, sizeof(buf), "%s%d%c", prefix, n, suffix);
	fputs(buf, stdout);
}

inline void cursor_left(int n)
{
	seqn("\x1b[", n, 'D');
}

inline void cursor_right(int n)
{
	seqn("\x1b[", n, 'C');
}

inline void cursor_home(void)
{
	seq("\r");
}

inline void cursor_line_end(void)
{
	seqn("\x1b[", 999, 'C');
}

inline void cursor_to_col(int col)
{
	seqn("\x1b[", col, 'G');
}

inline void clear_line(void)
{
	seq("\x1b[2K\r");
}

inline void clear_to_end(void)
{
	seq("\x1b[K");
}

inline void clear_to_start(void)
{
	seq("\x1b[1K");
}

inline void erase_chars(int n)
{
	if (n > 0)
		seqn("\x1b[", n, 'P');
}

constexpr int KEY_ESC = 0x1b;
constexpr int KEY_RETURN = 0x0d;
constexpr int KEY_BACKSPACE = 0x7f;
constexpr int KEY_UP = 1000;
constexpr int KEY_DOWN = 1001;
constexpr int KEY_LEFT = 1002;
constexpr int KEY_RIGHT = 1003;

#ifdef _WIN32

inline int getch_raw(void)
{
	if (!_kbhit())
		return -1;
	return _getch();
}

inline int enable_raw_mode(void)
{
	return 0;
}

inline void disable_raw_mode(void) {}

#else

static int g_raw = 0;
static struct termios g_orig;

inline int getch_raw(void)
{
	unsigned char c;
	if (read(STDIN_FILENO, &c, 1) != 1)
		return -1;
	return c;
}

inline int read_byte_timeout(unsigned char *c, long usec)
{
	fd_set set;
	struct timeval tv = {0, usec};
	int r;

	FD_ZERO(&set);
	FD_SET(STDIN_FILENO, &set);
	if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) > 0) {
		r = getch_raw();
		if (r >= 0) {
			*c = (unsigned char)r;
			return 1;
		}
	}
	return 0;
}

inline int enable_raw_mode(void)
{
	struct termios raw;
	extern void disable_raw_mode(void);

	if (g_raw)
		return 0;
	if (tcgetattr(STDIN_FILENO, &g_orig) == -1)
		return -1;
	raw = g_orig;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_cflag |= CS8;
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		return -1;
	g_raw = 1;
	atexit(disable_raw_mode);
	return 0;
}

inline void disable_raw_mode(void)
{
	if (g_raw) {
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig);
		g_raw = 0;
	}
}

#endif /* _WIN32 */

inline int getch(void)
{
	int c = getch_raw();

	if (c == -1)
		return -1;
#ifdef _WIN32
	if (c == 0 || c == 0xe0) {
		switch (getch_raw()) {
		case 72:
			return KEY_UP;
		case 80:
			return KEY_DOWN;
		case 75:
			return KEY_LEFT;
		case 77:
			return KEY_RIGHT;
		default:
			return KEY_ESC;
		}
	}
	if (c == 8)
		return KEY_BACKSPACE;
	if (c == KEY_RETURN)
		return KEY_RETURN;
	return c;
#else
	if (c == 0x1b) {
		unsigned char seq[2];

		if (read_byte_timeout(&seq[0], 50000) && (seq[0] == '[' || seq[0] == 'O') && read_byte_timeout(&seq[1], 50000)) {
			switch (seq[1]) {
			case 'A':
				return KEY_UP;
			case 'B':
				return KEY_DOWN;
			case 'C':
				return KEY_RIGHT;
			case 'D':
				return KEY_LEFT;
			default:
				return KEY_ESC;
			}
		}
		return KEY_ESC;
	}
	if (c == 0x08)
		return KEY_BACKSPACE;
	if (c == KEY_RETURN)
		return KEY_RETURN;
	return c;
#endif
}

} // namespace tc

#endif /* TCTL_H */
