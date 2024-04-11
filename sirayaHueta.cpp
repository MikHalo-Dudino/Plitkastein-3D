#include <iostream> 
#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#define люцхвеяйне_еаюмне_вхякн_декхрекэ_бпелемх 200
#define WALL_SIZE 20
#define WALL_COLOR sf::Color::Magenta
#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define MIN_WALL_HEIGHT 100
#define MAP_SIZE 400
#define MAX_DIST 500
#define MIN_DIST 9
#define RAY_STEP 0.3
#define FOV_RANGE PI/2
#define MOVE_DIST 5
#define ROTATE_ANGLE 0.05
#define PLAYER_SPEED 0.05
#define PI 3.141592653589793116f

//camera settings
#define CEILLING_COLOR sf::Color(128, 128, 128, 200)
#define FLOOR_COLOR sf::Color(128, 128, 128, 200)
#define MOUSE_SENSIVITY 3
#define MOUSE_DEAD_ZONE 50
//player settings
#define MAX_PLAYER_HP 100;
//other


typedef std::vector<std::string> maptype;
using namespace sf;
enum DIR
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};
enum Cell
{
	Empty,
	Wall
};

float degToRad(float _degrees)
{
	return _degrees * PI / 180.f;
}
float get_degrees(float i_degrees)
{
	return static_cast<float>(fmod(360 + fmod(i_degrees, 360), 360));
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
void drawMap(sf::RenderWindow& win, maptype map)
{
	sf::RectangleShape wall;
	wall.setFillColor(WALL_COLOR);
	wall.setOrigin(WALL_SIZE / 2, WALL_SIZE / 2);
	wall.setSize(sf::Vector2f(WALL_SIZE, WALL_SIZE));
	for (int i = 0; i < map.size(); i++)
		for (int j = 0; j < map[0].size(); j++)
		{
			if (map[i][j] != ' ')
			{
				wall.setPosition(sf::Vector2f(j * WALL_SIZE, i * WALL_SIZE));
				win.draw(wall);
			}
		}
}
class Movable
{
protected:
	std::pair<float, float> coord;
	float speed = 0;
	DIR dir = NONE;
	float direction;
public:
	Movable(float x, float y, float _direction)
	{
		coord = { x, y };
		direction = _direction;
	}
	float getDirection()
	{
		return direction;
	}
	void setDirection(float _dir)
	{
		direction = _dir;
	}
	std::pair<float, float> getCoord()
	{
		return coord;
	}
	void setCoord(float x, float y)
	{
		coord.first = x;
		coord.second = y;
	}
	void rotate(float d_angle, float time)
	{
		direction += d_angle;
	}
	void rotateWithKeys()
	{
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			direction += ROTATE_ANGLE;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			direction -= ROTATE_ANGLE;
		}
		direction = fmod(360 + fmod(direction, 360), 360);
	}
	void mouseMoved(sf::RenderWindow& window, sf::Event& event, float time)
	{
		sf::Vector2i v2i = (sf::Vector2i(WIN_HEIGHT, WIN_HEIGHT));
		sf::Mouse::setPosition(v2i);
		if (sf::Event::MouseMoved)
		{
			if ((event.mouseMove.x < sf::Mouse::getPosition(window).x) && (abs(event.mouseMove.x - sf::Mouse::getPosition(window).x) > MOUSE_DEAD_ZONE)) // second expression used to
			{																														// there isnt accidental rotating when
				rotate(-ROTATE_ANGLE * MOUSE_SENSIVITY, time);			//something needs to be done with second expression mb				// mouse goes on y axis
			}
			if ((event.mouseMove.x > sf::Mouse::getPosition(window).x) && (abs(event.mouseMove.x - sf::Mouse::getPosition(window).x) > MOUSE_DEAD_ZONE))
			{
				rotate(ROTATE_ANGLE * MOUSE_SENSIVITY, time);
			}
		}
	}
	void movement()
	{
		if (Keyboard::isKeyPressed(Keyboard::A)) {
			dir = LEFT; speed = PLAYER_SPEED;
		}
		if (Keyboard::isKeyPressed(Keyboard::D)) {
			dir = RIGHT; speed = PLAYER_SPEED;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
			dir = UP; speed = PLAYER_SPEED;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) {
			dir = DOWN; speed = PLAYER_SPEED;
		}
	}
	float getDist(maptype map, float angle)
	{
		float c = 0;
		float x, y;		//end of ray coords
		for (; c < MAX_DIST; c += RAY_STEP) {
			x = coord.first + c * cos(angle);
			y = coord.second + c * sin(angle);
			if (map[round(y / WALL_SIZE)][round(x / WALL_SIZE)] != ' ') break;
		}
		return pow(pow((x - coord.first), 2) + pow((y - coord.second), 2), 0.5);
	}
};
class Player : public Movable
{
private:
	Sprite sprite;
	Texture texture;
public:
	Player(float x, float y, float _dir) : Movable(x, y, _dir)
	{ 
		coord.first = x; coord.second = y;
		direction = 0;
		texture.loadFromFile("MapPlayer16.png");
		sprite.setTexture(texture);
	}
	~Player()
	{

	}
	void drawPlr(RenderWindow& window)
	{
		float frame_by_angle = 360.f / (texture.getSize().x / texture.getSize().y);
		float new_dir = fmod(360 + fmod(direction + 0.5f * frame_by_angle, 360), 360);
		sprite.setPosition(coord.first, coord.second);
		sprite.setTextureRect(IntRect(texture.getSize().y * floor(new_dir / frame_by_angle), 0, texture.getSize().y, texture.getSize().y));
		window.draw(sprite);
	}
	void update(float time, maptype map, sf::RenderWindow& window, sf::Event& event)
	{
		bool flag = true;
		movement();
		rotateWithKeys();
		//mouseMoved(window, event, time);
		float dx = 0;
		float dy = 0;
		direction = get_degrees(direction);
		switch (dir)
		{
		case RIGHT:
			dx = speed * cos(degToRad(get_degrees(direction - 90)));
			dy = -speed * sin(degToRad(get_degrees(direction - 90)));
			break;
		case LEFT:
			dx = speed * cos(degToRad(get_degrees(90 + direction)));
			dy = -speed * sin(degToRad(get_degrees(90 + direction)));
			break;
		case DOWN:
			dx = -speed * cos(degToRad(direction));
			dy = speed * sin(degToRad(direction));
			break;
		case UP:
			dx = speed * cos(degToRad(direction));
			dy = -speed * sin(degToRad(direction));
			break;
		}

		coord.first += dx * time;
		coord.second += dy * time;
		int n = 16;
		int index = 0;
		for (float i = -PI; i < PI; i += 2 * PI / n)
		{

			float dist = getDist(map, direction + i);

			if (dist < MIN_DIST)
			{

				flag = false;
				coord.first -= dx * time;
				coord.second -= dy * time;
				break;
			}

		}
		speed = 0;
		sprite.setPosition(coord.first, coord.second);
	}

};
class Camera : public Movable
{
private:
	sf::VertexArray ceilling = ceillingDraw();
	sf::VertexArray floor = floorDraw();
public:
	Camera(float x, float y, float _dir) : Movable(x, y, _dir)
	{
		coord.first = x; coord.second = y;
		direction = 0;
	}
	sf::RectangleShape distToWall(float dist, int index)
	{
		float wall_height;
		float alpha;
		if (dist < MIN_DIST)
		{
			wall_height = MAX_DIST * MIN_WALL_HEIGHT / MIN_DIST;
			alpha = 255;
		}
		else
		{
			wall_height = MAX_DIST * MIN_WALL_HEIGHT / dist;
			alpha = 5600 / (dist + 13);
		}

		sf::RectangleShape wall_object(sf::Vector2f(1, wall_height));
		wall_object.setFillColor(sf::Color(alpha, alpha, alpha));
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
			float dist = getDist(map, getDirection() + i);
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
	void update(float time, maptype map, sf::RenderWindow& window, sf::Event& event)
	{
		bool flag = true;
		movement();
		rotateWithKeys();
		mouseMoved(window, event, time);
		float dx = 0;
		float dy = 0;
		direction = get_degrees(direction);
		switch (dir)
		{
		case RIGHT:
			dx = speed * cos(degToRad(get_degrees(direction - 90)));
			dy = -speed * sin(degToRad(get_degrees(direction - 90)));
			break;
		case LEFT:
			dx = speed * cos(degToRad(get_degrees(90 + direction)));
			dy = -speed * sin(degToRad(get_degrees(90 + direction)));
			break;
		case DOWN:
			dx = -speed * cos(degToRad(direction));
			dy = speed * sin(degToRad(direction));
			break;
		case UP:
			dx = speed * cos(degToRad(direction));
			dy = -speed * sin(degToRad(direction));
			break;
		}

		coord.first += dx * time;
		coord.second += dy * time;
		int n = 16;
		int index = 0;
		for (float i = -PI; i < PI; i += 2 * PI / n)
		{

			float dist = getDist(map, direction + i);

			if (dist < MIN_DIST)
			{

				flag = false;
				coord.first -= dx * time;
				coord.second -= dy * time;
				break;
			}

		}
		speed = 0;
	}
};
int main()
{
	RenderWindow window(sf::VideoMode(640, 420), "Plitkastein map");
	Clock clock;
	maptype map = loadMap("map.txt");
	Player p(100, 100, PI);

	sf::RenderWindow window_game(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "game");
	Camera camera(100, 100, PI);

	while (window.isOpen())
	{
	
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / люцхвеяйне_еаюмне_вхякн_декхрекэ_бпелемх;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		p.update(time, map, window, event);
		camera.update(time, map, window_game, event);
		window.clear();
		p.drawPlr(window);
		drawMap(window, map);
		window.display();

		camera.drawFov(window_game, map);
		window_game.display();

		std::cout << p.getCoord().first << " " << p.getCoord().second << "\n";
		
	}

	return 0;
}









