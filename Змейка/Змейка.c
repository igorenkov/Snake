#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h> 
#include <conio.h>
#include <dos.h>

#define UP 72
#define DOWN 80
#define RIGHT 77
#define LEFT 75
#define WIDTH 150
#define HEIGHT 40
#define TIME 150 
#define PERSENTAGE_OF_WALLS 3
#define PERSENTAGE_OF_APPLES 0.5

int game_over, move_x, move_y;
//int count;

//Структура с координатами части тела змейки(row - номер строки, col - номер столбца)
typedef struct place {
	int row, col;
} place;
//Структура с информацией о змейке
typedef struct Snake {
	place head;	//Координаты головы
	place* body;	//Указатель на массив с координатами всех частей тела змеи(0-й элемент в массиве - голова)
	int length;	//Длина змеи
	int score;	//Кол-во очков в игре (число съеденных яблок)
	char direct;
} Snake;

typedef struct Field {
	int width;	//Ширина поля
	int height;	//Высота поля
	int time;	//Время обновления кадров
	float persentage_of_walls;	//Процент заполнения поля стенами
	float persentage_of_apples;		//Процент заполнения свободного поля яблоками
	int maximum;	//Количество яблок на поле
	char** arr;	//Указатель на поле (двумерный массив из символов)
} Field;
//Функция для помещения курсора в точку (x, y) (взял с форума)
void setcur(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};
//Функция очистки строки в n-й строчке под полем (так проще вводить текст)
void clean_string(Field* field, int n) {
	setcur(0, field->height + 2 + n);
	printf("                                                                            ");
	setcur(0, field->height + 2 + n);
}
//Просто функция с инициализацией переменных
void start() {
	game_over = 0;
//	count = 0;
}
//Функция инициализации змеи
void init_snake(Snake* snake, Field* field) {
	//Координаты головы изначально - (0, 1)
	snake->body = (place*)malloc(sizeof(place) * field->height * field->width);
	snake->head.col = 1;
	snake->head.row = 0;
	snake->body[0].col = snake->head.col;
	snake->body[0].row = snake->head.row;
	snake->length = 1;
	snake->score = 0;
	snake->direct = 'v';
}
//Функция рандомного заполнения стенами
void put_wall(Field* field) {
	int wall_x, wall_y;
	for (int i = 0; i < (float)field->persentage_of_walls * field->width * field->height / 100; i++) {
		do {
			wall_x = rand() % field->width + 1;
			wall_y = rand() % field->height + 1;
		} while (field->arr[wall_y][wall_x] != ' ');
		field->arr[wall_y][wall_x] = '#';
	}
}
//Функция рандомного заполнения яблоками
void put_apple(Field* field) {
	int apple_x, apple_y;
	int maximum = 0;
	for (int i = 0; i < (float)field->persentage_of_apples * field->width * field->height * (100 - field->persentage_of_walls) / 100 / 100; i++) {
		do {
			apple_x = rand() % field->width + 1;
			apple_y = rand() % field->height + 1;
		} while (field->arr[apple_y][apple_x] != ' ');
		field->arr[apple_y][apple_x] = '@';
		maximum++;
	}
	field->maximum = maximum;
}
//Функция инициализации поля (считывание размеров и характеристик, постройка граничных стен, постройка стен внутри, заполнение яблоками)
void init_field(Field* field) {
	int speed;
	int flag = 0;
	printf("Press 'D' to use default settings, 'S' to set personal arguments.\n");
	while (!flag) {
		switch (_getch()) {
		case 's':
			printf("Enter width of the field: ");
			scanf("%d", &field->width);
			printf("Enter height of the field: ");
			scanf("%d", &field->height);
			printf("Enter speed of the snake(1-10): ");
			scanf("%d", &speed);
			field->time = 750 / speed;
			printf("Enter percentage of walls: ");
			scanf("%f", &field->persentage_of_walls);
			printf("Enter percentage of apples: ");
			scanf("%f", &field->persentage_of_apples);
			flag = 1;
		case 'd':
			field->width = WIDTH;
			field->height = HEIGHT;
			field->time = TIME;
			field->persentage_of_walls = PERSENTAGE_OF_WALLS;
			field->persentage_of_apples = PERSENTAGE_OF_APPLES;
			flag = 1;
		}
	}
	system("cls");
	//Выделение памяти под массив с симолами поля
	field->arr = (char**)malloc(sizeof(char*) * (field->height + 2));
	for (int i = 0; i < field->height + 2; i++) {
		field->arr[i] = (char*)malloc(sizeof(char) * (field->width + 2));
	}
	//Постройка границ
	for (int i = 0; i < field->width + 2; i++) {
		field->arr[0][i] = '#';
		field->arr[field->height + 1][i] = '#';
	}
	for (int i = 1; i < field->height + 1; i++) {
		field->arr[i][0] = '#';
		field->arr[i][field->width + 1] = '#';
	}
	for (int i = 1; i < field->height + 1; i++) {
		for (int j = 1; j < field->width + 1; j++) {
			field->arr[i][j] = ' ';
		}
	}
	field->arr[0][1] = 'v';
	//Постройка стен внутри, заполнение яблоками
	put_wall(field);
	put_apple(field);
}
//Функция стартует с точки (1, 1) и, двигаясь в порядке "вверх, вправо, вниз, влево", заполняет пустые клетки символом "*" (вместо яблок ставится символ "%" исключительно для удобства)
void check_cell(int y, int x, Field* field, int recurs) {
	if (field->arr[y][x] == ' ' && recurs < 4500) {
		recurs++;	//Счётчик для контроля переполнения стека
		field->arr[y][x] = '*';
		check_cell(y - 1, x, field, recurs);
		check_cell(y, x + 1, field, recurs);
		check_cell(y + 1, x, field, recurs);
		check_cell(y, x - 1, field, recurs);
	}
	else if (field->arr[y][x] == '@' && recurs < 4500) {
		recurs++;	//Счётчик для контроля переполнения стека
	//	count++;	//Счётчик доступных яблок (т.е. тех, до которых змейка может добраться)
		field->arr[y][x] = '%';
		check_cell(y - 1, x, field, recurs);
		check_cell(y, x + 1, field, recurs);
		check_cell(y + 1, x, field, recurs);
		check_cell(y, x - 1, field, recurs);
	}
}
//Функция проверки поля на то, что до всех яблок змейка добраться и что змейка может из точки входа в лабиринт прийти в точку выхода
void check_access(Field* field,int recurs) {
	check_cell(1, 1, field, recurs);
	if (/*(count == field->maximum) &&*/ ((field->arr[1][1] == '*') || (field->arr[1][1] == '%')) && ((field->arr[field->height][field->width] == '*') || (field->arr[field->height][field->width] == '%'))) {
		game_over = 0;
	}
	else {
		game_over = 2;
	}

}
//Функция очистки массива от символов "*" и "%", оставшизся после функции check_cell и замены на изначальные символы " " и "@"
void clean(Field* field) {
	setcur(0, 0);
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			if (field->arr[i][j] == '*') {
				field->arr[i][j] = ' ';
			}
			else if (field->arr[i][j] == '%') {
				field->arr[i][j] = '@';
			}
		}
	}
}
//Функция сохранения змейки и поля в файл "Progress.txt"
void save(Snake* snake, Field* field) {
	FILE* fp = fopen("Progress.txt", "w");
	fprintf(fp, "%d %d\n", field->height, field->width);
	fprintf(fp, "%d\n", field->time);
	fprintf(fp, "%f %f\n", field->persentage_of_walls, field->persentage_of_apples);
	fprintf(fp, "%d\n", field->maximum);
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			fprintf(fp, "%c", field->arr[i][j]);
		}
		fprintf(fp, "%c", '\n');
	}
	fprintf(fp, "%d\n", snake->length);
	for (int i = 0; i < snake->length; i++) {
		fprintf(fp, "%d %d\n", snake->body[i].row, snake->body[i].col);
	}
	fprintf(fp, "%c", snake->direct);
	fclose(fp);
}
//Функция чтения направления движения с клавиатуры
void move(Snake* snake, Field* field) {
	//Условие на первое движение только вниз
	int flag = 0;
	char temp;
	if (snake->head.col == 1 && snake->head.row == 0) {
		while (!flag) {
			if (_kbhit) {
				if (_getch() == DOWN) {
					move_x = 0;
					move_y = 1;
					snake->direct = 'v';
					flag = 1;
					//Если яблок изначально нет, генерируется другая надпись
					if (field->maximum == snake->score) {
						clean_string(field, 2);
						printf("There are no apples here. Find a way out!", snake->score);
					}
					else {
						clean_string(field, 1);
						printf("Up, down, left, right - snake control, 'space' - pause.\nApples number - %d", field->maximum);
					}
				}
			}
		}
	}
	else if (_kbhit()) {	//функция работает только при вводе символа
		temp = _getch();
		if (temp == ' ') {
			clean_string(field, 1);
			printf("Press 'Space' to continue, 'F' to save your progress, 'E' finish the game: ");
			while (!flag) {
				if (_kbhit) {
					switch (_getch()) {
					case ' ':	//Условие на продолжение игры
						flag = 1;
						clean_string(field, 1);
						printf("Up, down, left, right - snake control, 'space' - pause.");
						break;
					case 'f':	//Условие на запись в файл
						save(snake, field);
						clean_string(field, 1);
						printf("Your progress is saved!");
						Sleep(2000);
						clean_string(field, 1);
						printf("Press 'space' to continue, 'F' to save your progress, 'E' to finish the game: ");
						break;
					case 'e':	//Условие на выход из игры
						clean_string(field, 2);
						clean_string(field, 1);
						printf("See you again! :)\n");
						Sleep(1000);
						exit(1);
					}
				}
			}
		}
		else if (temp = -32) {
			switch (_getch()) {
			case UP:
				if (snake->direct != 'v') {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
					move_x = 0;
					move_y = -1;
					snake->direct = '^';	//Изменение символа головы
				}
				break;
			case DOWN:
				if (snake->direct != '^') {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
					move_x = 0;
					move_y = 1;
					snake->direct = 'v';	//Изменение символа головы
				}
				break;
			case LEFT:
				if (snake->direct != '>') {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
					move_x = -1;
					move_y = 0;
					snake->direct = '<';	//Изменение символа головы
				}
				break;
			case RIGHT:
				if (snake->direct != '<') {	//Проверка, что змейка не пойдёт назад (т.е. сама в себя)
					move_x = 1;
					move_y = 0;
					snake->direct = '>';	//Изменение символа головы
				}
				break;
			default:
				move_x = move_x;
				move_y = move_y;
			}
		}
	}
}
//Функция рисования символа symb в точке (x, y)
void draw_symbol(int y, int x, char symb) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	setcur(x, y);

	if (symb == '#') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 6));
	}
	else if (symb == '@') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 12));
	}
	else if (symb == 'o') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 9));
	} else if (symb == '^' || symb == '>' || symb == 'v' || symb == '<') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 11));
	}

	putchar(symb);

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
}
//Главная функция проверки на столкновение или поедание яблока
void check(Snake* snake, Field* field) {
	//Столкновение
	if (field->arr[snake->head.row + move_y][snake->head.col + move_x] == '#' || field->arr[snake->head.row + move_y][snake->head.col + move_x] == 'o') {
		game_over = -1;
	}
	else if (field->arr[snake->head.row + move_y][snake->head.col + move_x] == '@') {	//Поедание яблока
		field->arr[snake->head.row][snake->head.col] = 'o';
		draw_symbol(snake->head.row, snake->head.col, 'o');
		snake->head.col += move_x;
		snake->head.row += move_y;
		for (int i = snake->length; i > 0; i--) {
			snake->body[i].col = snake->body[i - 1].col;
			snake->body[i].row = snake->body[i - 1].row;
		}
		snake->body[0].col = snake->head.col;
		snake->body[0].row = snake->head.row;

		field->arr[snake->head.row][snake->head.col] = snake->direct;
		draw_symbol(snake->head.row, snake->head.col, snake->direct);

		snake->length++;
		snake->score++;
		//Вывод информации об очках
		clean_string(field, 2);
		printf("Your score - %d. Apples left - %d.", snake->score, field->maximum - snake->score);
		if (field->maximum == snake->score) {
			clean_string(field, 2);
			printf("Your score - %d. Find a way out!", snake->score);
		}
	}
	else {	//Движение на следующую пустую клетку без яблок
		field->arr[snake->head.row][snake->head.col] = 'o';
		draw_symbol(snake->head.row, snake->head.col, 'o');

		field->arr[snake->body[snake->length - 1].row][snake->body[snake->length - 1].col] = ' ';
		draw_symbol(snake->body[snake->length - 1].row, snake->body[snake->length - 1].col, ' ');

		snake->head.col += move_x;
		snake->head.row += move_y;
		for (int i = snake->length; i > 0; i--) {
			snake->body[i].col = snake->body[i - 1].col;
			snake->body[i].row = snake->body[i - 1].row;
		}
		snake->body[0].col = snake->head.col;
		snake->body[0].row = snake->head.row;

		field->arr[snake->head.row][snake->head.col] = snake->direct;
		draw_symbol(snake->head.row, snake->head.col, snake->direct);
	}
	//Условие на удаление символа " " в точке старта после начала игры
	if (field->arr[0][1] == ' ') {
		field->arr[0][1] = '#';
		draw_symbol(0, 1, '#');
	}
	//Условие на стирание надписи про старт и "открывает выход из лабиринта", т.е. помещает в точку выхода символ " "
	if (snake->head.col == 1 && snake->head.row == 1) {
		field->arr[field->height + 1][field->width] = ' ';
		draw_symbol(field->height + 1, field->width, ' ');
	}
}
//Функция проверки на попадание в выход из лабиринта
void check_victory(Snake* snake, Field* field) {
	if ((snake->head.col == (field->width)) && (snake->head.row == (field->height + 1))) {
		game_over = 1;
	}
}
//Функция рисования всего поля
void draw(Field* field) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	setcur(0, 0);
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			draw_symbol(i, j, field->arr[i][j]);
		}
		putchar('\n');
	}
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
}
//Функция считывания поля и змейки из файла "Progress.txt"
void read(FILE* fp, Snake* snake, Field* field) {
	//Запись информации о поле
	fscanf(fp, "%d", &field->height);
	fgetc(fp);
	fscanf(fp, "%d", &field->width);
	fgetc(fp);
	fscanf(fp, "%d", &field->time);
	fgetc(fp);
	fscanf(fp, "%f", &field->persentage_of_walls);
	fgetc(fp);
	fscanf(fp, "%f", &field->persentage_of_apples);
	fgetc(fp);
	fscanf(fp, "%d", &field->maximum);
	fgetc(fp);
	//Выделение памяти под поле и запись поля в массив
	field->arr = (char**)malloc(sizeof(char*) * (field->height + 2));
	for (int i = 0; i < field->height + 2; i++) {
		field->arr[i] = (char*)malloc(sizeof(char) * (field->width + 2));
	}
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			fscanf(fp, "%c", &field->arr[i][j]);
		}
		fgetc(fp);
	}
	//Выделение памяти под тело змейки и запись информации о змейке
	snake->body = (place*)malloc(sizeof(place) * field->height * field->width);
	//Запись длины змейки
	fscanf(fp, "%d", &snake->length);
	fgetc(fp);
	//Запись координат сегментов тела змейки в структуру
	for (int i = 0; i < snake->length; i++) {
		fscanf(fp, "%d", &snake->body[i].row);
		fgetc(fp);
		fscanf(fp, "%d", &snake->body[i].col);
		fgetc(fp);
	}
	//Запись координат головы
	snake->head.col = snake->body[0].col;
	snake->head.row = snake->body[0].row;
	fscanf(fp, "%c", &snake->direct);	//Запись значка головы(^,>,v,<)
	fgetc(fp);
	snake->score = snake->length - 1;
}
//Функция удаления массива под поле, массива под тело змейки, структуры поля, структуры змейки 
void free_game(Snake* snake, Field* field) {
	free(snake->body);
	free(snake);
	for (int i = 0; i < field->height + 2; i++) {
		free(field->arr[i]);
	}
	free(field->arr);
	free(field);
}

