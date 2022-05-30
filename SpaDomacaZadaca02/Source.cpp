#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

constexpr auto WIDTH = 1000;
constexpr auto HEIGHT = 1000;

constexpr auto BOARD_WIDTH = 50;
constexpr auto BOARD_HEIGHT = BOARD_WIDTH;

constexpr auto UPDATE_TIME = 2;

constexpr auto FPS = 60;
constexpr auto TITLE = "Matej Ruzic, 1IP1";

using namespace std;
using namespace sf;


Color hsv(int hue, float sat, float val)
{
	hue %= 360;

	while (hue < 0) hue += 360;

	if (sat < 0.f) sat = 0.f;
	if (sat > 1.f) sat = 1.f;

	if (val < 0.f) val = 0.f;
	if (val > 1.f) val = 1.f;

	int h = hue / 60;

	float f = float(hue) / 60 - h;
	float p = val * (1.f - sat);
	float q = val * (1.f - sat * f);
	float t = val * (1.f - sat * (1 - f));

	switch (h)
	{
	default:
	case 0:
	case 6: return Color(val * 255, t * 255, p * 255);
	case 1: return Color(q * 255, val * 255, p * 255);
	case 2: return Color(p * 255, val * 255, t * 255);
	case 3: return Color(p * 255, q * 255, val * 255);
	case 4: return Color(t * 255, p * 255, val * 255);
	case 5: return Color(val * 255, p * 255, q * 255);
	}
}


RectangleShape makeCell(int i, int j, bool placeTile, Vector2f& cellSize)
{
	RectangleShape cell;
	cell.setPosition(i * cellSize.x, j * cellSize.y);
	cell.setSize(cellSize);
	cell.setFillColor(placeTile ? hsv((BOARD_HEIGHT * (i - j)) % 256, 100, 100) : Color::Black);

	return cell;
}


void prepareGrid(vector<vector<int>>& grid)
{
	for (int row = 0; row < BOARD_HEIGHT; row++)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			int random = floor(rand());

			grid[row][column] = (random % 4 == 0 ? 1 : 0); // Divisible by 4 to make 25% chance.
		}
	}
}


void showCells(RenderWindow& window, vector<vector<int>>& grid)
{
	Vector2f cellSize(ceil(WIDTH / BOARD_WIDTH), ceil(HEIGHT / BOARD_HEIGHT));

	for (int row = 0; row < BOARD_HEIGHT; row++)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			window.draw(makeCell(row, column, grid[row][column], cellSize));
		}
	}
}


int getNeighbor(vector<vector<int>>& grid, int row, int column)
{
	// Simulate infinite board with edge wrapping
	return grid[(row + BOARD_HEIGHT) % BOARD_HEIGHT][(column + BOARD_WIDTH) % BOARD_WIDTH];
}


int getCount(vector<vector<int>>& grid, int row, int column)
{
	int count = 0;

	vector<int> deltas{ -1, 0, 1 };

	for (int i : deltas)
	{
		for (int j : deltas)
		{
			if (i || j)
			{
				count += getNeighbor(grid, row + j, column + i);
			}
		}
	}

	return count;
}


int tick(vector<vector<int>>& grid, int row, int column)
{
	int count = getCount(grid, row, column);

	bool birth = !grid[row][column] && count == 3;
	bool survive = grid[row][column] && (count == 2 || count == 3);

	return birth || survive;
}


void updateCells(vector<vector<int>>& grid)
{
	vector<vector<int>> originalGrid = grid;

	for (int row = 0; row < BOARD_HEIGHT; row++)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			grid[row][column] = tick(originalGrid, row, column);
		}
	}
}


int main()
{
	srand(time(nullptr));

	RenderWindow window(VideoMode(WIDTH, HEIGHT), TITLE);
	Clock clock;

	window.setFramerateLimit(FPS);

	vector<vector<int>> grid;

	Vector2f cellSize(ceil(WIDTH / BOARD_WIDTH), ceil(HEIGHT / BOARD_HEIGHT));

	grid.resize(BOARD_HEIGHT, vector<int>(BOARD_WIDTH));

	prepareGrid(grid);

	while (window.isOpen())
	{
		Event event;

		Time elapsed = clock.getElapsedTime();

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear();

		showCells(window, grid);

		if (elapsed.asSeconds() >= UPDATE_TIME)
		{
			updateCells(grid);

			clock.restart();
		}

		window.display();
	}

	return 0;
}