#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define ROAD_WIDTH 25
#define SCREEN_HEIGHT 20
#define MAX_OBSTACLES 4

// Structure for obstacles
typedef struct {
    int x;
    int y;
    char symbol;
} Obstacle;

// Globals
int score = 0;
int highscore = 0;   
int car_x, car_y;
int game_over = 0;
int game_speed = 100;

Obstacle obstacles[MAX_OBSTACLES];
struct termios original_termios;

// Function declarations
void set_terminal_mode();
void reset_terminal_mode();
void show_menu();
void setup();
void draw();
void input();
void logic();
void load_highscore();      // NEW
void save_highscore();      // NEW

// MAIN ---------------------------------------------------------
int main() {

    load_highscore();   // Load highest score at the start

    show_menu();
    set_terminal_mode();
    setup();

    while (!game_over) {
        draw();
        input();
        logic();
        usleep(game_speed * 1000);
    }

    system("clear");
    printf("\n\n\n\n\n\n\n\n\n");
    printf("     GAME OVER!\n");
    printf("     Your score: %d\n", score);
    printf("     Highest score: %d\n\n", highscore);
    printf("\n\n\n\n\n\n\n\n\n");

    save_highscore();   // Save if new high score achieved

    reset_terminal_mode();
    return 0;
}

// HIGH SCORE FILE HANDLING
void load_highscore() {
    FILE *f = fopen("highscore.txt", "r");
    if (f == NULL) {
        highscore = 0;
        return;
    }
    fscanf(f, "%d", &highscore);
    fclose(f);
}

void save_highscore() {
    if (score > highscore) {
        highscore = score;
        FILE *f = fopen("highscore.txt", "w");
        fprintf(f, "%d", highscore);
        fclose(f);
    }
}

// TERMINAL ------------------------------------------------------
void set_terminal_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    new_termios = original_termios;

    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

// MENU ----------------------------------------------------------
void show_menu() {
    system("clear");

    printf("\n\n\n\n\n");
    printf("        🚗 CAR DODGER 🚗\n\n");
    printf("     Highest Score: %d\n\n", highscore);
    printf("     Press 'a' to move left\n");
    printf("     Press 'd' to move right\n");
    printf("     Press 'w' to move up\n");
    printf("     Press 's' to move down\n");
    printf("     Avoid obstacles (0–9)!\n");
    printf("\n     Press ENTER to start!\n");

    getchar();
    system("clear");
}

// SETUP ---------------------------------------------------------
void setup() {
    srand(time(NULL));

    car_x = ROAD_WIDTH / 2;
    car_y = SCREEN_HEIGHT - 2;

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].x = rand() % ROAD_WIDTH;
        obstacles[i].y = -(rand() % SCREEN_HEIGHT);
        obstacles[i].symbol = '0' + (rand() % 10);
    }

    score = 0;
    game_over = 0;
    game_speed = 100;
}

// DRAW ----------------------------------------------------------
void draw() {
    printf("\033[H\033[J");

    for (int i = 0; i < ROAD_WIDTH + 2; i++) printf("#");
    printf("\n");

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        printf("#");

        for (int x = 0; x < ROAD_WIDTH; x++) {
            int printed = 0;

            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (x == obstacles[i].x && y == obstacles[i].y) {
                    printf("%c", obstacles[i].symbol);
                    printed = 1;
                    break;
                }
            }

            if (!printed) {
                if (x == car_x && y == car_y)
                    printf("V");
                else
                    printf(" ");
            }
        }
        printf("#\n");
    }

    for (int i = 0; i < ROAD_WIDTH + 2; i++) printf("#");
    
    printf("\nScore: %d   High Score: %d\n", score, highscore);
}

// INPUT ---------------------------------------------------------
void input() {
    char ch;
    if (read(STDIN_FILENO, &ch, 1) > 0) {
        switch (ch) {
            case 'a': if (car_x > 0) car_x--; break;
            case 'd': if (car_x < ROAD_WIDTH - 1) car_x++; break;
            case 'w': if (car_y > 0) car_y--; break;
            case 's': if (car_y < SCREEN_HEIGHT - 1) car_y++; break;
            case 'q': game_over = 1; break;
        }
    }
}

// LOGIC ---------------------------------------------------------
void logic() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {

        if (obstacles[i].x == car_x && obstacles[i].y == car_y) {
            game_over = 1;
            return;
        }

        obstacles[i].y++;

        if (obstacles[i].x == car_x && obstacles[i].y == car_y) {
            game_over = 1;
            return;
        }

        if (obstacles[i].y >= SCREEN_HEIGHT) {
            score++;

            obstacles[i].x = rand() % ROAD_WIDTH;
            obstacles[i].y = 0;
            obstacles[i].symbol = '0' + (rand() % 10);

            if (score % 5 == 0 && game_speed > 25)
                game_speed -= 5;
        }
    }
}
