//
//	Project Attack of the Circles
//
//	By - Adwait Rawats
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>

#define ANGLE_FACTOR 1.107148718
#define pi 3.141592654

using namespace std;

const unsigned short int bullet_limit = 15, star_number = 5000, font_size = 84;
unsigned short int side = 15, thickness = 1, paused = 0, ptr = 1;
int times, timesl, timeS, timeSl, timegod, timegodl, timet, timetl, timep, timepl, timeg, timegl, score = 0;
float sx[star_number], sy[star_number];
bool God_Mode = false, time_stop = false;
ALLEGRO_FONT *score_font = NULL;

class player {
	public:
	float speed, angle_speed;
	float x, y, r;
	float theta, angle, left, right;
	
	player() {
		angle = 90;
		x = 320; y = 240;
		theta = pi*angle/180;
		r = 11.180339887*3/2;
		speed = 2;
		angle_speed = 2;
	}
	void draw() {
		theta = pi*angle/180;
		left = theta - (pi/2) - ANGLE_FACTOR;
		right = theta + (pi/2) + ANGLE_FACTOR;
		al_draw_triangle(x - side*cos(theta), y - side*sin(theta), x - r*cos(right), y - r*sin(right), x - r*cos(left), y - r*sin(left), al_map_rgb(255, 255, 255), thickness);
	}
	void move(int dir) {
		x += (dir*speed*cos(theta));
		y += (dir*speed*sin(theta));
	}
}Ship;

class bullet {
	public:
	float speed, state;
	float x, y;
	float theta;

	bullet(){
		speed = 4;
	}

	void set(float bx, float by, float t, int s = 0) {
		x = bx;
		y = by;
		theta = t;
		state = s;
	}
	void draw() {
		if (state == 1)
			al_draw_circle(x, y, 3*thickness, al_map_rgb(255, 255, 255), thickness);
	}
	void move() {
		if (state == 1) {
			x -= (speed*cos(theta));
			y -= (speed*sin(theta));
			if (x > 640 || y > 480 || x < 0|| y < 0) {
				set(0, 0, 0);
			}
		}
	}
}Bullet[bullet_limit];

class asteroid {
	public:
	float x, y;
	int state, radius;
	float speed;
	float theta;
	asteroid() {
		radius = 40;
		speed = 4;
		generate();
	}
	void generate() {
		int udlr = rand()%4;
		state = 1;
		if (udlr == 0) {
			x = rand()%(640 + 2*radius) - radius;
			y = -radius;
		}
		else if (udlr == 1) {
			x = rand()%(640 + 2*radius) - radius;
			y = 480 + radius;
		}
		else if (udlr == 2) {
			x = -radius;
			y = rand()%(480 + 2*radius) - radius;
		}
		else if (udlr == 3) {
			x = 640 + radius;
			y = rand()%(480 + 2*radius) - radius;
		}
		if (x <= 320)
			theta = pi + atan((Ship.y - y)/(Ship.x - x));
		else
			theta = atan((Ship.y - y)/(Ship.x - x));
	}
	void draw() {
		al_draw_circle(x, y, radius, al_map_rgb(255, 255, 255), thickness*3);
	}
	void move() {
			x -= (speed*cos(theta));
			y -= (speed*sin(theta));
		if (x <= -40 || x >= 680 || y <= -40 || y >= 520) {
			state = 0;
		}
	}
	bool hit() {
		for (int i = 0; i < bullet_limit; i++) {
			if (Bullet[i].state == 1) {
				if (sqrt((Bullet[i].x - x)*(Bullet[i].x - x) + (Bullet[i].y - y)*(Bullet[i].y - y)) <= radius) {
					state = 0;
					score++;
					cout<<score<<endl;
					Bullet[i].set(0, 0, 0);
					time_stop = false;
					return true;
				}
			}
		}
		return false;
	}
	bool hit_player() {
		if (state == 1) {
			if (sqrt((Ship.x - side*cos(Ship.theta) - x)*(Ship.x - side*cos(Ship.theta) - x) + (Ship.y - side*sin(Ship.theta) - y)*(Ship.y - side*sin(Ship.theta) - y)) <= radius) {
				al_rest(2.0);
				return true;
			}
			if (sqrt((Ship.x - Ship.r*cos(Ship.right) - x)*(Ship.x - Ship.r*cos(Ship.right) - x) + (Ship.y - Ship.r*sin(Ship.right) - y)*(Ship.y - Ship.r*sin(Ship.right) - y)) <= radius) {
				al_rest(2.0);
				return true;
			}
			if (sqrt((Ship.x - Ship.r*cos(Ship.left) - x)*(Ship.x - Ship.r*cos(Ship.left) - x) + (Ship.y - Ship.r*sin(Ship.left) - y)*(Ship.y - Ship.r*sin(Ship.left) - y)) <= radius) {
				al_rest(2.0);
				return true;
			}
		}
		return false;
	}
}Asteroid;

