#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>

#include "snakeField.c"

void tty_mode(int);
void set_nodelay_mode();
void set_noecho_mode();
void render(int);
SnakeField* field;

main()
{
	initscr();

	tty_mode(0);
	noecho();
	set_nodelay_mode();

	field = newSnakeField(5, 1, 40, 20);

	while(true) {
		//clear();
		render(1000/60);
		refresh();
		usleep(1000000/60);
		char c = getch();
		if(c == 'q') {
			break;
		}
		snakeFieldKeyInput(field, c);
	}

	tty_mode(1);
	endwin();
}

void render(int milli) 
{
	snakeFieldRender(field, milli);
}

void tty_mode(int how)
{
	static struct termios original_mode;
	static int original_flags;

	if(how == 0) {
		tcgetattr(0, &original_mode);
		original_flags = fcntl(0, F_GETFL);
	}
	else
	{
		tcsetattr(0, TCSANOW, &original_mode);
		fcntl(0, F_SETFL, original_flags);
	}
}

void set_nodelay_mode()
{
	int termflags;
	termflags = fcntl(0, F_GETFL);
	termflags |= O_NDELAY;
	fcntl(0, F_SETFL, termflags);
}


void set_noecho_mode() {
	struct termios ttystate;
	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~(ICANON | ECHO);
	ttystate.c_cc[VMIN] = 1;
	ttystate.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &ttystate);
}

