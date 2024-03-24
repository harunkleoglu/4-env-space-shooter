#include <iostream>
#include <windows.h>
#include <conio.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <thread>

using namespace std;

#define COLUMNS_IN_LINE 80
#define LINES 25
#define ENTER_KEYCODE 13
#define LEFT_ARROW_KEYCODE 75
#define RIGHT_ARROW_KEYCODE 77
#define SPACE_KEYCODE 32

// meteor
int meteors[5][2];
int meteor_y = 0;
int meteor_x = 10;

// weapon
int bullet_x;
int bullet_y;
int shot_fired = 0;

// ship
int ship_x = COLUMNS_IN_LINE/2;
int ship_y = 23;
int ship_direction = 0;

// game manager
int gameStart = 0;
int health = 100;
int score = 0;
int meteors_init = 0;
int restart = 0;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

void move_cursor(short x, short y)
{
    COORD position = {x,y};
    SetConsoleCursorPosition(console, position);
}

void draw_str(const char* str, unsigned int x, unsigned int y)
{
    move_cursor(x,y);
    cout << str;
}

void draw_int(int val, unsigned int x, unsigned int y)
{
    move_cursor(x,y);
    cout << val;
}

void sleep_ms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void start_screen()
{
    system("CLS");
    move_cursor(0,11);
    cout << "                           Welcome to Space Invaders!                           ";
    move_cursor(0,12);
    cout << "                            Press \"Enter\" to Start!                         ";
    char key;
    while(1)
    {
        key = _getch();
        if(key == ENTER_KEYCODE) gameStart = 1; // Pressed enter
        if(gameStart==1)
        {
            break;
        }
    }
}

int random_number(int start, int end)
{
    return (rand() % (end-start))+start;
}

void clear_meteor(int x, int y)
{
    const char *blank = "   ";
    draw_str(blank,x,y);
}

void draw_meteor(int x, int y)
{
    const char *meteor_s = "[ ]";
    draw_str(meteor_s,x,y);
}

void meteor_handler(void)
{

    // initializes meteors in the beginning.
    if(meteors_init == 0)
    {
        int i;
        for(i = 0; i<5; i++)
        {
            // meteor[meteor_number][coordinate] 0=x, 1=y
            // rand_range function based on pseudo_random.h header file. I implemented this header.
            meteors[i][0] = random_number(15, 60);
            meteors[i][1] = random_number(-50,0);
        }
        meteors_init = 1;
    }
    int i;
    for(i = 0; i<5; i++)
    {

        //meteor and ship's collision detection.
        if(meteors[i][1] == ship_y && (meteors[i][0] > ship_x-2 && meteors[i][0] < ship_x+4))
        {
            health -=10;
        }

        // handles meteor rain.
        clear_meteor(meteors[i][0],meteors[i][1]);
        if(meteors[i][1] < 25)
        {
            meteors[i][1] = meteors[i][1] + 1;
            if(meteors[i][1] < 0) continue;
            draw_meteor(meteors[i][0],meteors[i][1]);
        }
        else
        {
            meteors[i][1] = random_number(-50,0);
            meteors[i][0] = random_number(15,60);
        }
    }

}
void draw_ship(void)
{
    // draws ship
    const char *ship_s = "< ^ >";
    draw_str(ship_s,ship_x,ship_y);
}

void render_ship(void)
{
    const char *blank = " ";
    if(ship_direction == 0)
    {
        draw_ship(); // idle/init
        return;
    }
    if(ship_direction == 1)
    {
        draw_str(blank, ship_x-2, ship_y);
        draw_ship(); // moving right
        return;
    }
    if(ship_direction == -1)
    {
        draw_str(blank, ship_x+6, ship_y);
        draw_ship(); // moving left
        return;
    }
}