void set_initial() {
	score = 0;
	Ship.x = 320; Ship.y = 240;
	Ship.angle = 90;
	for (int i = 0; i < bullet_limit; i++) {
		Bullet[i].state = 0;
		Bullet[i].set(0, 0, 0);
	}
}

void background(char color = 'w') {
	for (int i = 0; i < star_number; i++) {
		switch (color) {
			case 'w': {al_draw_pixel(sx[i], sy[i], al_map_rgb(255, 255, 255)); break;}
			case 'b': {al_draw_pixel(sx[i], sy[i], al_map_rgb(0, 0, 0)); break;}
			case 'y': {al_draw_pixel(sx[i], sy[i], al_map_rgb(255, 255, 0)); break;}
		}
	}
}

void star_generate() {
	for (int i = 0; i < star_number; i++) {
		sx[i] = rand()%640;
		sy[i] = rand()%480;
	}
}

void star_move() {
	for (int i = 0; i < star_number; i++) {
		float d = sqrt((Ship.x - sx[i])*(Ship.x - sx[i]) + (Ship.y - sy[i])*(Ship.y - sy[i]));
		for (int j = 0; j < bullet_limit; j++) {
			if (Bullet[j].state == 1) {
				float bd = sqrt((Bullet[j].x - sx[i])*(Bullet[j].x - sx[i]) + (Bullet[j].y - sy[i])*(Bullet[j].y - sy[i]));
				if (bd <= Asteroid.radius/10) {
					float star_btheta = atan2((Bullet[j].y - sy[i]),(Bullet[j].x - sx[i]));
					sx[i] -= ((Asteroid.radius/10 - bd)*cos(star_btheta));
					sy[i] -= ((Asteroid.radius/10 - bd)*sin(star_btheta));
				}
			}
		}
		if (d <= 3*Asteroid.radius/4) {
			float star_theta = atan2((Ship.y - sy[i]),(Ship.x - sx[i]));
			sx[i] -= ((3*Asteroid.radius/4 - d)*cos(star_theta)/50);
			sy[i] -= ((3*Asteroid.radius/4 - d)*sin(star_theta)/50);
		}
		if (sx[i] <= 0 || sx[i] >= 640 || sy[i] <= 0 || sy[i] >= 480) {
			sx[i] = rand()%640;
			sy[i] = rand()%480;
		}
	}
}

void display() {
	al_clear_to_color(al_map_rgb(0, 0, 0));
	background();
	Ship.draw();
	Asteroid.draw();
	al_draw_textf(score_font, al_map_rgb(255, 255, 0), 620, 480 - font_size/2, ALLEGRO_ALIGN_CENTRE, "%d", score);
	for (int i = 0; i < bullet_limit ; i++)
		Bullet[i].draw();
	al_flip_display();
}

