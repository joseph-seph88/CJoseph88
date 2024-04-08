#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAP_SIZE 6
#define SHIP_NUMBER 4

typedef struct
{
    int x, y;
    char symbol;
    int life;
} Ship;

Ship user_ship_number[SHIP_NUMBER];
Ship computer_ship_number[SHIP_NUMBER];

char user_status_board[MAP_SIZE][MAP_SIZE];
char user_attack_board[MAP_SIZE][MAP_SIZE];
char computer_status_board[MAP_SIZE][MAP_SIZE];
char computer_attack_board[MAP_SIZE][MAP_SIZE];

void initBoard(char board[MAP_SIZE][MAP_SIZE])
{
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            board[i][j] = '-';
        }
    }
}

void placeShip(Ship ships[SHIP_NUMBER], char board[MAP_SIZE][MAP_SIZE])
{
    int shipAsciiCode = 65;

    for (int i = 0; i < SHIP_NUMBER; i++)
    {
        ships[i].symbol = shipAsciiCode;
        ships[i].life = SHIP_NUMBER - i;
        ships[i].x = rand() % MAP_SIZE;
        ships[i].y = rand() % MAP_SIZE;

        while (board[ships[i].x][ships[i].y] != '-')
        {
            ships[i].x = rand() % MAP_SIZE;
            ships[i].y = rand() % MAP_SIZE;
        }

        board[ships[i].x][ships[i].y] = ships[i].symbol;
        shipAsciiCode++;
    }
}

void printBoard(char board1[MAP_SIZE][MAP_SIZE], char board2[MAP_SIZE][MAP_SIZE])
{
    printf("      ==보드 상태==\t\t       ==공격 현황==\n");
    printf("   ");
    for (int i = 0; i < MAP_SIZE; i++)
    {
        printf(" %2d ", i);
    }
    printf("\t   ");
    for (int i = 0; i < MAP_SIZE; i++)
    {
        printf(" %2d ", i);
    }
    printf("\n");

    for (int i = 0; i < MAP_SIZE; i++)
    {
        printf("%2d ", i);
        for (int j = 0; j < MAP_SIZE; j++)
        {
            printf("| %c ", board1[i][j]);
        }
        printf("\t");

        printf("%2d ", i);
        for (int j = 0; j < MAP_SIZE; j++)
        {
            printf("| %c ", board2[i][j]);
        }

        printf("\n");
    }
}
void userAttack() {
    int x, y;
    bool validAttack = false;

    while (!validAttack) {
        printf("포격 지점 좌표 입력 (#0-5, 0-5): ");
        scanf("%d %d", &x, &y);
        if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
            printf("좌표 설정 오류, 포격 좌표를 재입력해주세요.\n");
        }
        else {
            char targetSymbol = computer_status_board[x][y]; 

            if (targetSymbol == '!') {
                printf("이미 공격 중인 함선을 재포격합니다.\n");
                continue; 
            }

            if (targetSymbol >= 'A' && targetSymbol <= 'D') {
                printf("적 함선을 공격합니다!\n");
                validAttack = true;

                for (int i = 0; i < SHIP_NUMBER; i++) {
                    if (computer_ship_number[i].symbol == targetSymbol) {
                        computer_ship_number[i].life--; 
                        if (computer_ship_number[i].life == 0) {
                            printf("적 함선을 격침시켰습니다!\n");
                            computer_status_board[x][y] = 'O';
                            user_attack_board[x][y] = 'O';
                        }
                        else {
                            printf("적 함선을 공격했으나 아직 무력화되지 않았습니다.\n");
                            if (user_attack_board[x][y] == '!') {
                                printf("재공격을 시도합니다!\n");
                                user_attack_board[x][y] = '!'; 
                            }
                            else {
                                user_attack_board[x][y] = '!'; 
                            }
                        }
                        break; 
                    }
                }
            }
            else if (targetSymbol == '-') {
                printf("적 함선을 놓쳤습니다!\n");
                computer_status_board[x][y] = 'X';
                user_attack_board[x][y] = 'X';
                validAttack = true;
            }
            else if (targetSymbol == 'O') {
                printf("이미 적 함선이 무력화 되었습니다. 더이상 공격할 수 없습니다.\n");
                validAttack = true;
            }
        }
    }
}




void computerAttack()
{
    int x, y;
    while (1)
    {
        x = rand() % MAP_SIZE;
        y = rand() % MAP_SIZE;

        char targetSymbol = user_status_board[x][y]; 

        if (targetSymbol >= 'A' && targetSymbol <= 'D')
        {
            printf("컴퓨터가 함선을 공격합니다.\n");
            for (int i = 0; i < SHIP_NUMBER; i++)
            {
                if (user_ship_number[i].symbol == targetSymbol)
                {
                    user_ship_number[i].life--; 
                    if (user_ship_number[i].life == 0)
                    {
                        printf("컴퓨터가 함선을 격침시켰습니다!\n");
                        user_status_board[x][y] = 'O';
                        computer_attack_board[x][y] = 'O';
                    }
                    else
                    {
                        printf("컴퓨터가 함선을 공격했으나 아직 무력화되지 않았습니다.\n");
                        user_status_board[x][y] = '!';
                        computer_attack_board[x][y] = '!';
                    }
                    break;
                }
            }
            break;
        }
        else if (targetSymbol == '-')
        {
            printf("컴퓨터가 함선을 놓쳤습니다.\n");
            user_status_board[x][y] = 'X';
            computer_attack_board[x][y] = 'X';
            break;
        }
        else if (targetSymbol == 'O')
        {
            printf("컴퓨터가 이미 격침된 함선을 공격하였습니다. 아무런 피해가 없습니다.\n");
            break;
        }
        else if (targetSymbol == '!')
        {
            printf("컴퓨터가 이미 공격 중인 함선을 재포격합니다.\n");
            continue;
        }
    }
}

int isGameEnd(char board[MAP_SIZE][MAP_SIZE])
{
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            if (board[i][j] >= 'A' && board[i][j] <= 'D')
            {
                return 0; 
            }
        }
    }
    return 1; 
}
int main()
{
    srand(time(NULL));

    initBoard(user_status_board);
    initBoard(computer_status_board);
    initBoard(user_attack_board);
    initBoard(computer_attack_board);

    placeShip(user_ship_number, user_status_board);
    placeShip(computer_ship_number, computer_status_board);

    printf("\n\n");
    printf("\t\t====  나의 게임판  ====\n\n");
    printBoard(user_status_board, user_attack_board);
    printf("\n\n");
    printf("\t\t====  컴퓨터 게임판  ====\n\n");
    printBoard(computer_status_board, computer_attack_board);

    while (1)
    {
        userAttack();
        printf("\n\n");
        printBoard(user_status_board, user_attack_board);
        printf("\n\n");
        if (isGameEnd(computer_status_board))
        {
            printf("게임 종료: 사용자의 승리!\n");
            break;
        }

        computerAttack();
        printf("\n\n");
        printBoard(computer_status_board, computer_attack_board);
        printf("\n\n");
        if (isGameEnd(user_status_board))
        {
            printf("게임 종료: 컴퓨터의 승리!\n");
            break;
        }
    }

    return 0;
}
