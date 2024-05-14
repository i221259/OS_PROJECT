#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <atomic>
#include<string>
#include <queue>
#include <vector>
#include <utility> // For std::pair
#include <climits>

using namespace std;
using namespace sf;

#define NUM_SPEED_BOOSTS 2

#define BOARD_WIDTH 23
#define BOARD_HEIGHT 24
#define CELL_SIZE 25 // Size of each cell in pixels

int game_board[BOARD_HEIGHT][BOARD_WIDTH] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};
const int NUM_GHOSTS= 4;
int pacman_x=11, pacman_y=14;
struct ghostPos{
int x;
int y;
};

struct ghostPos ghost[NUM_GHOSTS];


int score=0;
int lives=3;
bool gameRun=1;
RenderWindow window(VideoMode(BOARD_WIDTH * CELL_SIZE *1.4, BOARD_HEIGHT * CELL_SIZE), "Pac-Man");

map<string, string> directionToImage = {
    {"left", "pac-left.png"},
    {"right", "pac-right.png"},
    {"up", "pac-up.png"},
    {"down", "pac-down.png"}
};

sem_t ghost_sem, pellet_sem, house_sem, boost_sem;
pthread_mutex_t board_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sfml_mutex = PTHREAD_MUTEX_INITIALIZER;

void* gameEngine(void* arg);
void* userInterface(void* arg);
void* ghostController(void* arg);

int main() {
    pthread_t engine_thread, ui_thread, ghost_threads[NUM_GHOSTS],pacman_thread;

    sem_init(&ghost_sem, 0, 1);
    sem_init(&pellet_sem, 0, 1);
    sem_init(&house_sem, 0, NUM_GHOSTS);
    sem_init(&boost_sem, 0, NUM_SPEED_BOOSTS);



    // Create user interface thread which handles all SFML operations
    pthread_create(&ui_thread, NULL, userInterface, NULL);
    sleep(2); // Ensure SFML is fully up and running

    // Start other threads
    pthread_create(&engine_thread, NULL, gameEngine, NULL);
    pthread_create(&pacman_thread, NULL, pacmanController, NULL);

    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_create(&ghost_threads[i], NULL, ghostController, (void*)(intptr_t)i);
    }

    // Join threads
    while(gameRun);

    // Clean up
    sem_destroy(&ghost_sem);
    sem_destroy(&pellet_sem);
    sem_destroy(&house_sem);
    sem_destroy(&boost_sem);
    pthread_mutex_destroy(&board_mutex);

    return 0;
}





void* pacmanController(void* arg) {
    while (gameRun) {
        	pthread_mutex_lock(&sfml_mutex);

        if (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    gameRun = 0;
                }
            }

            pthread_mutex_lock(&board_mutex);
            string currentDirection = "right"; // Default initial direction
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                if (game_board[pacman_y - 1][pacman_x] != 1) {
                    currentDirection = "up";
                    pacman_y--;
                }
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                if (game_board[pacman_y + 1][pacman_x] != 1) {
                    currentDirection = "down";
                    pacman_y++;
                }
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                if (game_board[pacman_y][pacman_x - 1] != 1) {
                    currentDirection = "left";
                    pacman_x--;
                }
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                if (game_board[pacman_y][pacman_x + 1] != 1) {
                    currentDirection = "right";
                    pacman_x++;
                }
            }

            // Update game board with Pac-Man's new position
            if (game_board[pacman_y][pacman_x] == 0) {
                game_board[pacman_y][pacman_x] = 2;  // Mark Pac-Man's position on the board
                score++;
            }
            for(int i=0;i<NUM_GHOSTS;i++){
                if(pacman_x==ghost[i].x && pacman_y==ghost[i].y){
                lives--;
                pacman_x=11;
                pacman_y=14;
      		}
        }
            if(lives==0){
                cout<<"PLAYER MUK GAYA";
                gameRun=0;
            }
        	pthread_mutex_unlock(&sfml_mutex);

            usleep(10000); // Sleep to throttle the update speed
        }
    }
    return NULL;
}



