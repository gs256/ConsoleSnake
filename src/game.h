#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>

struct Point {
    int x, y;

    Point() {
        x = 0;
        y = 0;
    }

    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    bool operator ==(const Point &other) {
        return ((this->x == other.x) && (this->y == other.y));
    }
};

enum Direction {
    UP = -1, 
    DOWN = 1, 
    LEFT = -2, 
    RIGHT = 2
};


class Game {
public:
    int fileldWidth = 20; 
    int fileldHeight = 20;
    unsigned short hardness = 180;
    wchar_t boundChar = *L"#";
    wchar_t foodChar = *L"$";

private:
    class Food {
    public:
        wchar_t foodChar = *L"$";
        Point position;

    public:
        Food(Point position);
        void Respawn(Point newPosition);
    };

    class Snake {
    public:
        wchar_t headChar = *L"0";
        wchar_t tailChar = *L"o";
        size_t startLen = 1;
        Direction direction = Direction::UP;

        Point headPosition;
        std::vector<Point> tailPositions;

    public:
        Snake(Point headPosition);
        void Move();
        void Grow();
        void UpdateDirection(Direction direction);
    };

    int consoleCols, consoleRows;
    wchar_t *buffer;
    HANDLE hConsole;
    Snake* snake;
    Food* food;
    bool inGame = false;


public:
    Game();
    void Start();
    void UpdateConsoleBuffer();
    void DebugLog(std::wstring msg);

private:
    void GetConsoleSize(int &outColumns, int &outRows);
    void DrawField();
    void ClearConsoleBuffer();
    void DrawSnake();
    void DrawFood();
    void DrawPoint(Point point, wchar_t symbol);
    void DetectCollision();
    int RandomNumber(int min, int max);
    bool IsKeyPressed(short vKey);
};
