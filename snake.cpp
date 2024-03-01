#include <ctime>
#include <fcntl.h> // Add fcntl.h for file control options
#include <iostream>
#include <termios.h> // for non-blocking input on Unix-based systems
#include <unistd.h>  // for Sleep function on Unix-based systems
#include <vector>

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
std::vector<int> tailX, tailY;
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

void Setup() {
	gameOver = false;
	dir = STOP;
	x = width / 2;
	y = height / 2;
	fruitX = rand() % width;
	fruitY = rand() % height;
	score = 0;
	nTail = 0;
}

void Draw() {
	// ANSI escape codes to clear the screen
	std::cout << "\033[2J\033[H";

	for (int i = 0; i < width + 2; i++)
		std::cout << "#";
	std::cout << std::endl;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (j == 0)
				std::cout << "#";
			if (i == y && j == x)
				std::cout << "O";
			else if (i == fruitY && j == fruitX)
				std::cout << "F";
			else {
				bool print = false;
				for (int k = 0; k < nTail; k++) {
					if (tailX[k] == j && tailY[k] == i) {
						std::cout << "o";
						print = true;
					}
				}
				if (!print)
					std::cout << " ";
			}

			if (j == width - 1)
				std::cout << "#";
		}
		std::cout << std::endl;
	}

	for (int i = 0; i < width + 2; i++)
		std::cout << "#";
	std::cout << std::endl;
	std::cout << "Score:" << score << std::endl;
}

int _kbhit() {
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

void Input() {
	if (_kbhit()) {
		char input = getchar();
		switch (input) {
		case 'w':
			if (dir != DOWN)
				dir = UP;
			break;
		case 's':
			if (dir != UP)
				dir = DOWN;
			break;
		case 'a':
			if (dir != RIGHT)
				dir = LEFT;
			break;
		case 'd':
			if (dir != LEFT)
				dir = RIGHT;
			break;
		default:
			break;
		}
	}
}

void Logic() {
	if (nTail > 0) {
		int prevX = tailX[0];
		int prevY = tailY[0];
		int tempX, tempY;
		tailX[0] = x;
		tailY[0] = y;
		for (int i = 1; i < nTail; i++) {
			if (i < tailX.size() && i < tailY.size()) {
				tempX = tailX[i];
				tempY = tailY[i];
				tailX[i] = prevX;
				tailY[i] = prevY;
				prevX = tempX;
				prevY = tempY;
			} else {
				std::cout << "Error: Accessing tailX or tailY out of bounds!"
									<< std::endl;
				break;
			}
		}
	}

	switch (dir) {
	case LEFT:
		x--;
		break;
	case RIGHT:
		x++;
		break;
	case UP:
		y--;
		break;
	case DOWN:
		y++;
		break;
	default:
		break;
	}

	if (x >= width)
		x = 0;
	else if (x < 0)
		x = width - 1;

	if (y >= height)
		y = 0;
	else if (y < 0)
		y = height - 1;

	for (int i = 0; i < nTail; i++) {
		if (i < tailX.size() && i < tailY.size()) {
			if (tailX[i] == x && tailY[i] == y) {
				gameOver = true;
				std::cout << "Snake collided with itself!" << std::endl;
				break;
			}
		} else {
			std::cout << "Error: Accessing tailX or tailY out of bounds!"
								<< std::endl;
			break;
		}
	}

	// Check if the snake eats the fruit
	if (x == fruitX && y == fruitY) {
		score += 10;
		fruitX = rand() % width;
		fruitY = rand() % height;
		nTail++;
		tailX.push_back(0); // Add a new element to the tailX vector
		tailY.push_back(0); // Add a new element to the tailY vector
	}
}

int main() {
	Setup();
	while (!gameOver) {
		Draw();
		Input();
		Logic();
		usleep(100000); // Sleep for 100 milliseconds (Unix-based systems)
	}
	return 0;
}