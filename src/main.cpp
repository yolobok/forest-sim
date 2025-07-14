#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace sf; // using this for our gui to work
using namespace std;

const int GRIDROWS = 75;
const int GRIDCOLS = 75;
const int CELLPIXELS = 10;

int forest[GRIDROWS][GRIDCOLS];
int tempForest[GRIDROWS][GRIDCOLS];

void initializeGrid()
{
    srand(time(0)); // most of the grid is made into a tree
    for (int i = 0; i < GRIDROWS; ++i)
    {
        for (int j = 0; j < GRIDCOLS; ++j)
        {
            if (rand() % 100 < 70)
            {
                forest[i][j] = 1;
            }
            else
            {
                forest[i][j] = 0;
            }
        }
    }

    // fire starter
    bool fireStarted = true;

    while (fireStarted)
    {
        int x = rand() % GRIDROWS, y = rand() % GRIDCOLS;
        if (forest[x][y] == 1)
        {
            forest[x][y] = 2;
            fireStarted = false;
        }
    }
}

// grid save
void saveGrid(const string &forestfile)
{
    ofstream saveFile(forestfile);
    if (!saveFile)
    {
        cerr << "the file is invalid :(" << endl;
        return;
    }
    saveFile << GRIDROWS << " " << GRIDCOLS << endl;
    for (int i = 0; i < GRIDROWS; ++i)
    {
        for (int j = 0; j < GRIDCOLS; ++j)
        {
            saveFile << forest[i][j] << " ";
        }
        saveFile << endl;
    }
    saveFile.close();
    cout << "Grid saved to " << forestfile << endl;
}

// grid load
void loadGrid(const string &forestfile)
{
    ifstream loadFile(forestfile);
    if (!loadFile)
    {
        cerr << "the file is invalid :(" << endl;
        return;
    }
    int rows, cols;
    loadFile >> rows >> cols;

    if (rows != GRIDROWS || cols != GRIDCOLS)
    {
        cerr << "incorrect dimensions" << endl;
        loadFile.close();
        return;
    }

    for (int i = 0; i < GRIDROWS; ++i)
    {
        for (int j = 0; j < GRIDCOLS; ++j)
        {
            if (!(loadFile >> forest[i][j]))
            {
                cerr << "corrupted file" << endl;
                loadFile.close();
                return;
            }
        }
    }

    loadFile.close();
    cout << "successfully loaded: " << forestfile << endl;
}

// grid renderer
void renderGrid(RenderWindow &window)
{
    RectangleShape dimensions(Vector2f(CELLPIXELS, CELLPIXELS));
    for (int i = 0; i < GRIDROWS; ++i)
    {
        for (int j = 0; j < GRIDCOLS; ++j)
        {
            if (forest[i][j] == 1)
                dimensions.setFillColor(Color::Green);
            else if (forest[i][j] == 2)
                dimensions.setFillColor(Color::Red);
            else if (forest[i][j] == -1)
                dimensions.setFillColor(Color(255, 192, 203));
            else if (forest[i][j] == 3)
                dimensions.setFillColor(Color::Yellow);
            else if (forest[i][j] == 4)
                dimensions.setFillColor(Color(150, 75, 0));
            else
                dimensions.setFillColor(Color::Black);
            dimensions.setPosition(j * CELLPIXELS, i * CELLPIXELS);
            window.draw(dimensions);
        }
    }
}

// fire spreader >:)
void updateGrid()
{
    for (int i = 0; i < GRIDROWS; ++i)
    {
        for (int j = 0; j < GRIDCOLS; ++j)
        {
            if (forest[i][j] == 0)
            {
                if (rand() % 2000000 < 1)
                {
                    forest[i][j] = 1;
                }
            }
            if (forest[i][j] == 2 || forest[i][j] == 3)
            {
                if (rand() % 100 < 15)
                {
                    forest[i][j] = 0;
                }
            }
            if (forest[i][j] == 1)
            {
                if ((i > 0 && forest[i - 1][j] == 2) || (i < GRIDROWS - 1 && forest[i + 1][j] == 2) ||
                    (j > 0 && forest[i][j - 1] == 2) || (j < GRIDCOLS - 1 && forest[i][j + 1] == 2))
                {
                    tempForest[i][j] = 2;
                }
                else
                {
                    tempForest[i][j] = 1;
                }
            }
            else if (forest[i][j] == 2)
                tempForest[i][j] = 3;
            else if (forest[i][j] == 3)
                tempForest[i][j] = 4;
            else if (forest[i][j] == 4)
                tempForest[i][j] = 0;
            else
                tempForest[i][j] = forest[i][j];
        }
    }
    for (int i = 0; i < GRIDROWS; ++i)
    {
        for (int j = 0; j < GRIDCOLS; ++j)
        {
            forest[i][j] = tempForest[i][j];
        }
    }
}