int main() {

	while (1) {
		Field* field = (Field*)malloc(sizeof(Field));	//Выделение памяти под поле
		Snake* snake = (Snake*)malloc(sizeof(Snake));	//Выделение памяти под змейку
		start();
		FILE* fp;
		int flag = 0;	//Флажок для одной функции ниже
		int extraflag = 0;
		int recurs = 0;
		printf("Press 'C' to continue your last game, 'N' to start a new game.");
		while (!extraflag) {
			switch (_getch()) {
			case 'c':
				//Условие на вывод из файла
				system("cls");
				fp = fopen("Progress.txt", "r");
				read(fp, snake, field);
				draw(field);
				clean_string(field, 1);
				printf("Press w,a,s,d to continue.");
				move_x = 0;
				move_y = 0;
				//Условие на первое движение
				while (!flag) {
					move(snake, field);
					if (move_x != 0 || move_y != 0) {
						flag = 1;
						clean_string(field, 1);
						printf("Up, down, left, right - snake control, 'space' - pause.");
					}
				}
				extraflag = 1;
				break;
			case 'n':
				//Условие на новую игру
				system("cls");
				//Генерация
				srand(time(NULL));
				init_field(field);
				init_snake(snake, field);
				check_access(field, recurs);
				clean(field);
				draw(field);
				clean_string(field, 1);
				printf("Press 'down' to start the game:	");
				extraflag = 1;
				break;
			default:
				break;
			}
		}
		//Непосредственно функция игры
		while (!game_over) {
			move(snake, field);
			check(snake, field);
			check_victory(snake, field);
			setcur(0, field->height + 5);
			Sleep(field->time);
		}
		//Вывод результатов
		if (game_over == -1) {
			clean_string(field, 2);
			clean_string(field, 1);
			printf("Game Over :(");
			free(snake);
			free(field);
		}
		else if (game_over == 2) {
			clean_string(field, 1);
			printf("This map is incorrect. Try again.");
			free_game(snake, field);
		}
		else {
			clean_string(field, 1);
			printf("You have escaped! :)");
			clean_string(field, 2);
			if (snake->score < field->maximum) {

				printf("Your score: %d of %d. There are still fresh apples inside. Try again! ;)", snake->score, field->maximum);
			}
			else {
				printf("Your score: %d. You've eaten all the apples. My congratulations! And try again! :)", field->maximum);
			}
			free_game(snake, field);
		}
		Sleep(4000);
		//Условие на начало новой игры или выход
		system("cls");
		flag = 0;
		printf("Press 'R' to restart the game,'E' to finish the game: ");
		while (!flag) {
			if (_kbhit) {
				switch (_getch()) {
				case 'r':	//Условие на продолжение игры
					flag = 1;
					system("cls");
					break;
				case 'e':	//Условие на выход из игры
					system("cls");
					printf("See you again! :)\n");
					Sleep(1000);
					exit(1);
				}
			}
		}
	}
	return 0;
}