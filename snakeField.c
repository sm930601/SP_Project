#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "snakeBody.c"
#include "rank.c"

typedef struct {
	int x;
	int y;
} Item;
typedef struct {
	int x;
	int y;
	int width;
	int height;
	SnakeBody* snake;
	Item* item;
	int gameover;	//0 is playing, 1 is game over
	Rank* rank;
	int redraw; //1 need redraw, 0 ignore
} SnakeField;

SnakeField* newSnakeField(int x, int y, int width, int height) {
	SnakeField* field = (SnakeField*)malloc(sizeof(SnakeField));
	field->x = x;
	field->y = y;
	field->width = width;
	field->height = height;
	field->gameover = 0;
	field->item = NULL;
	field->redraw = 1;
	field->snake = newSnake(width / 2, height / 2);
	field->snake->windowX = x;
	field->snake->windowY = y;
	field->rank = NULL;

	makeItemRandomly(field);

	srand((int)time(NULL));
	return field;
}
void makeItemRandomly(SnakeField* field) {
	int x = rand() % (field->width - 1) + 1;
	int y = rand() % (field->height - 1) + 1;
	int flag = 0;	//if item overlap snake, 1
	do {
		x = rand() % (field->width - 1) + 1;
		y = rand() % (field->height - 1) + 1;
		SnakeNode* node = field->snake->head;
		while (node) {
			if (node->x == x && node->y == y)
				flag = 1;
			node = node->link;
		}
	} while (flag == 1);

	Item* item = (Item*)malloc(sizeof(Item));
	item->x = x;
	item->y = y;
	field->item = item;
}
int snakeFieldKeyInput(SnakeField* field, char key) {
	if (field->rank != NULL)
		return rankKeyInput(field->rank, key);

	SnakeNode* head = field->snake->head;
  switch(key) {
		case 'q':
			return -1;
		case '\n':
			if (field->gameover == 1)
				field->rank = createRank(field->snake->length);
			break;
    case 65:
    case 'w':
			if (!head->link || head->y == head->link->y)
      	field->snake->direction = 0;
      break;
    case 66:
    case 's':
			if (!head->link || head->y == head->link->y)
        field->snake->direction = 2;
      break;
    case 67:
    case 'd':
			if (!head->link || head->x == head->link->x)
        field->snake->direction = 1;
      break;
    case 68:
    case 'a':
			if (!head->link || head->x == head->link->x)
      	field->snake->direction = 3;
      break;
  }
	return 0;
}
void snakeFieldRender(SnakeField* field, int milli) {
	if (field->rank != NULL)
	{
		rankRender(field->rank, milli);
		return;
	}

	if (field->redraw == 1)
	{
		for(int i=0;i<field->width+2;i++) {
			move(field->y, field->x+i);
			addch('X');
			move(field->y+field->height+1, field->x+i);
			addch('X');
		}
		for(int j=0;j<field->height+2;j++) {
			move(field->y+j, field->x);
			addch('X');
			move(field->y+j, field->x+field->width+1);
			addch('X');
		}
		field->redraw = 0;
	}
	move(field->y, field->x+field->width+5);
	addstr("                             ");
	move(field->y, field->x+field->width+5);
	addstr("length: ");
	static char textLength[3];
	sprintf(&textLength, "%d", field->snake->length);
	addstr(textLength);

	if (field->gameover != 1) {
		snakeRender(field->snake, milli);
		SnakeNode* head = field->snake->head;

		if (head->x <= 0 || head->x > field->width)
			field->snake->dead = 1;
		if (head->y <= 0 || head->y > field->height)
			field->snake->dead = 1;
		if (field->snake->dead == 1)
			field->gameover = 1;

		if (field->item) {
			Item* item = field->item;
			if (item->x == head->x && item->y == head->y) {
				addSnakeNode(field->snake);
				field->snake->mdelay *= 0.8;
				free(item);
				field->item = NULL;

				snakeRender(field->snake, 0);
				makeItemRandomly(field);
			}
		}

		if (field->item) {
			move(field->y+field->item->y, field->x+field->item->x);
			addch('I');
		}
	}
	else {
		move(field->y+field->height/2,field->x+field->width/2);
		addstr("game over press enter to view rank");
	}
	move(0,0);
}
