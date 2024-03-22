#include "keyboard_map.h"
#include "pseudo_random.h"

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08


// Keyboard keycodes.
#define ENTER_KEY_CODE 0x1C
#define LEFT_KEY_CODE 0x4B 
#define RIGHT_KEY_CODE 0x4D 
#define SPACE_KEY_CODE 0x39 

extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;

void move_cursor(unsigned int x, unsigned int y);
void renderer(unsigned int inst_cout);
void draw_str(const char *str, unsigned int x, unsigned int y);
void clear_meteor(int x, int y);
void draw_meteor(int x, int y);
void render_status_bar(void);
void weapon_handler(void);
void meteor_handler(void);
void draw_ship(void);
void render_ship(void);
void start_screen(void);
void end_screen(void);
void game(void);

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

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];


void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}


void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x07;
	}
}

void kprint_int(int n, int color) // printing integer out of buffer
{
    int i = 0;
    char buffer[20];  
    do {
        buffer[i++] = (n % 10) + '0';  
        n /= 10;
    } while (n != 0);
    while (i > 0) {
      vidptr[current_loc++] = buffer[--i];
      vidptr[current_loc++] = color;
    }
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
}

void keyboard_handler_main(void)
{
	unsigned char status;
	char keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;

		if(keycode == ENTER_KEY_CODE) {
			gameStart = 1;
			return;
		}
		if(keycode == LEFT_KEY_CODE){
			if(gameStart == 1){			
				if(ship_x == 0) return;
				ship_x = ship_x - 1;
				ship_direction = -1;
				render_ship();
			}
		}
		if(keycode == RIGHT_KEY_CODE){
			if(gameStart == 1){			
				if(ship_x == COLUMNS_IN_LINE-5) return;
				ship_x = ship_x + 1;
				ship_direction = 1;
				render_ship();
			}
		}
		
		if(keycode == SPACE_KEY_CODE){
			if(shot_fired == 0){
				shot_fired = 1;
			}
		}

		vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
		vidptr[current_loc++] = 0x07;
	}
}


// move cursor to certain x, y coordinate
void move_cursor(unsigned int x, unsigned int y){
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = BYTES_FOR_EACH_ELEMENT * (x * COLUMNS_IN_LINE + y);
}

void renderer(unsigned int inst_cout){ 
	int i;
	for(i=0; i<inst_cout;i++); // busy wait..
}

void draw_str(const char *str, unsigned int x, unsigned int y){
	move_cursor(x,y);
	kprint(str);
}

void clear_meteor(int x, int y)
{
	const char *blank = "   ";
	draw_str(blank,y,x);
}

void draw_meteor(int x, int y) 
{
	const char *meteor_s = "[ ]";
	draw_str(meteor_s,y,x);
}

void render_status_bar(void){

	// clears status bar.
	const char *clear_status_bar = 	"                                                                                ";
	draw_str(clear_status_bar,24,0);
	
	// renders status bar
	const char *status_bar = 		"Health:                                                               Score:    ";
	draw_str(status_bar, 24,0);
	
	// print health and score values with print_integer function.
	move_cursor(24,7);
	kprint_int(health,0x04);
	move_cursor(24,76);
	kprint_int(score,0x02);
}

void weapon_handler(void){
	const char *bullet_str = "|";
	// reset bullet's location
	if(shot_fired == 0){
		bullet_x = ship_x+2;	
		bullet_y = ship_y + 1;
	}
	// move bullet
	if(shot_fired == 1){
		const char *blank = " ";
		draw_str(blank,bullet_y,bullet_x);
		bullet_y = bullet_y - 2;
		if(bullet_y < 0){
			bullet_y = ship_y + 1; 
			shot_fired = 0;
		}
		draw_str(bullet_str,bullet_y,bullet_x);		
	}
	int i;
	for(i = 0; i<5; i++){
			// if bullet hits a meteor
			if(meteors[i][0]-1 <= bullet_x && meteors[i][0]+2 >= bullet_x && (meteors[i][1] <= bullet_y && meteors[i][1] >= bullet_y-2)){
				score += 10;
				clear_meteor(meteors[i][0], meteors[i][1]);
				clear_meteor(meteors[i][0], meteors[i][1]+1);
				clear_meteor(meteors[i][0], meteors[i][1]+2);
				meteors[i][0] = rand_range(15, 60);
				meteors[i][1] = rand_range(-50,0);
				bullet_y = ship_y + 1;
				shot_fired = 0;
			}
	}
}

void meteor_handler(void){

	// initializes meteors in the beginning.
	if(meteors_init == 0){
		int i;	
		for(i = 0; i<5; i++){
			// meteor[meteor_number][coordinate] 0=x, 1=y
			// rand_range function based on pseudo_random.h header file. I implemented this header.
			meteors[i][0] = rand_range(15, 60);
			meteors[i][1] = rand_range(-50,0);
		}
		meteors_init = 1;
	}
	int i;
	for(i = 0; i<5; i++){
	
		//meteor and ship's collision detection.
		if(meteors[i][1] == ship_y && (meteors[i][0] > ship_x-2 && meteors[i][0] < ship_x+4)){
			health -=10;
		}
		
		// handles meteor rain.
		clear_meteor(meteors[i][0],meteors[i][1]);
		if(meteors[i][1] < 25){	
			meteors[i][1] = meteors[i][1] + 1;
			draw_meteor(meteors[i][0],meteors[i][1]);
		}
		else{		
			meteors[i][1] = rand_range(-50,0);
			meteors[i][0] = rand_range(15,60);
		}
	}
	
}

void draw_ship(void){
	// draws ship
	const char *ship_s = "< ^ >";
	draw_str(ship_s,ship_y,ship_x);
}


void render_ship(void){
	const char *blank = " ";
	if(ship_direction == 0){
		draw_ship(); // idle/init
		return;
	}
	if(ship_direction == 1){
		draw_str(blank, ship_y, ship_x-1);
		draw_ship(); // moving right
		return;
	}
	if(ship_direction == -1){
		draw_str(blank, ship_y, ship_x+5);
		draw_ship(); // moving left
		return;
	}
}

void start_screen(void){
	clear_screen();
	const char *header = 	"                           Welcome to Space Invaders!                           ";
	const char *info = 		"                            Press \"Enter\" to Start!                         ";
	draw_str(header, 11, 0);
	draw_str(info, 12, 0);
	
	while(1){ if(gameStart==1){break;}}
}

void end_screen(){
	clear_screen();
	const char *header = 	"                                   Game Over                                  ";
	const char *info = 		"                            Press \"Enter\" to Restart!                     ";
	const char *stat = 		"                                 Your Score:";
	draw_str(stat, 13, 0);
	kprint_int(score,0x04);
	draw_str(header, 11, 0);
	draw_str(info, 12, 0);
	
	while(1){ 
		// Restart Game
		if(gameStart==1){
			clear_screen();
			meteors_init = 0;
			health = 100;
			score = 0;
			game(); 
		
		}
	}
}

void game(void){
	clear_screen();
	while(1){
	
		ship_direction = 0; // ship is idle
		meteor_handler();
		weapon_handler();
		render_ship();
		render_status_bar();
		
		if(health <= 0){
			// game over
			gameStart = 0;
			end_screen();
			break;
		}
		renderer(31000000); // busy wait 
	}
}

void kmain(void)
{
	random_seed_by_cpu_cycle(); // pseudo_random.h requires
	idt_init();
	kb_init();
	start_screen();
	game();
	while(1);
}
