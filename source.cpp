#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <semaphore.h>
#include<unistd.h>
#include<iostream>
#include<map>
using namespace std;

#define NUM_GHOSTS 4
#define NUM_SPEED_BOOSTS 2


#define BOARD_WIDTH 20
#define BOARD_HEIGHT 20
#define CELL_SIZE 30 // Size of each cell in pixels

int game_board[BOARD_WIDTH][BOARD_HEIGHT] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};


int pacman_x=1, pacman_y=1;
int ghost_x[NUM_GHOSTS], ghost_y[NUM_GHOSTS];
int score = 0;
int lives = 3;
std::map<std::string, std::string> directionToImage = {
    {"left", "pac-left.png"},
    {"right", "pac-right.png"},
    {"up", "pac-up.png"},
    {"down", "pac-down.png"}
};
sem_t ghost_sem, pellet_sem, house_sem, boost_sem;

pthread_mutex_t board_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lives_mutex = PTHREAD_MUTEX_INITIALIZER;

void* gameEngine(void* arg);
void* userInterface(void* arg);
void* ghostController(void* arg);

int main() {

cout<<"hello";
    pthread_t engine_thread, ui_thread, ghost_threads[NUM_GHOSTS],pacman_thread;
    int i;

    sem_init(&ghost_sem, 0, 1);
    sem_init(&pellet_sem, 0, 1);
    sem_init(&house_sem, 0, NUM_GHOSTS);
    sem_init(&boost_sem, 0, NUM_SPEED_BOOSTS);

    pthread_create(&engine_thread, NULL, gameEngine, NULL);
    pthread_create(&ui_thread, NULL, userInterface, NULL);
sleep(2);
    for (i = 0; i < NUM_GHOSTS; i++) {
        pthread_create(&ghost_threads[i], NULL, ghostController, (void*)(intptr_t)i);
    }

   

    sem_destroy(&ghost_sem);
    sem_destroy(&pellet_sem);
    sem_destroy(&house_sem);
    sem_destroy(&boost_sem);
    pthread_mutex_destroy(&board_mutex);
    pthread_mutex_destroy(&score_mutex);
    pthread_mutex_destroy(&lives_mutex);

    return 0;
}

void* gameEngine(void* arg) {
    while (true) {
        // Handle user input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            if (game_board[pacman_x - 1][pacman_y] == 0)
                pacman_x--;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            if (game_board[pacman_x + 1][pacman_y] == 0)
                pacman_x++;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (game_board[pacman_x][pacman_y - 1] == 0)
                pacman_y--;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (game_board[pacman_x][pacman_y + 1] == 0)
                pacman_y++;
        }
usleep(100000); // Sleep for 100 ms
    }

    return NULL;
}




void* userInterface(void* arg) {
    sf::RenderWindow window(sf::VideoMode(BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE), "Pac-Man");

    sf::RectangleShape background(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    background.setFillColor(sf::Color::Black);

sf::CircleShape food(CELL_SIZE/8);
    food.setFillColor(sf::Color::White);

   
    sf::Texture wallTexture;
    wallTexture.loadFromFile("brick.png");
    sf::Sprite wall;
    wall.setTexture(wallTexture);
    wall.setScale(CELL_SIZE / wall.getLocalBounds().width, CELL_SIZE / wall.getLocalBounds().height);

    sf::Texture pacTexture;
    std::string currentDirection = "right"; // Default direction
    if (!pacTexture.loadFromFile(directionToImage[currentDirection])) {
        std::cerr << "Failed to load " << directionToImage[currentDirection] << std::endl;
        return NULL;
    }

    sf::Sprite player;
    player.setTexture(pacTexture);
    player.setScale(CELL_SIZE / player.getLocalBounds().width, CELL_SIZE / player.getLocalBounds().height);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left)
                    currentDirection = "left";
                else if (event.key.code == sf::Keyboard::Right)
                    currentDirection = "right";
                else if (event.key.code == sf::Keyboard::Up)
                    currentDirection = "up";
                else if (event.key.code == sf::Keyboard::Down)
                    currentDirection = "down";

                // Load the corresponding image based on the current direction
                if (!pacTexture.loadFromFile(directionToImage[currentDirection])) {
                    std::cerr << "Failed to load " << directionToImage[currentDirection] << std::endl;
                    return NULL;
                }
                player.setTexture(pacTexture);
                player.setScale(CELL_SIZE / player.getLocalBounds().width, CELL_SIZE / player.getLocalBounds().height);
            }
        }

        window.clear();

        // Draw background, walls, and player
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
                background.setPosition(j * CELL_SIZE, i * CELL_SIZE); // Adjusted positions
                window.draw(background);

                if (game_board[i][j] == 1) { // Adjusted array access
                    wall.setPosition(j * CELL_SIZE, i * CELL_SIZE); // Adjusted positions
                    window.draw(wall);
                }
                if (game_board[i][j] == 0) { // Adjusted array access
                    food.setPosition(j * CELL_SIZE + CELL_SIZE/3, i * CELL_SIZE + CELL_SIZE/3); // Adjusted positions
                    window.draw(food);
                }
               
            }
        }

        player.setPosition(pacman_y * CELL_SIZE, pacman_x * CELL_SIZE);
        window.draw(player);

        window.display();
    }

    return NULL;
}



void* ghostController(void* arg) {
    int ghost_id = (int)(intptr_t)arg;

    while (true) {
        pthread_mutex_lock(&board_mutex);
        sem_wait(&ghost_sem);

        // Ghost movement logic
        // Update ghost position based on game state

        pthread_mutex_unlock(&board_mutex);
        sem_post(&ghost_sem); // Release the ghost semaphore

        usleep(50000); // Sleep for 50 ms
    }

    return NULL;
}



