#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>

using std::cout; using std::endl;
using namespace sf;

const int M = 20; //height of game field
const int N = 10; //width of game field
int field[M][N] = { 0 }; //game field

const int TILE_SIZE = 18;

//teramino types
int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // S
	3,5,4,6, // Z
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

//contain coordinates of drawing point in global space 
struct Point
{
	int x, y;
};

bool check(Point *a)
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) 
			return 0;
		else if (field[a[i].y][a[i].x])
			return 0;

	return 1;
}

int main()
{
	srand(time(0));

	Font font;
	std::string font_path = "./../TetrisGame/font/calibri.ttf";
	font.loadFromFile(font_path);
	Text pre_score_text;
	Text score_text;

	pre_score_text.setFont(font);
	score_text.setFont(font);
	pre_score_text.setString("Score: ");
	score_text.setString("0");
	pre_score_text.setCharacterSize(24);
	score_text.setCharacterSize(24);
	pre_score_text.setStyle(Text::Bold);
	score_text.setStyle(Text::Bold);
	pre_score_text.setFillColor(sf::Color(110, 88, 58));
	score_text.setFillColor(sf::Color(110, 88, 58));
	pre_score_text.move(20, 415);
	score_text.move(100, 415);

	int score = 0;
	Point a[4] = { 0 };
	Point b[4] = { 0 };
	Point state_teramino[4] = {0};
	sf::Clock clock;
	float timer = 0, delay = 0.3f;
	int n = rand() % 7;
	int dx = 0;
	bool rotate = 0;
	int color_num = 1;
	bool begin_game = true;

	RenderWindow window(VideoMode(300, 460), "The Game!");

	std::string music_path = "./../TetrisGame/music/tetris.wav";
	std::string achieve_path = "./../TetrisGame/TetrisGame/music/goal.wav";
	Music main_music;
	Music achieve_music;
	main_music.openFromFile(music_path);
	achieve_music.openFromFile(achieve_path);
	main_music.setLoop(1);
	main_music.play();

	std::string tiles_path = "./../TetrisGame/images/tiles.png";
	std::string frame_path = "./../TetrisGame/images/frame.png";
	std::string background_path = "./../TetrisGame/images/background.png";
	Texture tiles_texture, background_texture, frame_texture;
	tiles_texture.loadFromFile(tiles_path);
	frame_texture.loadFromFile(frame_path);
	background_texture.loadFromFile(background_path);
	Sprite tiles_sprite(tiles_texture), frame_sprite(frame_texture), background_sprite(background_texture);
	tiles_sprite.setTextureRect(IntRect(0, 0, TILE_SIZE, TILE_SIZE));

	Event event;

	while (window.isOpen()) //main loop
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
			if (event.type == Event::KeyPressed)
			{
				//cout << "Pressed ANYKEY"<<endl;
				if (event.key.code == Keyboard::Up) rotate = true;
				else if (event.key.code == Keyboard::Left) { dx = -1; }
				else if (event.key.code == Keyboard::Right) { dx = 1; }
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05; //speed up

		//move
		for (int i = 0; i < 4; i++) {
			b[i] = a[i];
			a[i].x += dx;
		}

		if (!check(a)) {
			for (int i = 0; i < 4; i++)
				a[i] = b[i];
		}
		

		//rotate
		if (rotate)
		{
			Point p = a[1]; //rotation center
			for (int i = 0; i < 4; i++)
			{
				int x = a[i].y - p.y; // y - y0
				int y = a[i].x - p.x; // x - x0
				a[i].x = p.x - x;
				a[i].y = p.y + y;
			}
			if (!check(a)) {
				for (int i = 0; i < 4; i++)
					a[i] = b[i];
			}
		}

		//falling
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++) 
			{
				b[i] = a[i]; 
				a[i].y += 1; 
			}
			if (!check(a))
			{
				for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = color_num;
				color_num = 1 + rand() % 7;
				int n = rand() % 7;
				for (int i = 0; i < 4; i++)
				{
					state_teramino[i].x = a[i].x = figures[n][i] % 2 + 3; // 3 - is offset 
					state_teramino[i].y = a[i].y = figures[n][i] / 2;
				}
			}
			timer = 0;
		}

		//line filled up?
		int k = M - 1;
		for (int i = M - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < N; j++)
			{
				if (field[i][j]) 
				{
					count++;
				}
				field[k][j] = field[i][j];
			}
			if (count == N)
			{
				score += 10;
				score_text.setString(std::to_string(score));
				achieve_music.stop();
				achieve_music.play();
			}
			if (count < N) k--;
		}
		//first teramino?
		if (begin_game)
		{
			begin_game = false;
			n = rand() % 7;
			for (int i = 0; i < 4; i++)
			{ 
				state_teramino[i].x = a[i].x = figures[n][i] % 2 + 3; // 3 - is offset 
				state_teramino[i].y = a[i].y = figures[n][i] / 2;
			}
		}

		dx = 0;
		rotate = 0;
		delay = 0.3;

		//draw section

		window.clear(Color::White); //crear window
		window.draw(background_sprite);

		//drawing field teramino
		for (int i = 0; i < M; i++)
			for (int j = 0; j < N; j++)
			{
				if (field[i][j] == 0) continue;
				tiles_sprite.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
				tiles_sprite.setPosition(j * 18, i * 18);
				tiles_sprite.move(28, 31);
				window.draw(tiles_sprite);
			}
		//draw state teramino
		for (int i = 0; i < 4; i++)
		{
			tiles_sprite.setTextureRect(IntRect(color_num * 18, 0, 18, 18));
			tiles_sprite.setPosition(state_teramino[i].x * TILE_SIZE, state_teramino[i].y * TILE_SIZE);
			tiles_sprite.move(185, 45);
			//sprite draw
			window.draw(tiles_sprite);
		}

		//drawing falling teramino
		for (int i = 0; i < 4; i++)
		{
			//set position of every teramino chunk
			tiles_sprite.setTextureRect(IntRect(color_num * 18, 0, 18, 18));
			tiles_sprite.setPosition(a[i].x * TILE_SIZE, a[i].y * TILE_SIZE);
			tiles_sprite.move(28, 31);
			//sprite draw
			window.draw(tiles_sprite);
		}
		window.draw(frame_sprite);
		window.draw(pre_score_text);
		window.draw(score_text);
		window.display();
	}

	return 0;
}