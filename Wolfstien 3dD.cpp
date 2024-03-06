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
#define FOV_RANGE pi/2
#define MOVE_DIST 5
#define ROTATE_ANGLE 0.05
//player settings
#define MAX_PLAYER_HP 100;
//other
#define pi 3.1415

enum Direction
{
	Forward, Left, Back, Right
};

typedef std::vector<std::string> maptype;

class GameObject
{
public:
	virtual std::pair<float, float> getSize();
	virtual std::pair<float, float> getCoord();			// WIP
	virtual sf::Color getColor();
	virtual void objectEvent();

};
class Player
{
private:
	std::pair<float, float> coord;
	int health_point;
public:
	Player(float x, float y)
	{
		coord = { x, y };
		health_point = MAX_PLAYER_HP;
	}
	std::pair<float, float> getCoord()
	{													// WIP
		return coord;
	}
	int getHP()
	{
		return health_point;
	}
	void setCoord(float x, float y)
	{
		coord = { x, y };
	}
	void setHP(int value)
	{
		health_point = value;
	}
};
class Camera
{
private:
	std::pair<float, float> coord;
	float angle;
public:
	Camera(float x, float y)
	{
		coord = { x, y };
		angle = 0;
	}

	Camera(float x, float y, float new_angle) : Camera(x, y)
	{
		angle = new_angle;
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

	void draw(sf::RenderWindow& window)
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
		for (; c < MAP_SIZE; c += 0.5) {
			x = coord.first + c * cos(angle);
			y = coord.second + c * sin(angle);
			if (map[round(y / WALL_SIZE)][round(x / WALL_SIZE)] != ' ') break;

		}
		//Vertex ray[] = {sf::Vector2f(coord.first , coord.second), sf::Vector2f(x, y)};
		//window.draw(ray, 2, Lines);
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
		wall_object.setFillColor(sf::Color(255, 255, 255, 0.14 * wall_height));
		wall_object.setOrigin(0, wall_height / 2);
		wall_object.setPosition(index, WIN_HEIGHT / 2);
		return wall_object;
	}
	sf::RectangleShape ceillingDraw(sf::RectangleShape wall_object)  // Возможно, наступает пизда производительности, но не факт, я у себя не могу на Release потестить. Мб надо фиксить
	{
		sf::RectangleShape ceiling_object;
		ceiling_object.setOrigin(wall_object.getOrigin());
		ceiling_object.setFillColor(sf::Color::Blue);
		ceiling_object.setPosition(wall_object.getPosition());
		ceiling_object.setSize(sf::Vector2f(1, -WIN_WIDTH)); 
		return ceiling_object;
	}
	sf::RectangleShape floorDraw(sf::RectangleShape wall_object, maptype map) // Возможно, наступает пизда производительности, но не факт, я у себя не могу на Release потестить. Мб надо фиксить
	{
		sf::RectangleShape floor_object;
		sf::Vector2f service_vctr;
		service_vctr.x = 0;
		service_vctr.y = 450;
		floor_object.setOrigin(-(wall_object.getOrigin()));
		floor_object.setFillColor(sf::Color::Green);
		floor_object.setPosition(wall_object.getPosition());
		floor_object.setSize(sf::Vector2f(1, WIN_WIDTH)); 
		return floor_object;
	}
	void drawFov(sf::RenderWindow& window, sf::RenderWindow& window_game, maptype map)
	{
		int n = WIN_WIDTH;
		int index = 0;
		for (float i = -FOV_RANGE / 2; i < FOV_RANGE / 2; i += FOV_RANGE / n)
		{
			float dist = getDist(map, angle + i);
			sf::RectangleShape wall_line = distToWall(dist, index);
			window_game.draw(wall_line);
			sf::RectangleShape ceilling_line = ceillingDraw(distToWall(dist, index));
			window_game.draw(ceilling_line);
			sf::RectangleShape floor_line = floorDraw(distToWall(dist, index), map);
			window_game.draw(floor_line);
			index++;
		}
	}
};
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
void key_pressed(sf::Event& event, Camera& player) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		player.move(Forward, MOVE_DIST);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		player.move(Left, MOVE_DIST);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		player.move(Back, MOVE_DIST);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		player.move(Right, MOVE_DIST);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		player.rotate(-ROTATE_ANGLE);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		player.rotate(ROTATE_ANGLE);
	}
}

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

int main()
{
	std::vector<std::string> map;
	maptype map = loadMap("map.txt");
	sf::RenderWindow window(sf::VideoMode(MAP_SIZE, MAP_SIZE), "minimap");
	sf::RenderWindow window_game(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "game");
	Camera player(100, 300, 1);

	while (window_game.isOpen()) {
		drawMap(window, map);
		player.draw(window);
		player.drawFov(window, window_game, map);
		window.display();
		window_game.display();
		sf::Event event;

		while (window_game.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed: // закрытие окна
				window_game.close();
				break;

			case sf::Event::KeyPressed: // нажата клавиша клавиатуры
				key_pressed(event, player);
				break;

			default:
				break;
			}
		}
		window_game.clear();
		window.clear();
	}
}