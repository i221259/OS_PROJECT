#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <map>
#include <string>
#include <queue>
#include <vector>
#include <utility> // For std::pair
#include <climits>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <limits.h>  // For PATH_MAX definition

char currentPath[PATH_MAX];


using namespace std;
using namespace sf;

#define NUM_SPEED_BOOSTS 2
#include <iostream>
#include <filesystem> // C++17 filesystem library

bool scared=0;
#define BOARD_WIDTH 23
#define BOARD_HEIGHT 24
#define CELL_SIZE 25 // Size of each cell in pixels

int game_board[BOARD_HEIGHT][BOARD_WIDTH] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 1, 4, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 4, 1, 0, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
{1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
{1, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};
const int NUM_GHOSTS= 4;
int pacman_x=11, pacman_y=14;
struct ghostPos{
int x;
int y;
bool inHouse=true;
bool canLeave=false;
bool isfast=false;
bool canfast=false;
bool timeChanged=false;
};
int flashes=2;
int powerPellets=4;
Texture pacTexture;
struct ghostPos ghost[NUM_GHOSTS];
Clock gameclock;
float timePassed;
float scaredTime=0;
string currentDirection="right";
int score=0;
int lives=3;
bool gameRun=0;
bool startMenu=1;
bool exitMenu = 0;
RenderWindow window(VideoMode(BOARD_WIDTH * CELL_SIZE *1.4, BOARD_HEIGHT * CELL_SIZE), "Pac-Man");

map<string, string> directionToImage = {
    {"left", "pac-left.png"},
    {"right", "pac-right.png"},
    {"up", "pac-up.png"},
    {"down", "pac-down.png"}
};

sem_t ghost_sem, pellet_sem, house_sem, boost_sem,keys,permits;
pthread_mutex_t board_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sfml_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lives_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t power_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t flash_mutex = PTHREAD_MUTEX_INITIALIZER;



void* gameEngine(void* arg);
void* userInterface(void* arg);
void* ghostController(void* arg);
void* pacmanController(void* arg);
int main() {
    pthread_t engine_thread, ui_thread, ghost_threads[NUM_GHOSTS],pacman_thread;

   
    sem_init(&keys, 0, 2);    // Initialize each semaphore to 1 (binary semaphore)
    sem_init(&permits, 0, 2); // Initialize each semaphore to 1
   
   

if (getcwd(currentPath, sizeof(currentPath)) != nullptr) {
    std::cout << "Current Path: " << currentPath << std::endl;
} else {
    perror("getcwd() error");
    return 1;
}
    int n=rand()%4;
    int m;
    ghost[n].canfast=true;
    cout<<"Ghost "<<n<<" can be Fast"<<endl;
    while(1){
    m=rand()%4;
    if(m!=n)
    break;
    }
    ghost[m].canfast=true;
    cout<<"Ghost "<<m<<" can be Fast"<<endl;

    // Create user interface thread which handles all SFML operations
    pthread_create(&ui_thread, NULL, userInterface, NULL);
while(!gameRun)
sleep(1);
    // Start other threads
    pthread_create(&engine_thread, NULL, gameEngine, NULL);
        sleep(0.5); // Ensure SFML is fully up and running

    pthread_create(&pacman_thread, NULL, pacmanController, NULL);

    for (int i = 0; i < NUM_GHOSTS; i++) {
        pthread_create(&ghost_threads[i], NULL, ghostController, (void*)(intptr_t)i);
    }

    // Join threads
    while(gameRun);

    pthread_create(&ui_thread, NULL, userInterface, NULL);
    while(exitMenu);
    sleep(1);

    // Clean up
    sem_destroy(&keys);
    sem_destroy(&permits);
    pthread_mutex_destroy(&board_mutex);

    return 0;
}




void* pacmanController(void* arg) {
    while(startMenu)
    sleep(1);
    while (gameRun) {
        pthread_mutex_lock(&sfml_mutex);
            //cout<<"sfml lock pacman"<<endl;

        if (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    gameRun = 0;sleep(1);
                    sleep(1);
                    window.close();
                }
            }

            pthread_mutex_lock(&board_mutex);
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
            pthread_mutex_unlock(&board_mutex); //synchronization scenario # 1
            pthread_mutex_lock(&power_mutex);

            if (game_board[pacman_y][pacman_x] == 0) {
                game_board[pacman_y][pacman_x] = 2;  // Mark Pac-Man's position on the board
                score++;
            }
            if (game_board[pacman_y][pacman_x] == 3 && !scared) {
                game_board[pacman_y][pacman_x] = 2;  // Mark Pac-Man's position on the board
                powerPellets--;
                scared=1;
                scaredTime=gameclock.getElapsedTime().asSeconds() + 5.0;
                cout<<"**SCARED TIME IS : ****** "<<scaredTime<<endl;
            }

            pthread_mutex_unlock(&board_mutex); //synchronization scenario # 1
                pthread_mutex_unlock(&power_mutex);

           
            if(lives<=0){
                cout<<"PLAYER MUK GAYA";
                cout<<"GAME OVER";
                gameRun=0;
                exitMenu = 1;
                sleep(1);
            }
        pthread_mutex_unlock(&sfml_mutex);
            //cout<<"sfml unlock pacman"<<endl;

usleep(100000); // Sleep for 50 ms
        }
    }
    return NULL;
}



