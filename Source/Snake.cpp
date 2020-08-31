#include <Windows.h> // im sorry linux users
#include <iostream>
#include <vector>
#include <thread>

using namespace std;

// im super, super sorry for these define macros
#define SNAKEDIR_NORTH 0
#define SNAKEDIR_EAST  1
#define SNAKEDIR_SOUTH 2
#define SNAKEDIR_WEST  3
// also im sorry if the code is generally cursed

int ScreenWidth = 120;
int ScreenHeight = 40;

wchar_t* Screen = new wchar_t[ScreenWidth * ScreenHeight];

struct SnakeBody // should i rename this to vector2?
{
	int X, Y;
};

struct Apple // Basically the same as SnakeBody, just under a different name for readability sake (might change this to a define macro tbh)
{
	int X, Y;
};

void SetCharacterAt(int X, int Y, wchar_t Char)
{
	if (X >= 0 && X < ScreenWidth && Y >= 0 && Y < ScreenHeight) Screen[(Y * ScreenWidth) + X] = Char;
}

void ClearScreenBuffer()
{
	for (int i = 0; i < ScreenWidth * ScreenHeight; i++) Screen[i] = ' ';
}

//void GrowSnake(list<SnakeBody>& Snake)
//{
//	Snake.push_back({ Snake.back().X, Snake.back().Y });
//}
//
//void ShrinkSnake(list<SnakeBody>& Snake)
//{
//	Snake.pop_back();
//}

int main( void )
{
	srand(time(0)); // TODO: possibly switch to uniform_int_distribution

	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	SetConsoleTitle(L"Snake - join the OLC discord btw!");
	DWORD BytesWritten = 0; // consider this junk

	vector<SnakeBody> Snake = { {65, 20}, {66, 20}, {67, 20}, {68, 20} };
	int SnakeDir = SNAKEDIR_WEST;
	int Score = 0;
	bool Dead = false;
	Apple Fruit = { 30, 20 };

	while (true)
	{
		while (!Dead)
		{
			// Clear screen buffer
			ClearScreenBuffer();

			// Timing
			this_thread::sleep_for(200ms);

			// Drawing
			wsprintf(&Screen[0], L"Score: %d", Score);

			// Collision detection
			// Snake vs World
			if (Snake.front().X <= -1 || Snake.front().X >= ScreenWidth)
			{
				Dead = true;
			}
			if (Snake.front().Y <= -1 || Snake.front().Y >= ScreenHeight - 10)
			{
				Dead = true;
			}

			// Snake vs Snake
			for (int i = 1; i < Snake.size(); i++)
			{
				if (Snake.at(i).X == Snake.front().X && Snake.at(i).Y == Snake.front().Y) Dead = true;
			}

			// Snake vs Fruit
			if (Snake.front().X == Fruit.X && Snake.front().Y == Fruit.Y)
			{
				Snake.push_back({ Snake.back().X, Snake.back().Y });
				Score++;
				Fruit.X = rand() % ScreenWidth - 1;
				Fruit.Y = rand() % ScreenHeight - 1;
			}

			// Draw snake
			for (auto S : Snake)
				SetCharacterAt(S.X, S.Y, Dead ? 'X' : 'O');
			SetCharacterAt(Snake.front().X, Snake.front().Y, Dead ? '+' : '@'); 

			//Draw apple
			SetCharacterAt(Fruit.X, Fruit.Y, '*');

			// Game logic
			// Move snake
			switch (SnakeDir)
			{
			case SNAKEDIR_NORTH:
				Snake.insert(Snake.begin(), { Snake.front().X, Snake.front().Y - 1 });
				break;

			case SNAKEDIR_EAST:
				Snake.insert(Snake.begin(), { Snake.front().X + 1, Snake.front().Y });
				break;

			case SNAKEDIR_SOUTH:
				Snake.insert(Snake.begin(), { Snake.front().X, Snake.front().Y + 1 });
				break;

			case SNAKEDIR_WEST:
				Snake.insert(Snake.begin(), { Snake.front().X - 1, Snake.front().Y });
				break;

			default:
				break;
			}
			Snake.pop_back(); // CUT HIS TAIL OFF.

			// Input
			if (GetAsyncKeyState(VK_RIGHT))
			{
				SnakeDir++;
				if (SnakeDir > SNAKEDIR_WEST) SnakeDir = SNAKEDIR_NORTH;
			}
			if (GetAsyncKeyState(VK_LEFT))
			{
				SnakeDir--;
				if (SnakeDir < SNAKEDIR_NORTH) SnakeDir = SNAKEDIR_WEST;
			}

			// debugging only
			/*if (GetAsyncKeyState(VK_INSERT)) GrowSnake(Snake);
			if (GetAsyncKeyState(VK_DELETE)) ShrinkSnake(Snake);*/


			// Render frame
			WriteConsoleOutputCharacter(Console, Screen, ScreenWidth * ScreenHeight, { 0,0 }, &BytesWritten);
		}

		/*wsprintf(&Screen[0], L"Score: %d - GAME OVER!", Score);*/
		while (!GetAsyncKeyState(VK_SPACE));
		// Reset snake
		Dead = false;
		Snake = { {65, 20}, {66, 20}, {67, 20}, {68, 20} };
		SnakeDir = SNAKEDIR_WEST;
		Score = 0;
	}

	return 0;
}
