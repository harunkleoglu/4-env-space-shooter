#include "keyboard.h"
#include "string.h"
#include "graphics.h"
#include "ioports.h"
#include "pseudo_random.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define RIGHT_KEYCODE 77
#define LEFT_KEYCODE 75
#define SPACE_KEYCODE 57
#define ENTER_KEYCODE 28

struct graphics *g;
struct console *c;

int game_started = 0;
int ship_x = SCREEN_WIDTH / 2;
int ship_y = SCREEN_HEIGHT - 100;

int health = 100;
int score = 0;

int meteors[5][2];
int meteors_init = 0;

int bullet_x;
int bullet_y;
int shot_fired = 0;

void print_integer(int n, int x, int y)
{
	if(n<0) return;
	char str[20];
	*uint_to_string(n, str);
	print_string(str, x, y);
}

void print_string(char *str, int x, int y)
{
	for (int i = 0; i < strlen(str); i++)
	{
		graphics_char(g, x + (8 * i), y, str[i]);
	}
}

void draw_ship()
{
	int x = ship_x;
	int y = ship_y;
	// Draws ship graphics
	graphics_line(g, x, y, 15, 15);
	graphics_line(g, x + 15, y + 15, 0, 5);
	graphics_line(g, x + 15, y + 20, 35, 20);
	graphics_line(g, x + 50, y + 40, -40, 0);
	graphics_line(g, x + 10, y + 40, 0, 5);
	graphics_line(g, x + 10, y + 45, -20, 0);
	graphics_line(g, x - 10, y + 45, 0, -5);
	graphics_line(g, x - 10, y + 40, -40, 0);
	graphics_line(g, x - 50, y + 40, 35, -20);
	graphics_line(g, x - 15, y + 20, 0, -5);
	graphics_line(g, x - 15, y + 15, 15, -15);
}

void keyboard_handler()
{
	int keycode = inb(0x60);
	if (keycode == RIGHT_KEYCODE)
	{
		if (ship_x < SCREEN_WIDTH - 100)
		{
			ship_x += 15;
		}
		return;
	}
	if (keycode == LEFT_KEYCODE)
	{
		if (ship_x > 100)
		{
			ship_x -= 15;
		}
		return;
	}
	if (keycode == SPACE_KEYCODE && shot_fired == 0) shot_fired = 1;
}

void meteor_handler()
{
	if (meteors_init == 0)
	{
		for (int i = 0; i < 5; i++)
		{
			// meteor[meteor_number][coordinate] 0=x, 1=y
			// rand_range function based on pseudo_random.h header file. I implemented this header.
			meteors[i][0] = rand_range((SCREEN_WIDTH / 8) * 2, (SCREEN_WIDTH / 8) * 6);
			meteors[i][1] = rand_range(-SCREEN_HEIGHT, 0);
		}
		meteors_init = 1;
	}

	for (int i = 0; i < 5; i++)
	{
		meteors[i][1] = meteors[i][1] + 10;
		if(meteors[i][0] > ship_x-100 && meteors[i][0] <= ship_x+50 && meteors[i][1] > ship_y - 50 && meteors[i][1] <= ship_y){
			health -= 10;
			meteors[i][0] = rand_range((SCREEN_WIDTH / 8) * 2, (SCREEN_WIDTH / 8) * 6);
			meteors[i][1] = rand_range(-SCREEN_HEIGHT, 0);
		}
		if (meteors[i][1] < SCREEN_HEIGHT && meteors[i][1] > 0)
		{
			graphics_rect(g, meteors[i][0], meteors[i][1], 50, 50);
		}
		else
		{
			meteors[i][0] = rand_range((SCREEN_WIDTH / 8) * 2, (SCREEN_WIDTH / 8) * 6);
			meteors[i][1] = rand_range(-SCREEN_HEIGHT, 0);
		}
	}
}

void render_status_bar(){
	print_string("Health:",30,20);
	print_integer(health,30+(8*strlen("Health:")),20);
	print_string("Score:",SCREEN_WIDTH-100,20);
	print_integer(score,SCREEN_WIDTH-100+(8*strlen("Score:")),20);
	graphics_clear(g,health == 100 ? 110 : 102 ,0,800,35);
}

void render_bullet()
{
    graphics_rect(g,bullet_x,bullet_y+10,1,10);
}

void check_meteor_collision()
{
    int i;
    for(i=0; i<5; i++)
    {
        if(bullet_x <= meteors[i][0]+50 && bullet_x >= meteors[i][0] && bullet_y <= meteors[i][1]+50 && bullet_y >= meteors[i][1])
        {
            shot_fired = 0;
            meteors[i][0] = rand_range((SCREEN_WIDTH / 8) * 2, (SCREEN_WIDTH / 8) * 6);
			meteors[i][1] = rand_range(-SCREEN_HEIGHT, 0);
            score += 10;
        }
    }
}

void weapon_handler()
{
    if(bullet_y<35) shot_fired = 0;

    if(shot_fired == 1){
        render_bullet();
        bullet_y -= 20;
        check_meteor_collision();
    }
    else{
        bullet_x = ship_x;
        bullet_y = ship_y - 10;
    }
}


void start_screen(){
	char *welcome_message = "Welcome to Space Invaders!"; 
	print_string(welcome_message,(SCREEN_WIDTH/2)-(4*strlen(welcome_message)),SCREEN_HEIGHT/35*17);
	char *start_message = "Press \"Enter\" to Start Game"; 
	print_string(start_message,(SCREEN_WIDTH/2)-(4*strlen(start_message)),SCREEN_HEIGHT/35*18);

	while(!game_started){
		int keycode = inb(0x60);
		if(keycode == ENTER_KEYCODE){
			game_started = 1;
		}
	}
}

void end_screen(){
	graphics_clear(g, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	char *gameover_str = "Game Over!"; 
	print_string(gameover_str,(SCREEN_WIDTH/2)-(4*strlen(gameover_str)),SCREEN_HEIGHT/35*17);
	char *restart_str = "Press \"Enter\" to Restart Game"; 
	print_string(restart_str,(SCREEN_WIDTH/2)-(4*strlen(restart_str)),SCREEN_HEIGHT/35*18);

	while(!game_started){
		int keycode = inb(0x60);
		if(keycode == ENTER_KEYCODE){
			game_started = 1;
			score = 0;
			health = 100;
			meteors_init = 0;
		}
	}
}

void game()
{
	while (1)
	{
		graphics_clear(g, 0, 35, SCREEN_WIDTH, SCREEN_HEIGHT);
		keyboard_handler();
		draw_ship();
		meteor_handler();
		weapon_handler();
		render_status_bar();
		if(health<=0){
			meteors_init = 0;
			game_started = 0;
			end_screen();
		}
	}
}

int kernel_main()
{
	// Initialize Graphics
	g = graphics_create_root();

	keyboard_init();
	start_screen();
	game();
	return 0;
}
