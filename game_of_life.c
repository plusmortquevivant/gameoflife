#include<stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#define N 25
#define M 80
#define ALIVE '0'
#define DEAD ' '

int input(char arr[N][M]);
void live(char arr[N][M]);
void draw(char arr[N][M]);
int tick(char arr[N][M]);
void play(int argc, char **argv, char arr[N][M]);
int file_input(FILE *fp, char arr[N][M]);
char survival_check(char state, int amount_of_live_neighbours);
int count_alive_neighbours(char arr[N][M], int i, int j);
int kbhit(void);

int main(int argc, char **argv) {
    char arr[N][M];

    play(argc, argv, arr);
    return 0;
}

void play(int argc, char **argv, char arr[N][M]) {
    FILE *fp;
    if (argc == 1) {
        if (input(arr) == 0) {
            live(arr);
        } else {
            printf("Wrong input");
        }
    } else if (argc == 2) {
        if ((fp = fopen(argv[1], "r")) == NULL) {
            printf("Cannot open file\n");
        } else {
            if (file_input(fp, arr) == 0) {
            fclose(fp);
            live(arr);
            } else {
                printf("Wrong input");
            }
        }
    } else {
        printf("Wrong amount of arguments. Want: filename or nothing.");
    }
}

int file_input(FILE *fp, char arr[N][M]) {
    int err = 0;
    for (int i = 0; i < N && err == 0; i++) {
        for (int j = 0; j < M; j++) {
            if (fscanf(fp, "%c", &arr[i][j]) != 1 ||
                ((arr[i][j] != ALIVE) &&
                (arr[i][j] != DEAD))) {
                if (arr[i][j] == '\n') {
                    j--;
                } else {
                    err = 1;
                    break;
                }
            }
        }
    }
    return err;
}

int input(char arr[N][M]) {
    int err = 0;
    for (int i = 0; i < N && err == 0; i++) {
        for (int j = 0; j < M; j++) {
            if (scanf("%c", &arr[i][j]) != 1 ||
                ((arr[i][j] != ALIVE) &&
                (arr[i][j] != DEAD))) {
                if (arr[i][j] == '\n') {
                    j--;
                } else {
                    err = 1;
                    break;
                }
            }
        }
    }
    return err;
}

void live(char arr[N][M]) {
    draw(arr);
    char c;
    int end = 0;
    int speed = 1;
    while (1) {
        if (kbhit()) {
            c = getchar();
            if (c == 'q') {
                break;
            } else if (c >= '0' && c <= '9') {
                speed = c - '0';
            } else {
                printf("\nincorrect input\n");
            }
        } else {
            for (int i = 0; i < speed; i++) {
                end = tick(arr);
                draw(arr);
            }
            if (end != 0) {
                break;
            }
            sleep(1);
        }
    }
    printf("\nGAME OVER\n");
}

int kbhit(void) {
    // new and old terminal configurations
    struct termios oldt, newt;
    int ch;
    int oldf;
    int res;
    res = 0;

    // copy stdin params into oldt
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // set local mode flag
    newt.c_lflag &= ~(ICANON | ECHO);
    // update stdin params
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    // copy status flags
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    // add nonblock flag
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // return stdin params
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    // return status flags
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
      ungetc(ch, stdin);
      res = 1;
    }

    return res;
}

int tick(char arr[N][M]) {
    char temp_arr[N][M];
    int neighbours_alive;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            neighbours_alive = count_alive_neighbours(arr, i, j);
            temp_arr[i][j] = survival_check(arr[i][j], neighbours_alive);
        }
    }
    int live_amount = 0, res = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            arr[i][j] = temp_arr[i][j];
            if (arr[i][j] == ALIVE) {
                live_amount++;
            }
        }
    }
    if (live_amount == 0 || live_amount == N * M) {
        res = 1;
    }
    return res;
}

void draw(char arr[N][M]) {
    printf("   █   █ ███ ███\n");
    printf("   █     █   █       <0-9>-speed\n");
    printf("** █   █ ███ ███ **  <q>-выход\n");
    printf("   █   █ █   █\n");
    printf("   ███ █ █   ███\n");
    for (int a = 0; a < M + 2; a++) {
        printf("-");
    }
    printf("\n");
    for (int i=0; i < N; i++) {
        printf("|");
        for (int j=0; j < M; j++) {
            printf("%c", arr[i][j]);
        }
        printf("|\n");
    }
    for (int b = 0; b < M + 2; b++) {
        printf("-");
    }
    printf("\n");
}


char survival_check(char state, int amount_of_live_neighbours) {
    char res;
    if (state == ALIVE) {
        if (amount_of_live_neighbours == 2 ||
            amount_of_live_neighbours == 3) {
            res = ALIVE;
        } else {
            res = DEAD;
        }
    } else {
        if (amount_of_live_neighbours == 3) {
            res = ALIVE;
        } else {
            res = DEAD;
        }
    }
    return res;
}

int count_alive_neighbours(char arr[N][M], int i, int j) {
    int amount_of_live_neighbours = 0;
    int j_r, j_l, i_u, i_d;
    i_u = i == 0 ? N - 1 : i - 1;
    i_d = i == N - 1 ? 0 : i + 1;
    j_r = j == M - 1 ? 0 : j + 1;
    j_l = j == 0 ? M - 1 : j - 1;
    if (arr[i_u][j_l] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    if (arr[i_u][j] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    if (arr[i_u][j_r] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    if (arr[i][j_r] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    if (arr[i_d][j_r] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    if (arr[i_d][j] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    if (arr[i_d][j_l] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    if (arr[i][j_l] == ALIVE) {
        amount_of_live_neighbours += 1;
    }
    return amount_of_live_neighbours;
}
