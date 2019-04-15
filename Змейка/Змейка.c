#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h> 
#include <conio.h>
#include <dos.h>

#define WIDTH 60
#define HEIGHT 20
#define TIME 70
#define PERSENTAGE_OF_WALLS 0
#define PERSENTAGE_OF_APPLES 1

int width = WIDTH + 2;
int height = HEIGHT + 2;
char arr[HEIGHT + 2][WIDTH + 2];
int game_over, move_x, move_y, apple_x, apple_y, wall_x, wall_y, maximum, count;
char direct = 'v';

typedef struct place {
	int row, col;
} place;

typedef struct Snake {
	place head;
	place body[WIDTH*HEIGHT];
	int length;
	int score;
} Snake;

void setcur(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

void start() {
	game_over = 0;
	count = 0;
}

Snake* init_snake() {
	Snake* snake = (Snake*)malloc(sizeof(Snake));
	snake->head.col = 1;
	snake->head.row = 0;
	snake->body[0].col = snake->head.col;
	snake->body[0].row = snake->head.row;
	snake->length = 1;
	snake->score = 0;
	arr[snake->head.row][snake->head.col] = direct;
	

	move_x = 0;
	move_y = 0;

	return snake;
}

void put_wall() {
	for (int i = 0; i < PERSENTAGE_OF_WALLS * WIDTH * HEIGHT / 100; i++) {
		do {
			wall_x = rand() % WIDTH + 1;
			wall_y = rand() % HEIGHT + 1;
		} while (arr[wall_y][wall_x] != ' ' && arr[wall_y][wall_x] != '\0');
		arr[wall_y][wall_x] = '#';
	}
}

void put_apple() {
	maximum = 0;
	for (int i = 0; i < PERSENTAGE_OF_APPLES * WIDTH * HEIGHT * (100 - PERSENTAGE_OF_WALLS) / 100 / 100; i++) {
		do {
			apple_x = rand() % WIDTH + 1;
			apple_y = rand() % HEIGHT + 1;
		} while (arr[apple_y][apple_x] != ' ' && arr[apple_y][apple_x] != '\0');
		arr[apple_y][apple_x] = '@';
		maximum++;
	}
}

void init_field() {
	for (int i = 0; i < width; i++) {
		arr[0][i] = '#';
		arr[height - 1][i] = '#';
	}
	for (int i = 1; i < height - 1; i++) {
		arr[i][0] = '#';
		arr[i][width - 1] = '#';
	}

	put_wall();
	put_apple();
}

void check_cell(int y, int x) {
	if (arr[y][x] == ' ' || arr[y][x] == '\0') {
		arr[y][x] = '*';
		check_cell(y - 1, x);
		check_cell(y, x + 1);
		check_cell(y + 1, x);
		check_cell(y, x - 1);
	}
	else if (arr[y][x] == '@') {
		count++;
		arr[y][x] = '%';
		check_cell(y - 1, x);
		check_cell(y, x + 1);
		check_cell(y + 1, x);
		check_cell(y, x - 1);
	}
}

void clean() {
	setcur(0, 0);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (arr[i][j] == '*') {
				arr[i][j] = ' ';
			}
			else if (arr[i][j] == '%') {
				arr[i][j] = '@';
			}
		}
	}
}

void check_access() {
	check_cell(1, 1);
	if ((count == maximum) && (arr[1][1] == '*') && (arr[HEIGHT][WIDTH] == '*')) {
		game_over = 0;
	}
	else {
		game_over = 2;
	}

}

void move(Snake* snake) {
	if (_kbhit()) {
		switch (_getch()) {
		case 'w':
			if (direct != 'v') {
				move_x = 0;
				move_y = -1;
				direct = '^';
			}
			break;
		case 's':
			if (direct != '^') {
				move_x = 0;
				move_y = 1;
				direct = 'v';
			}
			break;
		case 'a':
			if (direct != '>') {
				move_x = -1;
				move_y = 0;
				direct = '<';
			}
			break;
		case 'd':
			if (direct != '<') {
				move_x = 1;
				move_y = 0;
				direct = '>';
			}
			break;
		default:
			move_x = move_x;
			move_y = move_y;
		}
	}
}

void draw_symbol(int y, int x, char symb) {
	setcur(x, y);
	putchar(symb);
}

void check(Snake* snake) {
	if (arr[snake->head.row + move_y][snake->head.col + move_x] == '#' || arr[snake->head.row + move_y][snake->head.col + move_x] == 'o') {
		game_over = -1;
	}
	else if (arr[snake->head.row + move_y][snake->head.col + move_x] == '@') {
		arr[snake->head.row][snake->head.col] = 'o';
		draw_symbol(snake->head.row, snake->head.col, 'o');
		snake->head.col += move_x;
		snake->head.row += move_y;
		for (int i = snake->length; i > 0; i--) {
			snake->body[i].col = snake->body[i - 1].col;
			snake->body[i].row = snake->body[i - 1].row;
		}
		snake->body[0].col = snake->head.col;
		snake->body[0].row = snake->head.row;

		arr[snake->head.row][snake->head.col] = direct;
		draw_symbol(snake->head.row, snake->head.col, direct);
		//put_apple();
		snake->length++;
		snake->score++;
	}
	else {
		arr[snake->head.row][snake->head.col] = 'o';
		draw_symbol(snake->head.row, snake->head.col, 'o');

		arr[snake->body[snake->length - 1].row][snake->body[snake->length - 1].col] = ' ';
		draw_symbol(snake->body[snake->length - 1].row, snake->body[snake->length - 1].col, ' ');

		snake->head.col += move_x;
		snake->head.row += move_y;
		for (int i = snake->length; i > 0; i--) {
			snake->body[i].col = snake->body[i - 1].col;
			snake->body[i].row = snake->body[i - 1].row;
		}
		snake->body[0].col = snake->head.col;
		snake->body[0].row = snake->head.row;
		
		arr[snake->head.row][snake->head.col] = direct;
		draw_symbol(snake->head.row, snake->head.col, direct);
	}
	if (arr[0][1] == ' ') {
		arr[0][1] = '#';
		draw_symbol(0, 1, '#');
	}
	if (snake->head.col == 1 && snake->head.row == 1) {
		setcur(0, height + 1);
		printf("                                  ");
		arr[height - 1][width - 2] = ' ';
		draw_symbol(height - 1, width - 2, ' ');
	}
}

void check_victory(Snake* snake) {
	if ((snake->head.col == (width - 2)) && (snake->head.row == (height - 1))) {
		game_over = 1;
	}
}

void draw() {
	setcur(0, 0);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			putchar(arr[i][j]);
		}
		putchar('\n');
	}
}

int main() {
	start();
	srand(time(NULL));
	init_field();
	Snake* snake = init_snake();
	check_access();
	clean();
	draw();
	printf("\nPress 'S' to start the game:	");
	
	while (!game_over) {
		move(snake);
		check(snake);
		check_victory(snake);
		setcur(0, height + 1);
		Sleep(TIME);
	}
	
	if (game_over == -1) {
		printf("Game Over :(\n");
	} else if (game_over == 2) {
		setcur(0, height + 1);
		printf("This map is incorrect. Try again.\n");
	}
	else {
		printf("You have escaped! :)");
		if (snake->score < maximum) {
			printf("\nYour score: %d. There are still fresh apples inside. Try again! ;)\n", snake->score);
		}
		else {
			printf("\nYour score: %d. You've eaten all the apples. My congratulations! And try again! :)\n", maximum);
		}
	}
	free(snake);
	return 0;
}