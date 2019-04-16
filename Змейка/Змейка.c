#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h> 
#include <conio.h>
#include <dos.h>

#define WIDTH 30	//Ширина игрового поля
#define HEIGHT 20	//Длина игрового поля
#define TIME 250		//Время в мс между задержкой кадров (скорость змейки)
#define PERSENTAGE_OF_WALLS 10	//Процент кол-ва стен от общего кол-ва клеток поля
#define PERSENTAGE_OF_APPLES 1	//Процент кол-ва яблок от кол-ва оставшихся после заполнения стенами свободных клеток поля

int width = WIDTH + 2;
int height = HEIGHT + 2;
char arr[HEIGHT + 2][WIDTH + 2];	//Глобальный массив, в котором хранится вся информация о поле
int game_over, move_x, move_y, apple_x, apple_y, wall_x, wall_y, maximum, count;
char direct = 'v';	//Значок для головы змеи
//Структура с координатами части тела змейки(row - номер строки, col - номер столбца)
typedef struct place {
	int row, col;
} place;
//Структура с информацией о змейке
typedef struct Snake {
	place head;	//Координаты головы
	place body[WIDTH*HEIGHT];	//Координаты всех частей тела змеи(0-й элемент в массиве - голова)
	int length;	//Длина змеи
	int score;	//Кол-во очков в игре (число съеденных яблок)
} Snake;
//Функция для помещения курсора в точку (x, y) (взял с форума)
void setcur(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

void clean_string() {
	setcur(0, height + 1);
	printf("                                                                          ");
	setcur(0, height + 1);
}
//Просто функция с инициализацией переменных
void start() {
	game_over = 0;
	count = 0;
}
//Функция инициализации змеи
void init_snake(Snake* snake) {
	//Координаты головы изначально - (0, 1)
	snake->head.col = 1;
	snake->head.row = 0;
	snake->body[0].col = snake->head.col;
	snake->body[0].row = snake->head.row;
	snake->length = 1;
	snake->score = 0;
	arr[snake->head.row][snake->head.col] = direct;
	

	move_x = 0;
	move_y = 0;
}
//Функция рандомного заполнения стенами
void put_wall() {
	for (int i = 0; i < PERSENTAGE_OF_WALLS * WIDTH * HEIGHT / 100; i++) {
		do {
			wall_x = rand() % WIDTH + 1;
			wall_y = rand() % HEIGHT + 1;
		} while (arr[wall_y][wall_x] != ' ' && arr[wall_y][wall_x] != '\0');
		arr[wall_y][wall_x] = '#';
	}
}
//Функция рандомного заполнения яблоками
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
//Функция инициализации поля (постройка граничных стен, постройка стен внутри, заполнение яблоками)
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
//Функция стартует с точки (1, 1) и, двигаясь в порядке "вверх, вправо, вниз, влево", заполняет пустые клетки символом "*" (вместо яблок ставится символ "%" исключительно для удобства)
void check_cell(int y, int x) {
	if (arr[y][x] == ' ' || arr[y][x] == '\0') {
		arr[y][x] = '*';
		check_cell(y - 1, x);
		check_cell(y, x + 1);
		check_cell(y + 1, x);
		check_cell(y, x - 1);
	}
	else if (arr[y][x] == '@') {
		count++;	//Счётчик доступных яблок (т.е. тех, до которых змейка может добраться)
		arr[y][x] = '%';
		check_cell(y - 1, x);
		check_cell(y, x + 1);
		check_cell(y + 1, x);
		check_cell(y, x - 1);
	}
}
//Функция проверки поля на то, что до всех яблок змейка добраться и что змейка может из точки входа в лабиринт прийти в точку выхода
void check_access() {
	check_cell(1, 1);
	if ((count == maximum) && ((arr[1][1] == '*') || (arr[1][1] == '%')) && ((arr[HEIGHT][WIDTH] == '*') || (arr[HEIGHT][WIDTH] == '%'))) {
		game_over = 0;
	}
	else {
		game_over = 2;
	}

}
//Функция очистки массива от символов "*" и "%", оставшизся после функции check_cell и замены на изначальные символы " " и "@"
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

void save(Snake* snake) {
	FILE* fp = fopen("Progress.txt", "w");
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fprintf(fp, "%c", arr[i][j]);
		}
		fprintf(fp, "%c", '\n');
	}
	fprintf(fp, "%d\n", snake->length);
	for (int i = 0; i < snake->length; i++) {
		fprintf(fp, "%d %d\n", snake->body[i].row, snake->body[i].col);
	}
	fprintf(fp, "%c\n%d\n%d", direct, snake->score, maximum);

	fclose(fp);
}
//Функция чтения направления движения с клавиатуры
void move(Snake* snake) {
	int flag = 0;
	if (_kbhit()) {	//функция работает только при вводе символа
		switch (_getch()) {
		case 'w':
			if ((direct != 'v') && (snake->head.col * snake->head.row != 0)) {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
				move_x = 0;
				move_y = -1;
				direct = '^';	//Изменение символа головы
			}
			break;
		case 's':
			if (direct != '^') {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
				move_x = 0;
				move_y = 1;
				direct = 'v';	//Изменение символа головы
			}
			break;
		case 'a':
			if ((direct != '>') && (snake->head.col * snake->head.row != 0)) {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
				move_x = -1;
				move_y = 0;
				direct = '<';	//Изменение символа головы
			}
			break;
		case 'd':
			if ((direct != '<') && (snake->head.col * snake->head.row != 0)) {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
				move_x = 1;
				move_y = 0;
				direct = '>';	//Изменение символа головы
			}
			break;
		case 'p':	//Условие на паузу
			printf("Press 'C' to continue or 'F' to save your progress and finish the game: ");
			while (!flag) {
				if (_kbhit) {
					switch (_getch()) {
					case 'c':	//Условие на продолжение игры
						flag = 1;
						clean_string();
						printf("w,a,s,d - snake control, p - pause.");
						break;
					case 'f':	//Условие на запись в файл и выход
						save(snake);
						clean_string();
						printf("Your progress is saved. See you again! :)\n");
						exit(1);
					}
				}
			}
		default:
			move_x = move_x;
			move_y = move_y;
		}
	}
}
//Функция рисования символа symb в точке (x, y)
void draw_symbol(int y, int x, char symb) {
	setcur(x, y);
	putchar(symb);
}
//Главная функция проверки на столкновение или поедание яблока
void check(Snake* snake) {
	//Столкновение
	if (arr[snake->head.row + move_y][snake->head.col + move_x] == '#' || arr[snake->head.row + move_y][snake->head.col + move_x] == 'o') {
		game_over = -1;
	}
	else if (arr[snake->head.row + move_y][snake->head.col + move_x] == '@') {	//Поедание яблока
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

		snake->length++;
		snake->score++;
	}
	else {	//Движение на следующую пустую клетку без яблок
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
	//Оператор убирает символ " " в точке старта после начала игры
	if (arr[0][1] == ' ') {
		arr[0][1] = '#';
		draw_symbol(0, 1, '#');
	}
	//Оператор стирает надпись про старт и "открывает выход из лабиринта", т.е. помещает в точку выхода символ " "
	if (snake->head.col == 1 && snake->head.row == 1) {
		clean_string();
		printf("w,a,s,d - snake control, p - pause.");
		arr[height - 1][width - 2] = ' ';
		draw_symbol(height - 1, width - 2, ' ');
	}
}
//Функция проверки на попадание в выход из лабиринта
void check_victory(Snake* snake) {
	if ((snake->head.col == (width - 2)) && (snake->head.row == (height - 1))) {
		game_over = 1;
	}
}
//Функция рисования всего поля
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
	Snake* snake = (Snake*)malloc(sizeof(Snake));	//Выделение памяти под "змею"
	start();
	FILE* fp;
	int flag = 0;
	printf("Press 'y', if you want to continue your last game. Press 'n', if you dont want.");
	switch (_getch()) {
	case 'y':
		
		system("cls");
		fp = fopen("Progress.txt", "r");
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				fscanf(fp, "%c", &arr[i][j]);
			}
			fgetc(fp);
		}
		fscanf(fp, "%d", &snake->length);
		fgetc(fp);
		for (int i = 0; i < snake->length; i++) {
			fscanf(fp, "%d", &snake->body[i].row);
			fgetc(fp);
			fscanf(fp, "%d", &snake->body[i].col);
			fgetc(fp);
		}
		snake->head.col = snake->body[0].col;
		snake->head.row = snake->body[0].row;
		fscanf(fp, "%c", &direct);
		fgetc(fp);
		fscanf(fp, "%d", &snake->score);
		fgetc(fp);
		fscanf(fp, "%d", &maximum);
		fgetc(fp);
		move_x = 0;
		move_y = 0;
		fclose(fp);

		draw();
		setcur(0, height + 1);
		printf("Press w,a,s,d to continue.");
		while (!flag) {
			move(snake);
			if (move_x != 0 || move_y != 0) {
				flag = 1;
				clean_string();
				printf("w,a,s,d - snake control, p - pause.");
			}
		}

		break;
	case 'n':
		system("cls");

		srand(time(NULL));
		init_field();
		init_snake(snake);
		check_access();
		clean();
		draw();

		printf("\nPress 's' to start the game:	");

		break;
	default:
		game_over = -1;
		break;
	}
	
	while (!game_over) {
		move(snake);
		check(snake);
		check_victory(snake);
		setcur(0, height + 1);
		Sleep(TIME);
	}
	
	if (game_over == -1) {
		clean_string();
		printf("Game Over :(\n");
	} else if (game_over == 2) {
		clean_string();
		printf("This map is incorrect. Try again.\n");
	}
	else {
		
		clean_string();
		printf("You have escaped! :)");
		if (snake->score < maximum) {
			printf("\nYour score: %d of %d. There are still fresh apples inside. Try again! ;)\n", snake->score, maximum);
		}
		else {
			printf("\nYour score: %d. You've eaten all the apples. My congratulations! And try again! :)\n", maximum);
		}
	}
	free(snake);
	return 0;
}