void* gameEngine(void* arg) {
    pthread_mutex_lock(&sfml_mutex);
    RectangleShape background(Vector2f(CELL_SIZE, CELL_SIZE));
    background.setFillColor(Color::Black);

    CircleShape food(CELL_SIZE / 8);
    food.setFillColor(Color::White);
    for(int i=0;i<NUM_GHOSTS;i++){
    ghost[i].x=10 + i;
    ghost[i].y=12;
    }

	sf::Font font;
	if (!font.loadFromFile("font.ttf"))
	{
	    cout<<"cant Load Font"<<endl;
	}
	Text scoreText;
	scoreText.setFillColor(Color::White);
	scoreText.setFont(font);
	scoreText.setPosition(BOARD_WIDTH * CELL_SIZE +10, BOARD_HEIGHT * CELL_SIZE*0.1);
	scoreText.setCharacterSize(CELL_SIZE*0.8); // in pixels, not points!
	
	

	Texture ghostTexture[NUM_GHOSTS];
	Sprite ghosts[NUM_GHOSTS];
	for(int i=0;i<NUM_GHOSTS;i++){
		ghostTexture[i].loadFromFile("ghost"+to_string(i+1 % 4)+".png");
		ghosts[i].setTexture(ghostTexture[i]);
		
		}
    sf::Texture wallTexture, pacTexture;
    wallTexture.loadFromFile("brick.png");
    sf::Sprite wall(wallTexture);
    wall.setScale(CELL_SIZE / wall.getLocalBounds().width, CELL_SIZE / wall.getLocalBounds().height);

    sf::Sprite player;
    string currentDirection = "right"; // Default direction
    Texture liveTex;
    if(!liveTex.loadFromFile("pac-right.png")){
    cout<<"Live Texture not found"<<endl;
    }
    Sprite liveSprite(liveTex);
    
    


    // Main event loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                gameRun=0;
            }
        }

        // Handle drawing in the main thread only
        window.clear();
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            if (game_board[pacman_y - 1][pacman_x] != 1){
            	currentDirection="up";
                pacman_y--;
                }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            if (game_board[pacman_y + 1][pacman_x] != 1){
                pacman_y++;
                currentDirection="down";
                
                }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (game_board[pacman_y][pacman_x - 1] != 1){
                pacman_x--;
                currentDirection="left";
                            	}
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (game_board[pacman_y][pacman_x + 1] != 1){
		  currentDirection="right";
                pacman_x++;
                }
        }
        usleep(10000);
	pthread_mutex_lock(&board_mutex); //synchronization scenario # 1
        if(game_board[pacman_y][pacman_x] == 0){
                game_board[pacman_y][pacman_x]=2;
                score++;

                scoreText.setString("SCORE: "+to_string(score));

                }
	pthread_mutex_unlock(&board_mutex); //synchronization scenario # 1
        // Background and walls
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
                background.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                window.draw(background);
                if (game_board[i][j] == 1) {
                    wall.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                    window.draw(wall);

                }
                if (game_board[i][j] == 0) {
                    food.setPosition(j * CELL_SIZE + CELL_SIZE/3, i * CELL_SIZE + CELL_SIZE/3);
                    window.draw(food);

                }
            }
        }
        
        
        for(int i=0;i<lives;i++){
            liveSprite.setPosition(BOARD_WIDTH * CELL_SIZE +10+i*30,BOARD_HEIGHT * CELL_SIZE*0.2);
            liveSprite.setScale(CELL_SIZE / player.getLocalBounds().width, CELL_SIZE / player.getLocalBounds().height);
            window.draw(liveSprite);
    }
    

        // Pac-Man
        if (!pacTexture.loadFromFile(directionToImage[currentDirection])) {
            cerr << "Failed to load " << directionToImage[currentDirection] << endl;
            return NULL;
        }
        player.setTexture(pacTexture);
        player.setPosition(pacman_x * CELL_SIZE, pacman_y * CELL_SIZE);
	player.setScale(CELL_SIZE / player.getLocalBounds().width, CELL_SIZE / player.getLocalBounds().height);
                        
	for(int i=0;i<NUM_GHOSTS;i++){
		ghosts[i].setPosition(ghost[i].x * CELL_SIZE,ghost[i].y * CELL_SIZE);
		ghosts[i].setScale(CELL_SIZE / ghosts[i].getLocalBounds().width, CELL_SIZE / ghosts[i].getLocalBounds().height);
		window.draw(ghosts[i]);
	}
       window.draw(player);
	window.draw(scoreText);
	window.display();
    pthread_mutex_unlock(&sfml_mutex);

	usleep(10000); // Sleep for 50 ms
    }

    return NULL;

}




void* userInterface(void* arg) {
    

    return NULL;
}

#include <queue>
#include <vector>
#include <cmath>
#include <queue>
#include <vector>
#include <iostream>

struct Node {
    int x, y;
    Node* parent;

    Node(int _x, int _y, Node* _parent = nullptr)
        : x(_x), y(_y), parent(_parent) {}
};

std::vector<Node*> findPath(int start_x, int start_y, int target_x, int target_y) {
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};

    std::queue<Node*> q;
    std::vector<std::vector<bool>> visited(BOARD_HEIGHT, std::vector<bool>(BOARD_WIDTH, false));

    Node* startNode = new Node(start_x, start_y);
    q.push(startNode);
    visited[start_y][start_x] = true;

    while (!q.empty()) {
        Node* current = q.front();
        q.pop();

        if (current->x == target_x && current->y == target_y) {
            std::vector<Node*> path;
            while (current != nullptr) {
                path.push_back(current);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end()); // Reverse to get the correct order
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int new_x = current->x + dx[i];
            int new_y = current->y + dy[i];

            if (new_x >= 0 && new_x < BOARD_WIDTH && new_y >= 0 && new_y < BOARD_HEIGHT &&
                game_board[new_y][new_x] != 1 && !visited[new_y][new_x]) {
                Node* nextNode = new Node(new_x, new_y, current);
                q.push(nextNode);
                visited[new_y][new_x] = true;
            }
        }
    }

    return std::vector<Node*>(); // No path found
}


void* ghostController(void* arg) {
    int ghost_id = (int)(intptr_t)arg;
    
    cout<<"Ghost with ID :"<<ghost_id<<endl;
    int sleeptime=140000;
	if(ghost_id==0)
		sleeptime *=1.1;
	if(ghost_id==1)
		sleeptime*=2;
	if(ghost_id==2)
		sleeptime *=3;
	if(ghost_id==3)
		sleeptime*=1.3;
    while (true) {
		
	// Assuming ghost and Pac-Man positions are updated elsewhere and accessible




	pthread_mutex_lock(&board_mutex);

        // Find path to Pacman
        std::vector<Node*> path = findPath(ghost[ghost_id].x, ghost[ghost_id].y, pacman_x, pacman_y);



        // Follow the path if it's not empty
                	int i=0;
                if (!path.empty()) {

            // Move towards the next node in the path
            while(path[i]->x == ghost[ghost_id].x && path[i]->y == ghost[ghost_id].y && i<path.size())
            i++;
            }
        if (!path.empty()) {
        
            ghost[ghost_id].x = path[i]->x;
            ghost[ghost_id].y = path[i]->y;

            // Cleanup memory
            for (Node* node : path) {
                delete node;
            }
        }
        pthread_mutex_unlock(&board_mutex);
        usleep(sleeptime); // Sleep for 50 ms
    }

    return NULL;
}


