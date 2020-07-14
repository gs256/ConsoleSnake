#include "game.h"


// Food members declaration

Game::Food::Food(Point position) {
    this->position = position;
}

void Game::Food::Respawn(Point newPosition) {
    position = newPosition;
}


// Snake members declaration

Game::Snake::Snake(Point headPosition) {
    this->headPosition = headPosition;
}

void Game::Snake::Move() {
    Point prevPosition = headPosition;

    switch (direction) {
    case Direction::UP:
        headPosition.y--;
        break;
    case Direction::DOWN:
        headPosition.y++;
        break;
    case Direction::LEFT:
        headPosition.x--;
        break;
    case Direction::RIGHT:
        headPosition.x++;
        break;
    }

    if (tailPositions.size() > 0) {
        Point temp;

        for (Point &tailElementPosition : tailPositions) {
            temp = tailElementPosition;
            tailElementPosition = prevPosition;
            prevPosition = temp;
        }
    }
}

void Game::Snake::Grow() {
    Point newTailElementPosition;

    if (tailPositions.size() > 0)
        newTailElementPosition = tailPositions[tailPositions.size() - 1];
    else
        newTailElementPosition = headPosition;

    tailPositions.push_back(newTailElementPosition);
}

void Game::Snake::UpdateDirection(Direction direction) {
    if (this->direction != -direction)
        this->direction = direction;
}


// Game members declaration

Game::Game() {
    GetConsoleSize(consoleCols, consoleRows);

    buffer = new wchar_t[consoleCols * consoleRows + 1];
    buffer[consoleCols * consoleRows] = '\0';

    hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    for (size_t i = 0; i < consoleCols * consoleRows; i++)
        buffer[i] = *L" ";
}

void Game::Start() {
    this->snake = new Snake(Point((fileldWidth-2)/2, (fileldHeight-2)/2));
    this->food = new Food(Point(6, 10));

    inGame = true;
    size_t tick = 0;
    Direction newDirection = snake->direction;

    DrawField();

    while (inGame) {
        if (IsKeyPressed(VK_DELETE))
            inGame = false;
        if (IsKeyPressed(VK_UP))
            newDirection = Direction::UP;
        if (IsKeyPressed(VK_DOWN))
            newDirection = Direction::DOWN;
        if (IsKeyPressed(VK_LEFT))
            newDirection = Direction::LEFT;
        if (IsKeyPressed(VK_RIGHT))
            newDirection = Direction::RIGHT;

        // Game update
        if (tick >= (255-hardness)/4) {
            snake->UpdateDirection(newDirection);
            snake->Move();
            DetectCollision();

            DrawField();
            DrawFood();
            DrawSnake();

            DebugLog(std::to_wstring(snake->tailPositions.size()));
            UpdateConsoleBuffer();
            ClearConsoleBuffer();
            tick = 0;
        }

        Sleep(15);
        tick++;
    }
    
}

void Game::UpdateConsoleBuffer() {
    DWORD dwBytesWritten = 0;
    WriteConsoleOutputCharacter(hConsole, buffer, consoleCols * consoleRows, { 0, 0 }, &dwBytesWritten);
}

void Game::DebugLog(std::wstring msg) {
    memcpy((void*)(buffer+consoleCols*(consoleRows-1)), (void*)(msg.c_str()), msg.length());
}

void Game::GetConsoleSize(int &outColumns, int &outRows) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    outColumns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    outRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Game::DrawField() {
    for (size_t i = 0; i < fileldWidth; i++) {
        buffer[i] = boundChar;
        buffer[fileldHeight*consoleCols+i] = boundChar; 
    }

    for (size_t i = 1; i < fileldHeight; i++) {
        buffer[consoleCols*i] = boundChar;
        buffer[consoleCols*i+fileldWidth-1] = boundChar;
    }     
}

void Game::ClearConsoleBuffer() {
    for (size_t i = 0; i < consoleCols * consoleRows; i++)
        buffer[i] = *L" ";
}

void Game::DrawSnake() {
    DrawPoint(snake->headPosition, snake->headChar);
    
    for (Point &tailElementPosition : snake->tailPositions) {
        DrawPoint(tailElementPosition, snake->tailChar);
    }
}

void Game::DrawFood() {
    DrawPoint(food->position, food->foodChar);
}

void Game::DrawPoint(Point point, wchar_t symbol) {
    buffer[consoleCols*point.y+point.x] = symbol;
}

void Game::DetectCollision() {
    if ((snake->headPosition.x <= 0) || (snake->headPosition.x >= fileldWidth-1)
        || (snake->headPosition.y <= 0) || (snake->headPosition.y >= fileldHeight)) {
        inGame = false;
    }

    for (Point &tailElementPosition: snake->tailPositions) {
        if (snake->headPosition == tailElementPosition)
            inGame = false;
    }

    if (snake->headPosition == food->position) {
        snake->Grow();

        size_t newFoodXPos = RandomNumber(1, fileldWidth - 1);
        size_t newFoodYPos = RandomNumber(1, fileldHeight - 1);
        food->Respawn(Point(newFoodXPos, newFoodYPos));
    }

}

int Game::RandomNumber(int min, int max) {
    std::srand(time(0));
    return std::rand() % (max-min) + min;
}

bool Game::IsKeyPressed(short vKey) {
    return GetAsyncKeyState(vKey);
}