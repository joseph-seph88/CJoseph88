#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define WIDTH 80
#define HEIGHT 30
#define INITIAL_SNAKE_LENGTH 1

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position* body;
    int length;
    int direction;
} Snake;

typedef struct {
    Position position;
} Food;

Snake snake;
Food food;

pthread_mutex_t mutex_snake;
pthread_cond_t cond_snake;

bool game_over = false;

void initSnake() {
    snake.length = INITIAL_SNAKE_LENGTH;
    snake.body = malloc(snake.length * sizeof(Position));
    snake.direction = KEY_RIGHT;
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = HEIGHT / 2;
        snake.body[i].y = (WIDTH / 2) - i;
    }
}

void initFood() {
    food.position.x = rand() % (HEIGHT - 2) + 1;
    food.position.y = rand() % (WIDTH - 2) + 1;
}

void drawSnake() {
    attron(COLOR_PAIR(1));
    for (int i = 0; i < snake.length; i++) {
        mvaddch(snake.body[i].x, snake.body[i].y, 'O');
    }
    attroff(COLOR_PAIR(1));
}

void drawFood() {
    attron(COLOR_PAIR(2));
    mvaddch(food.position.x, food.position.y, '*');
    attroff(COLOR_PAIR(2));
}

void drawMap() {
    attron(COLOR_PAIR(3));
    for (int i = 0; i < WIDTH; i++) {
        mvaddch(0, i, '#');
        mvaddch(HEIGHT - 1, i, '#');
    }
    for (int i = 0; i < HEIGHT; i++) {
        mvaddch(i, 0, '#');
        mvaddch(i, WIDTH - 1, '#');
    }
    attroff(COLOR_PAIR(3));
}

void* snakeMoveThread(void* arg) {
    while (!game_over) {
        usleep(200000); 
        pthread_mutex_lock(&mutex_snake);
        int ch = snake.direction;
        pthread_mutex_unlock(&mutex_snake);

        Position next_position = snake.body[0];
        switch (ch) {
            case KEY_UP:
                next_position.x--;
                break;
            case KEY_DOWN:
                next_position.x++;
                break;
            case KEY_LEFT:
                next_position.y--;
                break;
            case KEY_RIGHT:
                next_position.y++;
                break;
        }

        if (next_position.x <= 0 || next_position.x >= HEIGHT - 1 ||
            next_position.y <= 0 || next_position.y >= WIDTH - 1) {
            game_over = true;
            pthread_cond_signal(&cond_snake);
            break; 
        }

        for (int i = 1; i < snake.length; i++) {
            if (next_position.x == snake.body[i].x && next_position.y == snake.body[i].y) {
                game_over = true;
                pthread_cond_signal(&cond_snake);
                break; 
            }
        }

        if (next_position.x == food.position.x && next_position.y == food.position.y) {
            snake.length++;
            snake.body = realloc(snake.body, snake.length * sizeof(Position));
            snake.body[snake.length - 1] = snake.body[snake.length - 2]; 
            initFood();
        }

        for (int i = snake.length - 1; i > 0; i--) {
            snake.body[i] = snake.body[i - 1];
        }
        snake.body[0] = next_position;

        pthread_mutex_lock(&mutex_snake);
        pthread_cond_signal(&cond_snake);
        pthread_mutex_unlock(&mutex_snake);
    }
    return NULL;
}

int main() {
    initscr();
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);
    srand(time(NULL));
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(3, COLOR_CYAN, COLOR_BLACK); 
    initSnake();
    initFood();

    pthread_t tid;
    pthread_create(&tid, NULL, snakeMoveThread, NULL);

    pthread_mutex_init(&mutex_snake, NULL);
    pthread_cond_init(&cond_snake, NULL);

    nodelay(stdscr, TRUE); 
    
    while (!game_over) {
        int ch = getch();
        pthread_mutex_lock(&mutex_snake);
        switch (ch) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                snake.direction = ch;
                break;
            case 'q':
                game_over = true;
                break;
        }
        pthread_mutex_unlock(&mutex_snake);

        clear();
        drawMap();
        drawSnake();
        drawFood();
        refresh();
        usleep(100000);
    }

    pthread_join(tid, NULL);

    endwin();
    return 0;
}
