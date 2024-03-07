#include <SFML\Graphics.hpp>
#include <vector>
#include <iostream>
#include <fstream>
//minimap settings
#define WALL_SIZE 20
#define WALL_COLOR sf::Color::Magenta
#define PLAYER_SIZE 10
#define PLAYER_COLOR sf::Color::Red
//window settings
#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define MIN_WALL_HEIGHT 100
#define MAP_SIZE 400
const int MAX_DIST = (MAP_SIZE) * 1.41;
//camera settings
#define RAY_STEP 0.3
#define FOV_RANGE pi/2
#define MOVE_DIST 5
#define ROTATE_ANGLE 0.05
#define CEILLING_COLOR sf::Color(128, 128, 128, 200)
#define FLOOR_COLOR sf::Color(128, 128, 128, 200)
//player settings
#define MAX_PLAYER_HP 100;
//other
#define pi 3.1415

typedef std::vector<std::string> maptype;

maptype loadMap(std::string filename)
{
	std::ifstream filein(filename);
	maptype map;
	while (!filein.eof())
	{
		std::string buff;
		std::getline(filein, buff);
		map.push_back(buff);
	}
	return map;
}

void drawMap(sf::RenderWindow& win, maptype map)
{
	sf::RectangleShape wall;
	wall.setFillColor(WALL_COLOR);
	wall.setOrigin(WALL_SIZE / 2, WALL_SIZE / 2);
	wall.setSize(sf::Vector2f(WALL_SIZE, WALL_SIZE));
	for (int i = 0; i < map.size(); i++)
		for (int j = 0; j < map[0].size(); j++)
		{
			if (map[i][j] == '1')
			{
				wall.setPosition(sf::Vector2f(j * WALL_SIZE, i * WALL_SIZE));
				win.draw(wall);
			}
		}
}

enum Direction
{
	Forward, Left, Back, Right
};

class GameObject
{
public:
	virtual std::pair<float, float> getSize();
	virtual std::pair<float, float> getCoord();			// WIP
	virtual sf::Color getColor();
	virtual void objectEvent();

};

class Movable
{
protected:
	std::pair<float, float> coord;
	float angle;
public:
	Movable(float x, float y, float new_angle) 
	{
		coord = { x, y };
		angle = new_angle;
	}
	std::pair<float, float> getCoord()
	{													
		return coord;
	}
	void setCoord(float x, float y)
	{
		coord = { x, y };
	}
	void move(Direction direction, int distance)
	{
		float dx, dy;
		switch (direction)
		{
		case Direction::Forward:
			dx = distance * cos(angle);
			dy = distance * sin(angle);
			break;
		case Direction::Left:
			dx = distance * sin(angle);
			dy = -distance * cos(angle);
			break;
		case Direction::Right:
			dx = -distance * sin(angle);
			dy = distance * cos(angle);
			break;
		case Direction::Back:
			dx = -distance * cos(angle);
			dy = -distance * sin(angle);
			break;
		default:
			break;
		}
		coord.first += dx;
		coord.second += dy;
	}

	void rotate(float d_angle)
	{
		angle += d_angle;
	}

	void key_pressed(sf::Event& event) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			move(Forward, MOVE_DIST);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			move(Left, MOVE_DIST);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			move(Back, MOVE_DIST);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			move(Right, MOVE_DIST);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			rotate(-ROTATE_ANGLE);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			rotate(ROTATE_ANGLE);
		}
	}
};

class Player : private Movable 
{
private:
	int health_point;
public:
	Player(float x, float y, float angle) : Movable(x, y, angle)	// WIP
	{
		health_point = MAX_PLAYER_HP;
	}
	int getHP()
	{
		return health_point;
	}
	void setHP(int value)
	{
		health_point = value;
	}
};

class Camera : public Movable
{
private:
	sf::VertexArray ceilling = ceillingDraw();
	sf::VertexArray floor = floorDraw();
public:
	Camera(float x, float y, float angle) : Movable(x, y, angle) {};

