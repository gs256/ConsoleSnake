all:
	g++ src/snake.cpp src/game.cpp -municode -o build/snake
	./build/snake
