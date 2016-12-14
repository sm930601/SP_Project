#include <stdlib.h>
#include <curses.h>

struct SnakeNode {
	int x;
	int y;
	int lastX;
	int lastY;
	struct SnakeNode* link;
};
typedef struct SnakeNode SnakeNode;
typedef struct SnakeBody {
	SnakeNode* head;
	SnakeNode* tail;
	int length;
	int direction;	//-1 stop, 0 up, 1 right, 2 down, 3 left
	int mdelay;	//ndelay 마다 direction으로 이동한다.
	int _mdelay;
	char ch;
	int windowX;
	int windowY;
	int dead;	//1 dead, 0 live
	int redraw; //1 need, 0 ignore
} SnakeBody;
SnakeNode* newSnakeNode(int, int);
SnakeBody* newSnake(int, int);
void addSnakeNode(SnakeBody*);
void stepSnake(SnakeBody*);
SnakeNode* newSnakeNode(int x, int y) {
	SnakeNode* node = (SnakeNode*)malloc(sizeof(SnakeNode));
	node->x = x;
	node->y = y;
	node->lastX = x;
	node->lastY = y;
	node->link = NULL;
	return node;
}
SnakeBody* newSnake(int x, int y) {
	SnakeBody* body = (SnakeBody*)malloc(sizeof(SnakeBody));
	body->ch = 'O';
	body->head = newSnakeNode(x, y);
	body->tail = body->head;
	body->length = 1;
	body->windowX = 0;
	body->windowY = 0;
	body->dead = 0;
	body->direction = -1;
	body->mdelay = 700;
	body->_mdelay = 0;
	body->redraw = 1;

	return body;
}
void addSnakeNode(SnakeBody* body) {	//끝에 노드를 더한다.
	SnakeNode* tail = body->tail;
	SnakeNode* add_node = newSnakeNode(tail->lastX, tail->lastY);
	tail->link = add_node;
	body->tail = add_node;
	body->length++;

	body->redraw = 1;
}
void stepSnake(SnakeBody* body) {
	if (body->direction == -1)
		return;

	body->redraw = 1;

	SnakeNode* head = body->head;
	SnakeNode* temp = head;
	head->lastX = head->x;
  head->lastY = head->y;
	while (temp->link) {
		temp->link->lastX = temp->link->x;
		temp->link->lastY = temp->link->y;
		temp->link->x = temp->lastX;
		temp->link->y = temp->lastY;
		temp = temp->link;
	}

	if (body->direction == 0)
		head->y--;
	else if (body->direction == 1)
		head->x++;
	else if (body->direction == 2)
		head->y++;
	else if (body->direction == 3)
		head->x--;

	temp = head->link;
	while (temp) {
		if (temp->x == head->x && temp->y == head->y) {
			body->dead = 1;
			break;
		}
		temp = temp->link;
	}
}
void snakeRender(SnakeBody* body, int milli) {
	int* _mdelay = &(body->_mdelay);
	*_mdelay += milli;
	if (*_mdelay > body->mdelay) {
		*_mdelay = 0;
		stepSnake(body);
	}

	if (body->redraw == 1) {
		SnakeNode* node = body->head;
		while (node) {
      move(body->windowY + node->lastY, body->windowX + node->lastX);
      addch(' ');
      node = node->link;
    }
		node = body->head;
		while (node) {
			move(body->windowY + node->y, body->windowX + node->x);
			addch(body->ch);
			node = node->link;
		}
		body->redraw = 0;
	}
	move(1,1);
}
