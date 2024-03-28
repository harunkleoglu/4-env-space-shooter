#include <graphics.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <random>

using namespace std;

// Predefined ASCII codes of keys
#define ENTER_KEYCODE 13
#define RIGHT_ARROW_KEYCODE 77
#define LEFT_ARROW_KEYCODE 75
#define SPACE_KEYCODE 32

// Screen size adjustments
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Initial variables
int game_started = 0;
int meteors[5][2];
int meteor_init = 0;
int ship_x = SCREEN_WIDTH/2;
int ship_y = SCREEN_HEIGHT - 70;
int shot_fired = 0;
int bullet_x;
int bullet_y;
int health = 100;
int score = 0;

// This function handles keys pressed during gameplay

void countdown()
{
    clearviewport();
    int i;
    for(i=3; i>=1; i--)
    {
        char str[1];
        sprintf(str, "%d", i);
        outtextxy(SCREEN_WIDTH/2,(SCREEN_HEIGHT/19)*9, str);
        delay(1000);
        clearviewport();
    }
}
void keyboard_handler()
{
    if(kbhit())
    {
        int keycode = getch();

        if(keycode == RIGHT_ARROW_KEYCODE && ship_x <= SCREEN_WIDTH - 40) // Move Right
        {
            ship_x += 10;
        }

        if(keycode == LEFT_ARROW_KEYCODE && ship_x >= 40) // Move Left
        {
            ship_x -= 10;
        }

        if(keycode == SPACE_KEYCODE && shot_fired == 0) shot_fired = 1; // Attacks
    }
}

int random_number(int start, int end) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(start, end);
    return dis(gen);
}

// This function handles meteor initialization, meteor rain and randomization of meteors coordinates.
void meteor_handler()
{
    if(game_started == 1 && meteor_init == 0)
    {
        int i;
        for(i = 0; i<5; i++)
        {
            // initialize meteors out of screen.
            meteors[i][0] = random_number((SCREEN_WIDTH/8)*2, (SCREEN_WIDTH/8)*6);
            meteors[i][1] = random_number(-SCREEN_HEIGHT, 0);
        }
        meteor_init = 1;
    }
    int j;
    for(j = 0; j<5; j++)
    {
        if((meteors[j][1] > ship_y && meteors[j][1] < ship_y+40) && (meteors[j][0] > ship_x-90 && meteors[j][0] < ship_x+90))
        {
            health -=10;
            meteors[j][0] = random_number((SCREEN_WIDTH/6)*2, (SCREEN_WIDTH/6)*4);
            meteors[j][1] = random_number(-SCREEN_HEIGHT, 0);
        }

        if(meteors[j][1] < SCREEN_HEIGHT)
        {
            meteors[j][1]+=3; // moves meteor #j downward.
        }
        else
        {
            // if meteor is out of screen, reinitialize meteor at top.
            meteors[j][0] = random_number((SCREEN_WIDTH/6)*2, (SCREEN_WIDTH/6)*4);
            meteors[j][1] = random_number(-SCREEN_HEIGHT, 0);
        }
        fillellipse(meteors[j][0],meteors[j][1], 20,20);
    }
}

void render_ship()
{
    int x = ship_x;
    int y = ship_y;
    // Draws ship graphics
    line(x,y,x+15,y+15);
    line(x+15,y+15,x+15,y+20);
    line(x+15,y+20,x+50,y+40);
    line(x+50,y+40,x+10,y+40);
    line(x+10,y+40,x+10,y+45);
    line(x+10,y+45,x-10,y+45);
    line(x-10,y+45,x-10,y+40);
    line(x-10,y+40,x-50,y+40);
    line(x-50,y+40,x-15,y+20);
    line(x-15,y+20,x-15,y+15);
    line(x-15,y+15,x,y);
}

void render_bullet()
{
    line(bullet_x,bullet_y,bullet_x,bullet_y+10);
}

void check_meteor_collision()
{
    int i;
    for(i=0; i<5; i++)
    {
        if(bullet_x <= meteors[i][0]+20 && bullet_x >= meteors[i][0]-20 && bullet_y <= meteors[i][1]+20 && bullet_y >= meteors[i][1]-20)
        {
            shot_fired = 0;
            meteors[i][0] = random_number((SCREEN_WIDTH/8)*2, (SCREEN_WIDTH/8)*6);
            meteors[i][1] = random_number(-SCREEN_HEIGHT/2, 0);
            score += 10;
        }
    }
}

void weapon_handler()
{
    if(bullet_y<0) shot_fired = 0;

    if(shot_fired == 1){
        render_bullet();
        bullet_y-= 11;
        check_meteor_collision();
    }
    else{
        bullet_x = ship_x;
        bullet_y = ship_y - 10;
    }
}

void render_status_bar()
{
    char str[10];
    settextjustify(2,2);
    outtextxy(100,SCREEN_HEIGHT-30,"Health:");
    sprintf(str, "%d", health);
    settextjustify(0,2);
    outtextxy(100,SCREEN_HEIGHT-30,str);

    settextjustify(2,2);
    outtextxy(SCREEN_WIDTH-70,SCREEN_HEIGHT-30,"Score:");
    sprintf(str, "%d", score);
    settextjustify(0,2);
    outtextxy(SCREEN_WIDTH-68,SCREEN_HEIGHT-30,str);
}
// Main Game Loop

void handle_input()
{
    int keycode = getch();
    if(keycode == ENTER_KEYCODE)
    {
        clearviewport();
        game_started = 1;
        countdown();
    }
}

void start_screen()
{
    // Centers the text
    settextjustify(1, 1);
    // Created 2 x 18 grid to center the start game texts.
    outtextxy(SCREEN_WIDTH/2,(SCREEN_HEIGHT/19)*9,  "Welcome to Space Invaders");
    outtextxy(SCREEN_WIDTH/2,(SCREEN_HEIGHT/19)*10, "Press \"Enter\" to Start Game");

    while(!game_started)
    {
        handle_input();
        delay(100);
    }
}

void end_screen()
{
    // Centers the text
    settextjustify(1, 1);
    // Created 2 x 18 grid to center the start game texts.
    outtextxy(SCREEN_WIDTH/2,(SCREEN_HEIGHT/19)*9,  "Game Over");
    outtextxy(SCREEN_WIDTH/2,(SCREEN_HEIGHT/19)*10, "Press \"Enter\" to Restart Game");

    while(!game_started)
    {
        handle_input();
        health = 100;
        score = 0;
        delay(100);
    }
}

void stardust(){
}

void game()
{
    while(1)
    {
        clearviewport(); // Clears game screen to avoid overwritten objects.
        if(health <= 0)
        {
            game_started = 0;
            meteor_init = 0;
            end_screen();
        }
        render_ship();
        keyboard_handler();
        meteor_handler();
        weapon_handler();
        render_status_bar();

        delay(5); // 5 Milliseconds delay to adjust game speed.
    }
}

int main(void)
{
    // Initialization of window
    initwindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");
    settextstyle(2,0,8);
    start_screen();
    game();
}
