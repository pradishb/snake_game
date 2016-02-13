# Snake Game
A simple snake game made in C.

## Installation
Checkout the latest sources:

    git clone https://github.com/pradishb/snake_game

Get the dependencies:

    sudo add-apt-repository ppa:allegro/5.0
    sudo apt-get update
    sudo apt-get install liballegro5-dev

Compile:

    gcc `pkg-config --libs allegro-5.0` -o snake.out snake.c
    
Run:

    ./snake.out