	void drawMapPlayer(sf::RenderWindow& window)
	{
		sf::CircleShape playerShape(PLAYER_SIZE / 2);
		playerShape.setOrigin(sf::Vector2f(PLAYER_SIZE / 2, PLAYER_SIZE / 2));
		playerShape.setFillColor(PLAYER_COLOR);
		playerShape.setPosition(sf::Vector2f(coord.first, coord.second));
		window.draw(playerShape);
	}

	float getDist(maptype map, float angle)
	{
		float c = 0;
		float x, y;		//end of ray coords
		for (; c < MAP_SIZE; c += RAY_STEP) {
			x = coord.first + c * cos(angle);
			y = coord.second + c * sin(angle);
			if (map[round(y / WALL_SIZE)][round(x / WALL_SIZE)] != ' ') break;
		}
		return pow(pow((x - coord.first), 2) + pow((y - coord.second), 2), 0.5);
	}

	sf::RectangleShape distToWall(int dist, int index)
	{
		float wall_height;
		if (dist < 22)
		{
			wall_height = WIN_HEIGHT;
		}
		else
		{
			wall_height = MIN_WALL_HEIGHT * MAP_SIZE / dist;
		}
		sf::RectangleShape wall_object(sf::Vector2f(1, wall_height));
		wall_object.setFillColor(sf::Color(0.14 * wall_height, 0.14 * wall_height, 0.14 * wall_height));
		wall_object.setOrigin(0, wall_height / 2);
		wall_object.setPosition(index, WIN_HEIGHT / 2);

		return wall_object;
	}

	void drawFov(sf::RenderWindow& window_game, maptype map)
	{
		int n = WIN_WIDTH;
		int index = 0;
		window_game.draw(ceilling);
		window_game.draw(floor);
		for (float i = -FOV_RANGE / 2; i < FOV_RANGE / 2; i += FOV_RANGE / n)
		{
			float dist = getDist(map, angle + i);
			window_game.draw(distToWall(dist, index));
			index++;
		}
	}

	sf::VertexArray ceillingDraw()
	{
		sf::VertexArray ceilling(sf::Quads, 4);
		ceilling[0].position = sf::Vector2f(0, 0);
		ceilling[1].position = sf::Vector2f(WIN_WIDTH, 0);
		ceilling[2].position = sf::Vector2f(WIN_WIDTH, WIN_HEIGHT / 2);
		ceilling[3].position = sf::Vector2f(0, WIN_HEIGHT / 2);

		ceilling[0].color = CEILLING_COLOR;
		ceilling[1].color = CEILLING_COLOR;
		ceilling[2].color = sf::Color::Black;;
		ceilling[3].color = sf::Color::Black;;

		return ceilling;
	}

	sf::VertexArray floorDraw()
	{
		sf::VertexArray floor(sf::Quads, 4);
		floor[0].position = sf::Vector2f(0, WIN_HEIGHT / 2);
		floor[1].position = sf::Vector2f(WIN_WIDTH, WIN_HEIGHT / 2);
		floor[2].position = sf::Vector2f(WIN_WIDTH, WIN_HEIGHT);
		floor[3].position = sf::Vector2f(0, WIN_HEIGHT);

		floor[0].color = sf::Color::Black;
		floor[1].color = sf::Color::Black;
		floor[2].color = FLOOR_COLOR;
		floor[3].color = FLOOR_COLOR;

		return floor;
	}
};

int main()
{
	maptype map = loadMap("map.txt");
	sf::RenderWindow window_map(sf::VideoMode(MAP_SIZE, MAP_SIZE), "minimap");
	sf::RenderWindow window_game(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "game");
	Camera camera(300, 300, pi);

	while (window_game.isOpen()) {
		camera.drawFov(window_game, map);
		window_game.display();

		camera.drawMapPlayer(window_map);
		drawMap(window_map, map);
		window_map.display();
		
		sf::Event event;

		while (window_game.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window_game.close();
				break;

			case sf::Event::KeyPressed: 
				camera.key_pressed(event);
				break;

			default:
				break;
			}
		}
		window_game.clear();
		window_map.clear();
	}
}
