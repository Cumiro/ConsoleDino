#include <iostream>
#include <Windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

const float FRAME_DELAY = 0.2 ;

const char RED[] = "\033[31m";
const char GREEN[] = "\033[32m";
const char YELLOW[] = "\033[33m";
const char BLUE[] = "\033[34m";
const char MAGENTA[] = "\033[35m";
const char CYAN[] = "\033[36m";
const char DEFAULT[] = "\033[0m";

char map[] =
"#####################################\n"
"#                                   #\n"
"#                                   #\n"
"#                                   #\n"
"#                                   #\n"
"#                                   #\n"
"#                                   #\n"
"#####################################\n";
const int WIDTH = 38, HEIGHT = 8;
const char player_char = '$';
const char cactus_char = ':';
const char cloud_char = '*';

std::vector<int> map_cactuses(WIDTH - 2, 0);
std::vector<std::vector<int>> clouds;
int ticks_to_new_cactus = 0, ticks_to_new_cloud = 0;
bool isRunning = true;

int pos_x = WIDTH / 2-1;
int pos_y = HEIGHT-2;
int player_jumping = false;
int score = 0;

void gotoxy(int x, int y) {
	COORD pos = { x, y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}
void add_catuses() {
	rotate(map_cactuses.begin(), map_cactuses.begin() + 1, map_cactuses.end());
	if (ticks_to_new_cactus == 0) {
		map_cactuses.back() = 1;
		ticks_to_new_cactus = 7 + rand() % (10 - 7 + 1);
	}
	else
		ticks_to_new_cactus--;
}
void add_clouds() {
	for (int i = 0; i < clouds.size(); i++) {
		clouds[i][1] -= 1;
	}
	for (int i = 0; i < clouds.size(); i++) {
		if (clouds[i][1] < 1) {
			clouds.erase(clouds.begin() + i);
		}
	}
	if (ticks_to_new_cloud == 0) {
		int cloud_y = 1 + rand() % (3 - 1 + 1); // from 1 to 3
		int cloud_size = 2 + rand() % (3 - 2 + 1); // from 2 to 3
		for (int i = 0; i < cloud_size; i++)
			clouds.push_back({ cloud_y , WIDTH + i+1 });
		ticks_to_new_cloud = 3 + rand() % (10 - 3 + 1); // from 3 to 10
	}
	else
		ticks_to_new_cloud--;
}
void renderMap() {
	map[pos_y * WIDTH + pos_x] = player_char;
	for (int i = 1; i < WIDTH-2; i++)
	{
		if (map_cactuses[i] == 1) {
			map[(HEIGHT-2) * WIDTH + i] = cactus_char;
		}
	}
	for (int i = 0; i < clouds.size(); i++)
	{
		std::vector<int> cloud = clouds[i];
		if (cloud[1] < WIDTH-2)
			map[cloud[0] * WIDTH + cloud[1]] = cloud_char;
	}
	std::cout << RED << "Score: " << score << DEFAULT << std::endl << std::endl;
	for (int i = 0; i < WIDTH*HEIGHT; i++)
	{
		switch (map[i])
		{
		case cactus_char:
			std::cout << GREEN << map[i];
			break;
		case player_char:
			std::cout << RED << map[i];
			break;
		case cloud_char:
			std::cout << CYAN << map[i];
			break;
		case ' ':
			std::cout << map[i];
			break;
		default:
			if (i >= (HEIGHT - 1) * WIDTH)
				std::cout << YELLOW << map[i];
			else
				std::cout << DEFAULT << map[i];
			break;
		}
	}
	map[pos_y * WIDTH + pos_x] = ' ';
	for (int i = 1; i < WIDTH - 2; i++)
	{
		if (map_cactuses[i] == 1)
			map[(HEIGHT - 2) * WIDTH + i] = ' ';
	}
	for (int i = 0; i < clouds.size(); i++)
	{
		std::vector<int> cloud = clouds[i];
		if (cloud[1] < WIDTH - 2)
			map[cloud[0] * WIDTH + cloud[1]] = ' ';
	}
}
void make_jump() {
	switch (player_jumping) {
	case 1:
		pos_y--;
		player_jumping = 2;
		break;
	case 2:
		pos_y--;
		player_jumping = -2;
		break;
	case -2:
		pos_y++;
		player_jumping = -1;
		break;
	case -1:
		pos_y++;
		score++;
		player_jumping = false;
		break;
	}
}
int check_end() {
	if (pos_y == HEIGHT - 2) {
		for (int i = 1; i < WIDTH - 2; i++)
		{
			if (map_cactuses[i] == 1 && i == pos_x)
				return 1;
		}
	}
	return 0;
}
int main() {
	srand(static_cast<unsigned int>(time(0)));
	while (isRunning) {
		if ((GetKeyState('W') & 0x8000) ||
			(GetKeyState(VK_SPACE) & 0x8000)) {
			if (!player_jumping)
				player_jumping = true;
		}
		Sleep(FRAME_DELAY * 1000);
		gotoxy(0, 0);
		add_catuses();
		add_clouds();
		if (player_jumping != 0)
			make_jump();
		if (check_end() == 1)
			break;
		renderMap();
	}
	renderMap();
	gotoxy(7, HEIGHT / 2 + 2);
	std::cout << DEFAULT << "You're lose! Score: " << MAGENTA << score << DEFAULT;
	gotoxy(WIDTH, HEIGHT + 3);
}