void* gameEngine(void* arg) {
    while(startMenu)
    sleep(1);
    pthread_mutex_lock(&sfml_mutex);
    //cout<<"sfml lock game engine"<<endl;
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

Texture flashTex;
    flashTex.loadFromFile("flash.png");
    Sprite flash;
    flash.setTexture(flashTex);
    Texture powerTex;
    Sprite powerSprite;
    powerTex.loadFromFile("cherry.png");
    powerSprite.setTexture(powerTex);
Texture ghostTexture[NUM_GHOSTS];
Sprite ghosts[NUM_GHOSTS];
for(int i=0;i<NUM_GHOSTS;i++){
ghostTexture[i].loadFromFile("ghost"+to_string(i+1 % 4)+".png");
ghosts[i].setTexture(ghostTexture[i]);
}
    sf::Texture wallTexture;
    wallTexture.loadFromFile("brick.png");
    sf::Sprite wall(wallTexture);
    wall.setScale(CELL_SIZE / wall.getLocalBounds().width, CELL_SIZE / wall.getLocalBounds().height);

    sf::Sprite player;
    Texture liveTex;
    if(!liveTex.loadFromFile("pac-right.png")){
    cout<<"Live Texture not found"<<endl;
    }
    Sprite liveSprite(liveTex);
    pthread_mutex_unlock(&sfml_mutex);
                //cout<<"sfml unlock game engine"<<endl;
    Texture scaredTex;
    scaredTex.loadFromFile("scared.png");


    // Main event loop
    while (window.isOpen() && gameRun) {
        pthread_mutex_lock(&sfml_mutex);
                //cout<<"sfml lock game engine"<<endl;
        sf::Event event;
        while (gameRun && window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameRun=0;
                sleep(1);
                window.close();
               
            }
        }
        float timePassed=gameclock.getElapsedTime().asSeconds();
       window.clear();
        // Handle drawing in the main thread only

/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
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
        }*/
