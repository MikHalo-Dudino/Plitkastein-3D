#include <SFML\Graphics.hpp>
#include <vector>
#include <iostream>
using namespace sf;
#define WIN_WEIGHT 1280 + 400
#define WIN_EIGHT 720
#define WALL_SIZE 20
#define P_SIZE 15
#define pi 3.1415
#define WALL_COLOR Color::Magenta

typedef std::vector<std::string> maptype;
class Player
{
private:
	std::pair<float, float> coord;
	float angle;
public:
	Player(float x, float y)
	{
		coord = {x, y};
		angle = 0;
	}
	Player(float x, float y, float new_angle) : Player(x, y)
	{
		angle = new_angle;
	}

	void setCoord(std::pair<float, float> new_coord)
	{
		coord = new_coord;
	}
	void setAngle(float new_angle)
	{
		angle = new_angle;
	}
	void draw(RenderWindow& window)
	{
		CircleShape playerShape(P_SIZE);
		playerShape.setOrigin(Vector2f(P_SIZE, P_SIZE));
		playerShape.setFillColor(Color::Red);
		playerShape.setPosition(Vector2f(coord.first, coord.second));
		window.draw(playerShape);
	}
	float drawRay(RenderWindow& window, maptype map, float angle)
	{
		float c = 0;
		float x;
		float y;
		for (; c < 500; c += 0.1) 
		{
			x = coord.first + c * cos(angle);
			y = coord.second + c * sin(angle);
			if (map[round(y / WALL_SIZE)][round(x / WALL_SIZE)] != ' ') break;
			
		} 
		Vertex ray[] = {Vector2f(coord.first , coord.second), Vector2f(x, y)};
		window.draw(ray, 2, Lines);
		return pow(pow((x - coord.first), 2) + pow((y - coord.second), 2), 0.5);
	}
	RectangleShape distToWallProcessor(int dist, int index)
	{
		float height = 0;
		if (dist < 1) height = WIN_EIGHT;
		else
		{
			height = WIN_EIGHT / dist;
		}
		RectangleShape object(Vector2f(1.0, height));
		object.setOrigin(0, WIN_EIGHT / 2);
		object.setPosition(index, -height / 2);
		return object;
	}
	void drawFov(RenderWindow &window, RenderWindow& window_game, maptype map)
	{
		int n = 1280;
		int index = 0;
		for (float i = -pi/4; i < pi/4; i += pi/(2 * n))
		{
			float dist = drawRay(window, map, angle + i);
			RectangleShape line = distToWallProcessor(dist, index);
			window_game.draw(line);
			index++;
			window_game.display();
		}

	}
};

void drawMap(RenderWindow& win, maptype map)
{
	RectangleShape wall;
	wall.setFillColor(WALL_COLOR);
	wall.setOrigin(WALL_SIZE / 2, WALL_SIZE / 2);
	wall.setSize(Vector2f(WALL_SIZE, WALL_SIZE));
	for (int i = 0; i < map.size(); i++)
		for (int j = 0; j < map[0].size(); j++)
		{
			if (map[i][j] == '1')
			{
				wall.setPosition(Vector2f(j * WALL_SIZE, i * WALL_SIZE));
				win.draw(wall);
			}
		}

}

int main()
{
	std::vector<std::string> map;
	map.push_back("11111111111111111111");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("11   111111111111111");
	map.push_back("1    1             1");
	map.push_back("1    1             1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("11111111111111111111");
	
	RenderWindow win(VideoMode(400, 400), "Zalupstien");
	RenderWindow win_game(VideoMode( 1280, 720), "game");
	Player player(100, 60, 1.5);
	while (win.isOpen()) 
	{
		drawMap(win, map);
		player.draw(win);
		player.drawFov(win, win_game, map);
		win.display();
		win_game.display();
		system("pause");
		

	}
}