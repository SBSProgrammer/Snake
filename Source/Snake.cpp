#include <Windows.h> // im sorry linux users
#include <iostream>
#include <vector>
#include <thread>
#include <random>

using namespace std;

// im super, super sorry for these define macros
#define SNAKEDIR_NORTH 0
#define SNAKEDIR_EAST  1
#define SNAKEDIR_SOUTH 2
#define SNAKEDIR_WEST  3

#define SLEEP(t) this_thread::sleep_for(t)
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

class RandomGenerator
{
public:
	RandomGenerator()
	{
		m_Seed = time(NULL);
	}

	RandomGenerator(int seed)
	{
		m_Seed = seed;
	}

public:
	int RandomInt(int min, int max = 1)
	{
		uniform_int_distribution<int> Distribution(min, max);
		return Distribution(m_RandomDevice);
	}

private:
	random_device m_RandomDevice;
	int m_Seed;
};

void SetCharacterAt(int X, int Y, wchar_t Char)
{
	if (X >= 0 && X < ScreenWidth && Y >= 0 && Y < ScreenHeight) Screen[(Y * ScreenWidth) + X] = Char;
}

void ClearScreenBuffer(wchar_t Char = ' ')
{
	for (int i = 0; i < ScreenWidth * ScreenHeight; i++) Screen[i] = Char;
}

void DisplayScreenBuffer(HANDLE Console)
{
	DWORD BytesWritten = 0;
	WriteConsoleOutputCharacter(Console, Screen, ScreenWidth * ScreenHeight, { 0,0 }, &BytesWritten);
}

// TODO: find a better name for this function
int Vector2ToInt(int X, int Y)
{
	return (Y * ScreenWidth) + X;
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
	//srand(time(0));

	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	SetConsoleTitle(L"Snake - join the OLC discord btw!");

	RandomGenerator RandomGen(time(NULL)); // sorry, had to use that weird thing

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
			SLEEP(SnakeDir == SNAKEDIR_NORTH || SnakeDir == SNAKEDIR_SOUTH ? 400ms : 200ms);

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
				/*Fruit.X = rand() % ScreenWidth - 1;
				Fruit.Y = rand() % ScreenHeight - 1;*/
				Fruit.X = RandomGen.RandomInt(0, ScreenWidth);
				Fruit.Y = RandomGen.RandomInt(1, ScreenHeight);
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
			DisplayScreenBuffer(Console);
		}
		wsprintf(&Screen[Vector2ToInt(50, 13)], L"[      GAME OVER       ]");
		wsprintf(&Screen[Vector2ToInt(50, 14)], L"[PRESS SPACE TO RESTART]");
		DisplayScreenBuffer(Console);
		
		while (!GetAsyncKeyState(VK_SPACE));
		// Reset snake
		Dead = false;
		Snake = { {65, 20}, {66, 20}, {67, 20}, {68, 20} };
		SnakeDir = SNAKEDIR_WEST;
		Score = 0;
	}

	return 0;
}