/*pthread_mutex_lock(&board_mutex); //synchronization scenario # 1
        if(game_board[pacman_y][pacman_x] == 0){
                game_board[pacman_y][pacman_x]=2;
                score++;

                scoreText.setString("SCORE: "+to_string(score));

                }
               
pthread_mutex_unlock(&board_mutex);*/ //synchronization scenario # 1
                    scoreText.setString("SCORE: "+to_string(score));

        // Background and walls
        if(powerPellets<4){
            pthread_mutex_lock(&power_mutex);
            powerPellets++;
            while(1){
                int x=rand()%23;
                int y=rand()%24;
                if(game_board[x][y]==2){
                    game_board[x][y]=3;
                    break;
                }
            }
            pthread_mutex_unlock(&power_mutex);
        }

        if(flashes<2){
            pthread_mutex_lock(&flash_mutex);
            flashes++;
            while(1){
                int x=rand()%23;
                int y=rand()%24;
                if(game_board[x][y]==2){
                    game_board[x][y]=4;
                    break;
                }
            }
            pthread_mutex_unlock(&flash_mutex);
        }
       
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
                if (game_board[i][j] == 3) {
                    powerSprite.setPosition(j * CELL_SIZE , i * CELL_SIZE );
                    powerSprite.setScale(CELL_SIZE / powerSprite.getLocalBounds().width, CELL_SIZE / powerSprite.getLocalBounds().height);
                    window.draw(powerSprite);

                }
                if (game_board[i][j] == 4) {
                    flash.setPosition(j * CELL_SIZE , i * CELL_SIZE );
                    flash.setScale(CELL_SIZE / flash.getLocalBounds().width, CELL_SIZE / flash.getLocalBounds().height);
                    window.draw(flash);
                }
            }
        }
       
        for(int i=0;i<NUM_GHOSTS;i++){
                if(pacman_x==ghost[i].x && pacman_y==ghost[i].y && lives>0 ){
                    if(!scared){
                lives--;
               
                for(int j=0;j<NUM_GHOSTS;j++){
                    ghost[j].isfast=false;
                    ghost[j].timeChanged=false;
                    ghost[j].canLeave=0;
                }
                cout<<"----------CURRENT LIVES: ----------"<<lives<<endl;
                pacman_x=11;
                pacman_y=14;
                for(int i=0;i<NUM_GHOSTS;i++){
                    ghost[i].x=10 + i;
                    ghost[i].y=12;
                }
                    }
                    else{
                    cout<<"----------GHOST KILLED ----------"<<endl;
                        ghost[i].x=10 + i;
                        ghost[i].y=12;
                        ghost[i].canLeave=false;
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
                if(scared){
                    if(timePassed>=scaredTime)
                    {
                        scared=0;
                    for(int j=0;j<NUM_GHOSTS;j++){
           ghosts[j].setTexture(ghostTexture[j]);
           }
                    }
                    if(scared)
                    ghosts[i].setTexture(scaredTex);

                }
           
            ghosts[i].setPosition(ghost[i].x * CELL_SIZE,ghost[i].y * CELL_SIZE);
            ghosts[i].setScale(CELL_SIZE / ghosts[i].getLocalBounds().width, CELL_SIZE / ghosts[i].getLocalBounds().height);
            window.draw(ghosts[i]);
        }
    window.draw(player);
    window.draw(scoreText);
    window.display();
    pthread_mutex_unlock(&sfml_mutex);
    //cout<<"sfml unlock game enginer"<<endl;

usleep(100); // Sleep for 50 ms
    }

    return NULL;

}




void* userInterface(void* arg) {
while (startMenu) {
        pthread_mutex_lock(&sfml_mutex);

        RectangleShape background(Vector2f(CELL_SIZE, CELL_SIZE));
        background.setFillColor(Color::Black);

        sf::Texture menuTexture;
        if (!menuTexture.loadFromFile("start.png")) {
            cerr << "Failed to load start.png" << endl;
        }
        sf::Sprite menuSprite(menuTexture);
        menuSprite.setScale(CELL_SIZE / menuSprite.getLocalBounds().width*32, CELL_SIZE / menuSprite.getLocalBounds().height*25);

        window.draw(menuSprite);

        window.display();

        // Check for Enter key press to exit menu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            startMenu = false;
            gameRun = true;
            sleep(1);
        }
    pthread_mutex_unlock(&sfml_mutex);
    }  


    while (exitMenu) {
        pthread_mutex_lock(&sfml_mutex);

        RectangleShape background(Vector2f(CELL_SIZE, CELL_SIZE));
        background.setFillColor(Color::Black);

        sf::Texture menuTexture;
        if (!menuTexture.loadFromFile("exit.png")) {
            cerr << "Failed to exit start.png" << endl;
        }
        sf::Sprite menuSprite(menuTexture);
        menuSprite.setScale(CELL_SIZE / menuSprite.getLocalBounds().width*32, CELL_SIZE / menuSprite.getLocalBounds().height*25);

        window.draw(menuSprite);

        window.display();

        // Check for Enter key press to exit menu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            exitMenu=0;
            sleep(1);
            window.close();
        }
    pthread_mutex_unlock(&sfml_mutex);
    }    

    return NULL;
}


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
    while(startMenu)
    sleep(1);
    int ghost_id = (int)(intptr_t)arg;
   
   
    cout<<"Ghost with ID :"<<ghost_id<<endl;
    float sleeptime=140000;
