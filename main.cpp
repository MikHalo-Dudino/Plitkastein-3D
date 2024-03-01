#include <SFML\Graphics.hpp>
#include <vector>
#include <iostream>
using namespace sf;
#define WIN_WEIGHT 1280 + 400
#define WIN_EIGHT 720
#define WALL_SIZE 20
#define P_SIZE 10
#define pi 3.1415
#define WALL_COLOR Color::Magenta

enum Direction
{
	Forward, Left, Back, Right
};

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

	void move(Direction direction, int distance)
	{
		/*float distance = 5;*/
		float dx, dy;

		switch (direction)
		{
		case Direction::Forward:
			dx = distance * cos(angle);
			dy = distance * sin(angle);
			break;
		case Direction::Left:
			dx = distance * sin(angle);
			dy = -distance* cos(angle);
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

	void setAngle(float new_angle)
	{
		angle = new_angle;
	}

	void rotate(float d_angle)
	{
		angle += d_angle;
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
		for (; c < 500; c += 0.5) 
		{
			x = coord.first + c * cos(angle);
			y = coord.second + c * sin(angle);
			if (map[round(y / WALL_SIZE)][round(x / WALL_SIZE)] != ' ') break;
			
		} 
		//Vertex ray[] = {Vector2f(coord.first , coord.second), Vector2f(x, y)};
		//window.draw(ray, 2, Lines);
		return pow(pow((x - coord.first), 2) + pow((y - coord.second), 2), 0.5);
	}

	RectangleShape distToWall(int dist, int index)
	{
		float height;
		int alpha;
		if (dist < 15) 
		{
			height = WIN_EIGHT;
		
		}
		else
		{
			height = 100 * WIN_EIGHT / dist;
		}
		RectangleShape object(Vector2f(1, height));
		object.setFillColor(Color(255, 255, 255, height/8));
		object.setPosition(index, WIN_EIGHT / 2 - height / 2);
		return object;
	}

	void drawFov(RenderWindow &window, RenderWindow& window_game, maptype map)
	{
		int n = 1280;
		int index = 0;
		for (float i = -pi/4; i < pi/4; i += pi/(2 * n))
		{
			float dist = drawRay(window, map, angle + i);
			RectangleShape line = distToWall(dist, index);
			window_game.draw(line);
			index++;
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



void key_pressed(sf::Event& event, Player &player) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		player.move(Forward, 5);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		player.move(Left, 5);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		player.move(Back, 5);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		player.move(Right, 5);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		player.rotate(-0.05);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		player.rotate(0.05);
	}
}

void mouse_move(sf::Event& event, Player& player, float& mouse_x)
{
	float dx = event.mouseMove.x - mouse_x;
	player.rotate(dx / (500 * pi));
	
}


int main()
{
	std::vector<std::string> map;
	map.push_back("11111111111111111111");
	map.push_back("1            1     1");
	map.push_back("11           1     1");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("1            1     1");
	map.push_back("11   1111111111  11");
	map.push_back("1    1             1");
	map.push_back("1    1             1");
	map.push_back("1             1    1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1       111        1");
	map.push_back("1                  1");
	map.push_back("1                  1");
	map.push_back("1        11111     1");
	map.push_back("1                  1");
	map.push_back("11111111111111111111");
	
	RenderWindow window(VideoMode(400, 400), "minimap");
	RenderWindow window_game(VideoMode( 1280, 720), "game");
	Player player(100, 60, 1);

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

			case sf::Event::MouseButtonPressed: // нажата кнопка мыши
				break;

			default:
				break;
			}
		}
		window_game.clear();
		window.clear();
	}
}