void menu_display(ALLEGRO_FONT *fnt) {
	al_clear_to_color(al_map_rgb(0, 0, 0));
	God_Mode = true;
	background();
	Ship.draw();
	Asteroid.draw();
	al_draw_text(fnt, al_map_rgb(255, 255, 255), 320, 240 - 3*font_size/2, ALLEGRO_ALIGN_CENTRE, "ASTEROID BREAK");
	fnt = al_load_ttf_font("Fonts/orbitron-light.ttf", font_size/2, 0);
	al_draw_text(fnt, al_map_rgb(255, 255, 0), 320, 240 + 120 - 3*font_size/2, ALLEGRO_ALIGN_CENTRE, "START");
	al_draw_text(fnt, al_map_rgb(255, 255, 0), 320, 240 + 60 + font_size/2, ALLEGRO_ALIGN_CENTRE, "QUIT");

	if (ptr == 1) {
		al_draw_rectangle(320 - 5*font_size/4 - thickness, 240 + 120 - 3*font_size/2, 320 + 5*font_size/4 + thickness, 240 + 90 - font_size + 3*side/2, al_map_rgb(255, 255,0), 2*thickness);
	}
	else if (ptr == 0) {
		al_draw_rectangle(320 - font_size + thickness, 240 + 60 + font_size/2, 320 + font_size - thickness, 240 + 20 + 3*font_size/2 - side/2, al_map_rgb(255, 255,0), 2*thickness);
	}

	al_flip_display();
}

void bullet_shoot() {
	for (int i = 0; i < bullet_limit; i++) {
		if (Bullet[i].state == 0) {
			Bullet[i].set(Ship.x - side*cos(Ship.theta), Ship.y - side*sin(Ship.theta), Ship.theta, 1);
			break;
		}
	}
}

bool keyboard(ALLEGRO_KEYBOARD_STATE &key) {
	al_get_keyboard_state(&key);
	if (al_key_down(&key,ALLEGRO_KEY_ESCAPE)) return true;
	if (paused == 0) {
		if (al_key_down(&key,ALLEGRO_KEY_LEFT) || al_key_down(&key,ALLEGRO_KEY_PAD_4)) Ship.angle -= Ship.angle_speed;
		if (al_key_down(&key,ALLEGRO_KEY_RIGHT) || al_key_down(&key,ALLEGRO_KEY_PAD_6)) Ship.angle += Ship.angle_speed;
		if (al_key_down(&key,ALLEGRO_KEY_UP) || al_key_down(&key,ALLEGRO_KEY_PAD_8)) Ship.move(-1);
		if (al_key_down(&key,ALLEGRO_KEY_DOWN) || al_key_down(&key,ALLEGRO_KEY_PAD_2)) Ship.move(+1);
		if (al_key_down(&key,ALLEGRO_KEY_SPACE) || al_key_down(&key,ALLEGRO_KEY_PAD_5)) times++;
		if (!al_key_down(&key,ALLEGRO_KEY_SPACE) && !al_key_down(&key,ALLEGRO_KEY_PAD_5)) {timesl = times; times = 0;}
		if (timesl <= 15 && timesl > 0) {bullet_shoot(); timesl = 0;}
		if (al_key_down(&key,ALLEGRO_KEY_G)) timeg += 1;
		if (!al_key_down(&key,ALLEGRO_KEY_G)) {timegl = timeg; timeg = 0;}
		if (timegl <= 15 && timegl > 0) {Asteroid.generate();}
		if (al_key_down(&key,ALLEGRO_KEY_S)) timeS += 1;
		if (!al_key_down(&key,ALLEGRO_KEY_S)) {timeSl = timeS; timeS = 0;}
		if (timeSl <= 15 && timeSl > 0) {score++;}
	}
	if (al_key_down(&key,ALLEGRO_KEY_K)) timegod += 1;
	if (!al_key_down(&key,ALLEGRO_KEY_K)) {timegodl = timegod; timegod = 0;}
	if (timegodl <= 15 && timegodl > 0) { if(God_Mode) God_Mode = false; else God_Mode = true;}
	if (al_key_down(&key,ALLEGRO_KEY_P)) {timep += 1;}
	if (!al_key_down(&key,ALLEGRO_KEY_P)) {timepl = timep; timep = 0;}
	if (timepl <= 99999 && timepl > 0) {if (paused == 0) paused = 1; else if (paused == 1) paused = 0; timepl = 0;}
	if (al_key_down(&key,ALLEGRO_KEY_T)) {timet += 1;}
	if (!al_key_down(&key,ALLEGRO_KEY_T)) {timetl = timet; timet = 0;}
	if (timetl <= 15 && timetl > 0) {if (time_stop) time_stop = false; else time_stop = true; timetl = 0;}
	return false;
}