if(ghost_id==0)
sleeptime *=1.5;
if(ghost_id==1)
sleeptime*=2;
if(ghost_id==2)
sleeptime *=3;
if(ghost_id==3)
sleeptime*=1.3;


    while (gameRun) {


        pthread_mutex_lock(&flash_mutex);

            if(game_board[ghost[ghost_id].y][ghost[ghost_id].x]==4 && ghost[ghost_id].canfast && !ghost[ghost_id].isfast){
                game_board[ghost[ghost_id].y][ghost[ghost_id].x]=2;
                flashes--;
                ghost[ghost_id].isfast=true;
            }
            pthread_mutex_unlock(&flash_mutex);



if(ghost[ghost_id].isfast && !ghost[ghost_id].timeChanged){
            sleeptime*=0.8;
            ghost[ghost_id].timeChanged=1;
        }
if(ghost[ghost_id].x>8 && ghost[ghost_id].x<14 && ghost[ghost_id].y>10 && ghost[ghost_id].y<14){
    ghost[ghost_id].inHouse=true;
    ghost[ghost_id].timeChanged=0;
    ghost[ghost_id].isfast=0;
        //cout<<"Ghost "<<ghost_id<<" is inside the house"<<endl;
}
else{
    ghost[ghost_id].inHouse=false;
    //cout<<"Ghost "<<ghost_id<<" has left the house"<<endl;
}
if(!ghost[ghost_id].canLeave ){
    cout<<"Ghost "<<ghost_id<<" cannot leave"<<endl;
}

if (ghost[ghost_id].inHouse && !ghost[ghost_id].canLeave ) {
    cout<<"Ghost "<<ghost_id<<"waiting for za key"<<endl;
            // Different order for ghost #3 to prevent deadlock
            if (ghost_id == 3) {
                sem_wait(&permits); // First acquire permit
                cout<<"Ghost "<<ghost_id<<" got da permit"<<endl;
                sem_wait(&keys);    // Then acquire key
                cout<<"Ghost "<<ghost_id<<" got da key"<<endl;
            } else {
                sem_wait(&keys);    // First acquire key
                cout<<"Ghost "<<ghost_id<<" got da key"<<endl;
                sem_wait(&permits); // Then acquire permit
                cout<<"Ghost "<<ghost_id<<" got da permit"<<endl;
               
            }

            // Ghost is now ready to leave the house
            ghost[ghost_id].canLeave = true;
            sleep(2);
            cout << "Ghost " << ghost_id << " is leaving the house." << endl;

            // Release the resources after leaving the house
            sem_post(&keys);
            sem_post(&permits);
        }
    if(ghost[ghost_id].canLeave){
// Assuming ghost and Pac-Man positions are updated elsewhere and accessible
pthread_mutex_lock(&board_mutex);
    pthread_mutex_lock(&power_mutex);

        // Find path to Pacman
        std::vector<Node*> path = findPath(ghost[ghost_id].x, ghost[ghost_id].y, pacman_x, pacman_y);
        // Follow the path if it's not empty
        int i=0;
        if (!path.empty()) {

            // Move towards the next node in the path
        while(i<path.size() && path[i]->x == ghost[ghost_id].x && path[i]->y == ghost[ghost_id].y )
            i++;
        }
        if(i==path.size())
        i--;
        if (!path.empty()) {
        if(!scared){
            ghost[ghost_id].x = path[i]->x;
            ghost[ghost_id].y = path[i]->y;
        }
        else{
            while(1){
                int x=rand()%4 +1;
                int newX=ghost[ghost_id].x;
                int newY=ghost[ghost_id].y;
                if(x==1){
                    newX--;
                }
                if(x==2){
                    newX++;
                }
                if(x==3){
                    newY++;
                }
                if(x==4){
                    newY--;
                }
                if(newX<BOARD_WIDTH && newY<BOARD_HEIGHT && game_board[newY][newX]!=1 && !(newX==path[i]->x && newY==path[i]->y)){
                    ghost[ghost_id].x = newX;
                    ghost[ghost_id].y = newY;
                    break;
                }
            }

        }
            // Cleanup memory
            for (Node* node : path) {
                delete node;
            }
        }
        pthread_mutex_unlock(&board_mutex);
        pthread_mutex_unlock(&power_mutex);

        usleep(sleeptime); // Sleep for 50 ms
    }
    }

exit(0);

}