void weapon_handler(void)
{
    const char *bullet_str = "|";
    // reset bullet's location
    if(shot_fired == 0)
    {
        bullet_x = 0;
        bullet_y = 0;
    }
    // move bullet
    if(shot_fired == 1)
    {
        if(bullet_x == 0 && bullet_y == 0)
        {
            bullet_x = ship_x+2;
            bullet_y = ship_y + 1;
        }
        const char *blank = " ";
        draw_str(blank,bullet_x,bullet_y);
        bullet_y = bullet_y - 2;
        draw_str(bullet_str,bullet_x,bullet_y);
        if(bullet_y <= 0)
        {
            draw_str(blank,bullet_x,bullet_y);
            bullet_y = ship_y + 1;
            shot_fired = 0;
        }
    }
    int i;
    for(i = 0; i<5; i++)
    {
        // if bullet hits a meteor
        if(meteors[i][0]-1 <= bullet_x && meteors[i][0]+2 >= bullet_x && (meteors[i][1] <= bullet_y && meteors[i][1] >= bullet_y-2))
        {
            score += 10;
            clear_meteor(meteors[i][0], meteors[i][1]);
            clear_meteor(meteors[i][0], meteors[i][1]+1);
            clear_meteor(meteors[i][0], meteors[i][1]+2);
            meteors[i][0] = random_number(15, 60);
            meteors[i][1] = random_number(-50,0);
            bullet_y = ship_y + 1;
            shot_fired = 0;
        }
    }
}

void render_status_bar(void)
{

    // clears status bar.
    const char *clear_status_bar = 	"                                                                                ";
    draw_str(clear_status_bar,0,24);

    // renders status bar
    const char *status_bar = 		"Health:                                                               Score:    ";
    draw_str(status_bar, 0,24);

    // print health and score values with print_integer function.
    //move_cursor(7,24);
    draw_int(health,7,24);
    //move_cursor(76,24);
    draw_int(score,76,24);
}

void end_screen()
{
    system("CLS");
    move_cursor(0,11);
    cout << "                                   Game Over                                  ";
    move_cursor(0,12);
    cout << "                            Press \"Enter\" to Restart!                     ";
    move_cursor(0,13);
    cout << "                                 Your Score:";
    cout << score;

    char key;
    while(1)
    {
        key = _getch();
        if(key == ENTER_KEYCODE)
        {
            // Restart Game
            gameStart = 1;
            system("CLS");
            meteors_init = 0;
            health = 100;
            score = 0;
            restart = 1;
            break;
        }
    }
}

void game()
{
    system("CLS");
    while(1)
    {
        ship_direction = 0; // ship is idle
        meteor_handler();
        weapon_handler();
        render_ship();
        render_status_bar();
        if(health <= 0)
        {
            // game over
            gameStart = 0;
            end_screen();
        }

        // controls
        char key;
        if(kbhit())
        {
            key = _getch();

            if(key == RIGHT_ARROW_KEYCODE)
            {
                if(gameStart == 1)
                {
                    if(ship_x == COLUMNS_IN_LINE-5)
                    {
                        sleep_ms(70);
                        continue;
                    }
                    ship_x = ship_x + 2;
                    ship_direction = 1;
                    render_ship();
                }
            }
            if(key == LEFT_ARROW_KEYCODE)
            {
                if(gameStart == 1)
                {
                    if(ship_x == 0)
                    {
                        sleep_ms(70);
                        continue;
                    }
                    ship_x = ship_x - 2;
                    ship_direction = -1;
                    render_ship();
                }
            }
            if(key == SPACE_KEYCODE)
            {
                if(shot_fired == 0)
                {
                    shot_fired = 1;
                }
            }
        }
        sleep_ms(70); // sleep function
    }
}

int main()
{
    time_t t;
    srand((unsigned) time(&t));

    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(console, &bufferInfo);

    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;

    SetConsoleCursorInfo(console, &info);
    SMALL_RECT windowSize = {0, 0, 80, 25};
    SetConsoleWindowInfo(console, TRUE, &windowSize);


    // Game Loop
    start_screen();
    game();

    getchar();
    return 0;
}