void retry_menu(ALLEGRO_FONT *fnt) {
	al_clear_to_color(al_map_rgb(0, 0, 0));
	
	al_draw_text(fnt, al_map_rgb(255, 255, 0), 320, 240 - 3*font_size/2, ALLEGRO_ALIGN_CENTRE, "RETRY");
	al_draw_text(fnt, al_map_rgb(255, 255, 0), 320, 240 + font_size/2, ALLEGRO_ALIGN_CENTRE, "QUIT");
	if (ptr == 1) {
		al_draw_rectangle(320 - 7*font_size/2 - thickness, 240 - 3*font_size/2, 320 + 7*font_size/2 + thickness, 240 - font_size + 3*side/2, al_map_rgb(255, 255,0), 2*thickness);
	}
	else if (ptr == 0) {
		al_draw_rectangle(320 - 3*font_size/2 + thickness, 240 + font_size/2, 320 + 3*font_size/2 - thickness, 240 + 3*font_size/2 - side/2, al_map_rgb(255, 255,0), 2*thickness);
	}
	al_flip_display();
}

int other_keyboard(ALLEGRO_KEYBOARD_STATE &key) {
	al_get_keyboard_state(&key);
	if (al_key_down(&key,ALLEGRO_KEY_UP) && ptr == 0) ptr += 1;
	if (al_key_down(&key,ALLEGRO_KEY_DOWN) && ptr == 1) ptr -= 1;
	if (al_key_down(&key,ALLEGRO_KEY_ENTER) || al_key_down(&key,ALLEGRO_KEY_PAD_ENTER)) if (ptr == 1) return 0; else if (ptr == 0) return 1;
	return -1;
}

int main() {
	bool quit = false, hit = false, if_hit = false;

	srand(time(0));

	ALLEGRO_DISPLAY *screen = NULL;
	ALLEGRO_KEYBOARD_STATE keystate;
	ALLEGRO_FONT *font = NULL, *menu_font = NULL, *retry_font = NULL;

	if(!al_init()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to initialize Allegro", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	screen = al_create_display(640, 480);

	if(!screen) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to Create Display", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	if(!al_init_primitives_addon()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to initialize Primitives", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	
	if(!al_install_keyboard()) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to install kayboard", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	al_init_font_addon();
	al_init_ttf_addon();

	font = al_load_ttf_font("Fonts/orbitron-light.ttf",3*font_size/2,0 );
	score_font = al_load_ttf_font("Fonts/orbitron-light.ttf",font_size/3,0 );
	retry_font = al_load_ttf_font("Fonts/orbitron-light.ttf", font_size, 0);
	menu_font = al_load_ttf_font("Fonts/orbitron-light.ttf", font_size*2/3, 0);
	
	if(!font) {
		al_show_native_message_box(screen, "Allegro", "Error", "Failed to Load Font", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}

	while (!quit) {
		short int in = other_keyboard(keystate);
		if (in == 0) break;
		else if (in == 1) {quit = true; return 0;}
		if(Asteroid.state == 0) Asteroid.generate();
		Asteroid.move();
		
		menu_display(menu_font);
	}

Start:

	star_generate();

	if (!quit) {
		Asteroid.generate();
		display();
		al_rest(1.0);
		God_Mode = false;
	}

	while (!quit && !hit) {
		quit = keyboard(keystate);
		if (paused == 0) for (int i = 0; i < bullet_limit; i++) Bullet[i].move();
		if (Asteroid.state == 0) Asteroid.generate();
		if (paused == 0) if (!time_stop) Asteroid.move();
		star_move();
		if_hit = Asteroid.hit();
		if (!God_Mode) hit = Asteroid.hit_player();
		if (score > 0 && score%5 == 0 && if_hit) {Ship.angle_speed += 0.5; Ship.speed += 0.5; Asteroid.speed += 0.5; for (int j = 0; j < bullet_limit; j++) Bullet[j].speed += 0.5;}

		display();
	}
	
	while(!quit) {
		short int retry = other_keyboard(keystate);
		if (retry == 1) {quit = true; return 0;} 
		else if (retry == 0) {
			hit = false;
			set_initial();
			goto Start;
		}
		
		retry_menu(retry_font);
	}
	
	cout<<"END"<<endl;
	al_destroy_display(screen);
}
