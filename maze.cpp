#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <ctime>
#include <random>
#include <algorithm>

const int CELL_SIZE = 25;
const int WIDTH = 800;
const int HEIGHT = 600;
const int COLS = WIDTH / CELL_SIZE;
const int ROWS = HEIGHT / CELL_SIZE;

struct Cell {
    int x, y;
    bool visited = false;
    bool walls[4] = {true, true, true, true};
};

std::vector<std::vector<Cell>> grid;
std::mt19937 rng;
sf::Color wallColor = sf::Color(220, 220, 220);
sf::Color bgColor = sf::Color(10, 10, 10);

void setupGrid() {
    grid.resize(COLS, std::vector<Cell>(ROWS));
    
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            grid[x][y].x = x;
            grid[x][y].y = y;
            grid[x][y].visited = false;
            for (int i = 0; i < 4; i++) {
                grid[x][y].walls[i] = true;
            }
        }
    }
}

bool isValid(int x, int y) {
    return x >= 0 && x < COLS && y >= 0 && y < ROWS;
}

std::vector<Cell*> getUnvisitedNeighbors(int x, int y) {
    std::vector<Cell*> neighbors;
    
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        
        if (isValid(nx, ny) && !grid[nx][ny].visited) {
            neighbors.push_back(&grid[nx][ny]);
        }
    }
    
    return neighbors;
}

void removeWalls(Cell& current, Cell& next) {
    int x = current.x - next.x;
    int y = current.y - next.y;
    
    if (x == 1) {
        current.walls[3] = false;
        next.walls[1] = false;
    } else if (x == -1) {
        current.walls[1] = false;
        next.walls[3] = false;
    }
    
    if (y == 1) {
        current.walls[0] = false;
        next.walls[2] = false;
    } else if (y == -1) {
        current.walls[2] = false;
        next.walls[0] = false;
    }
}

void generateMaze() {
    std::stack<Cell*> stack;
    
    int startX = std::uniform_int_distribution<int>(0, COLS-1)(rng);
    int startY = std::uniform_int_distribution<int>(0, ROWS-1)(rng);
    
    grid[startX][startY].visited = true;
    stack.push(&grid[startX][startY]);
    
    while (!stack.empty()) {
        Cell* current = stack.top();
        std::vector<Cell*> neighbors = getUnvisitedNeighbors(current->x, current->y);
        
        if (neighbors.empty()) {
            stack.pop();
            continue;
        }
        
        std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
        Cell* next = neighbors[dist(rng)];
        
        next->visited = true;
        removeWalls(*current, *next);
        stack.push(next);
    }
    
    grid[0][0].walls[3] = false;
    grid[COLS-1][ROWS-1].walls[1] = false;
}

void drawCell(sf::RenderWindow& window, const Cell& cell) {
    float x = cell.x * CELL_SIZE;
    float y = cell.y * CELL_SIZE;
    
    sf::VertexArray lines(sf::Lines);
    
    if (cell.walls[0]) {
        lines.append(sf::Vertex(sf::Vector2f(x, y), wallColor));
        lines.append(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y), wallColor));
    }
    
    if (cell.walls[1]) {
        lines.append(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y), wallColor));
        lines.append(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y + CELL_SIZE), wallColor));
    }
    
    if (cell.walls[2]) {
        lines.append(sf::Vertex(sf::Vector2f(x, y + CELL_SIZE), wallColor));
        lines.append(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y + CELL_SIZE), wallColor));
    }
    
    if (cell.walls[3]) {
        lines.append(sf::Vertex(sf::Vector2f(x, y), wallColor));
        lines.append(sf::Vertex(sf::Vector2f(x, y + CELL_SIZE), wallColor));
    }
    
    window.draw(lines);
}

int main() {
    unsigned seed = static_cast<unsigned>(std::time(nullptr));
    rng.seed(seed);
    
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Simple Maze Generator", sf::Style::Close);
    window.setFramerateLimit(60);
    
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
    }
    
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setString("Press space to regenerate | Esc to exit");
    instructions.setCharacterSize(14);
    instructions.setFillColor(sf::Color(150, 150, 150));
    instructions.setPosition(10, HEIGHT - 25);
    
    setupGrid();
    generateMaze();
    
    bool regenerate = false;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    regenerate = true;
                } else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }
        
        if (regenerate) {
            setupGrid();
            generateMaze();
            regenerate = false;
        }
        
        window.clear(bgColor);
        
        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < ROWS; y++) {
                drawCell(window, grid[x][y]);
            }
        }
        
        window.draw(instructions);
        window.display();
    }
    
    return 0;
}