int main()
{
    RenderWindow window(VideoMode(GRIDCOLS * CELLPIXELS, GRIDROWS * CELLPIXELS), "CSPROJ");
    Font font;
    if (!font.loadFromFile("papyrus.ttf"))
    {
        cerr << "Error: Could not load font!" << endl;
        return -1;
    }

    Text title("Group 19", font, 50);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    title.setPosition((GRIDCOLS * CELLPIXELS) / 2 - 100, (GRIDROWS * CELLPIXELS) / 2 - 80);

    Text subtitle("Syed Mustafa Hasan", font, 30);
    subtitle.setFillColor(Color::White);
    subtitle.setPosition((GRIDCOLS * CELLPIXELS) / 2 - 140, (GRIDROWS * CELLPIXELS) / 2);

    Texture splashTexture;
        if (!splashTexture.loadFromFile("smileface.png")) {
            cerr << "Error: Could not load splash image!" << endl;
            return -1;
        }
        
    Sprite splashSprite(splashTexture);
    splashSprite.setPosition(0,0);


    bool splashScreen = true;
    initializeGrid();
    bool running = false;
    Music peacefulMusic, sinisterMusic;
    if (!peacefulMusic.openFromFile("peaceful.ogg") || !sinisterMusic.openFromFile("sinister.ogg"))
    {
        cerr << "Error: Could not load music files!" << endl;
        return -1;
    }

    peacefulMusic.setLoop(true);
    sinisterMusic.setLoop(true);
    peacefulMusic.play();

    Clock clck;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (splashScreen)
            {
                if (event.type == Event::KeyPressed)
                {
                    splashScreen = false;
                }
            }
            else
            {
                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Space)
                        running = !running;
                    else if (event.key.code == Keyboard::S)
                        saveGrid("grid.txt");
                    else if (event.key.code == Keyboard::L)
                        loadGrid("grid.txt");
                    else if (event.key.code == Keyboard::R)
                        initializeGrid();
                }
                if (event.type == Event::MouseButtonPressed)
                {
                    int x = event.mouseButton.x / CELLPIXELS;
                    int y = event.mouseButton.y / CELLPIXELS;
                    if (event.mouseButton.button == Mouse::Left)
                    {
                        if(forest[y][x] == 1){
                        forest[y][x] = 2; // adds fire
                        }
                    }
                    else if (event.mouseButton.button == Mouse::Right)
                    {
                        forest[y][x] = -1; // adds extinguishing zone
                    }
                }
            }
        }

        if (splashScreen)
        {
            window.clear(Color::Black);
            window.draw(splashSprite);
            window.draw(title);
            window.draw(subtitle);
            window.display();
            continue;
        }

        if (running && clck.getElapsedTime().asMilliseconds() > 500)
        {
            updateGrid();
            clck.restart();
        }

        int fireCount = 0, treeCount = 0;
        for (int i = 0; i < GRIDROWS; ++i)
        {
            for (int j = 0; j < GRIDCOLS; ++j)
            {
                if (forest[i][j] == 1)
                    treeCount++;
                else if (forest[i][j] == 2 || forest[i][j] == 3)
                    fireCount = fireCount + 25;
            }
        }

        

        if (fireCount > treeCount && peacefulMusic.getStatus() == Music::Playing)
        {
            peacefulMusic.stop();
            sinisterMusic.play();

        }
        else if (treeCount >= fireCount && sinisterMusic.getStatus() == Music::Playing)
        {
            sinisterMusic.stop();
            peacefulMusic.play();
        }
        window.clear();
        renderGrid(window);
        window.display();
    }
    return 0;
}