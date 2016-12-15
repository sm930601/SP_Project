#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>

#include "snakeField.c"

void tty_mode(int);
void set_noecho_mode();
void set_nodelay_mode();
void render(int);	//milliseconds
SnakeField* field;
main() {
	initscr();

	tty_mode(0);		//상태 저장
	//set_noecho_mode();
	noecho();
	set_nodelay_mode();	//입력을 받기위해서 입력 딜레이를 없앤다.

	field = newSnakeField(5,1,40,20);	//실제 게임이 돌아가는 필드

	while(true) {
		//clear();
		render(1000/30);	//30fps
		refresh();
		usleep(1000000/30);
		char c = getch();
		if (snakeFieldKeyInput(field, c) < 0)
      break;
	}

	tty_mode(1);		//상태 복원
	endwin();
}
void render(int milli) {
	snakeFieldRender(field, milli);
}
void tty_mode(int how) {
	static struct termios original_mode;
	static int original_flags;
	if (how == 0) {
		tcgetattr(0, &original_mode);
		original_flags = fcntl(0, F_GETFL);
	} else {
		tcsetattr(0, TCSANOW, &original_mode);
		fcntl(0, F_SETFL, original_flags);
	}
}
void set_noecho_mode() {
	struct termios ttystate;
	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~(ICANON | ECHO);
	ttystate.c_cc[VMIN] = 1;
	ttystate.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &ttystate);
}
void set_nodelay_mode() {
	int termflags;
	termflags = fcntl(0, F_GETFL);
	termflags |= O_NDELAY;
	fcntl(0, F_SETFL, termflags);
}
