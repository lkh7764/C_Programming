#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>


// problem1
void findLongestName(const char* names) {
	int size = strlen(names);

	int namePos = 0;
	int nameNum = 0;
	int nameLen = 0;

	char* longestName = nullptr;
	int longestPos = 0;
	int longestLen = 0;

	for (int i = 0; i < size; i++) {
		if (names[i] == '#') {
			nameNum++;
			char* name = new char[nameLen + 1];
			for (int j = 0; j < nameLen; j++)
				name[j] = names[namePos + j];
			name[nameLen] = '\0';
			printf("%d: %s\n", nameNum, name);

			if (nameLen > longestLen) {
				if (longestName != nullptr)
					delete[] longestName;

				longestName = new char[nameLen + 1];
				strcpy(longestName, name);
				longestLen = nameLen;
				longestPos = namePos;
			}

			namePos = i + 1;
			nameLen = 0;
			delete[] name;
			name = nullptr;

			continue;
		}
		nameLen++;
	}

	printf("The longest name is %s\n", longestName);

	delete[] longestName;
}

// problem1 ����
void problem1() {
	char test[100] = "Mozart PPPPPPPPPPP#Elvis Presley#Jim Carry#SchubertQ!@#%^&@!#!#Dominggo#";
	findLongestName(test);
}


// problem2
struct GameObject;
struct Canvas {
	char* frameBuffer;
	int size;

	Canvas(int size) : size(size), frameBuffer(new char[size + 1]) {
		clear();
	}

	void clear() {
		for (int i = 0; i < size; i++)
			frameBuffer[i] = ' ';
		frameBuffer[size] = '\0';
	}

	int getSize() { return size; }

	void draw(const GameObject&);

	void render() const {
		printf("%s\r", frameBuffer);
	}

	~Canvas() {
		delete[] frameBuffer;
		frameBuffer = nullptr;
		size = 0;
	}
};

struct GameObject {
	char* shape;
	int pos;

	GameObject(const char* shape, int pos) :shape(new char[strlen(shape) + 1]), pos(pos) {
		if (this->shape != nullptr) strcpy(this->shape, shape);
	}

	const char* getShape() const { return shape; }
	int getShapeSize() const { return strlen(shape); }
	void setShape(const char* shape) { strcpy(this->shape, shape); }

	int getPos() const { return pos; }
	void setPos(int pos) { this->pos = pos; }

	void draw(Canvas& canvas) const { canvas.draw(*this); }

	~GameObject() {
		delete[] shape;
		shape = nullptr;
		pos = 0;
	}
};

void Canvas::draw(const GameObject& obj) {
	for (int i = 0; i < strlen(obj.shape); i++) {
		if (obj.pos + i < 0) continue;
		if (obj.pos + i > size - 1) continue;

		frameBuffer[obj.pos + i] = obj.shape[i];
	}
}

struct Mirror : public GameObject {

	Mirror(int pos) : GameObject("|", pos) {}

	bool isColliding(const int pos) {
		if (pos >= this->pos && pos <= (this->pos + strlen(shape))) return true;
		return false;
	}

	void draw(Canvas& canvas) {
		canvas.draw(*this);
	}

	~Mirror() {}
};

struct Bullet : public GameObject {
	int direction; //0: LtoR, 1: RtoL
	int hp;

	Bullet(const char* shape, int pos, int direciton) : GameObject(shape, pos), direction(direction), hp(10) {}

	void move(Canvas& canvas) {
		int pos = getPos();
		if (direction == 0) setPos(pos + 1);
		else setPos(pos - 1);

		if (pos + 1 >= canvas.getSize() - 1)
			direction = 1;
		else if (pos - 1 <= 0)
			direction = 0;
	}

	void isColliding() {
		if (direction == 0) {
			direction = 1;
			setShape("<");
		}
		else {
			direction = 0;
			setShape(">");
		}
		damaged();
	}

	void damaged() { hp -= 4; }

	int getHP() { return hp; }

	void draw(Canvas& canvas) { canvas.draw(*this); }

	void update(Mirror& m1, Mirror& m2) {
		move();
		if (m1.isColliding(pos) == true || m2.isColliding(pos) == true) isColliding();
	}

	~Bullet() {
		direction = 0;
		hp = 0;
	}
};

struct Player : public GameObject {
	int frameCount;
	Bullet* bullets[100];

	Player(const char* shape, int pos) : GameObject(shape, pos), frameCount(30) {
		for (int i = 0; i < 100; i++)
			bullets[i] = nullptr;
	}

	void draw(Canvas& canvas) { canvas.draw(*this); }

	void update(Canvas& canvas, Mirror& m1, Mirror& m2) {
		frameCount--;
		if (frameCount <= 0) {
			int direction = rand() % 2;

			char* bulletShape;
			if (direction == 0) bulletShape = new char[] { ">" };
			else bulletShape = new char[] { "<" };

			int pos = 0;
			if (direction == 0) pos = getPos() + getShapeSize();
			else pos = getPos() - 1;

			for (int i = 0; i < 100; i++) {
				if (bullets[i] == nullptr) {
					bullets[i] = new Bullet(bulletShape, pos, direction);
					break;
				}
			}

			delete[] bulletShape;
			bulletShape = nullptr;
			frameCount = 30;
		}
		for (int i = 0; i < 100; i++) {
			if (bullets[i] != nullptr) {
				if (bullets[i]->getHP() <= 0) {
					delete(bullets[i]);
					bullets[i] = nullptr;
				}
				bullets[i]->draw(canvas);
				bullets[i]->update(m1, m2);
			}
		}
	}

	~Player() {
		frameCount = 0;
		for (int i = 0; i < 100; i++)
			bullets[i] = nullptr;
	}
};

// problem2 ����
void problem2() {
	int size = 80;

	Canvas* canvas = new Canvas(size);
	Player* player = new Player("(^-^)", 50);
	Mirror* mirror1 = new Mirror(10);
	Mirror* mirror2 = new Mirror(60);

	while (true) {
		canvas->clear();

		mirror1->draw(*canvas);
		mirror2->draw(*canvas);
		player->draw(*canvas);

		player->update(*canvas, *mirror1, *mirror2);

		canvas->render();
		Sleep(100);
	}

	delete mirror1;
	delete mirror2;
	delete player;
	delete canvas;
}


int main() {
	// problem1();
	problem2();

	return 0;
}