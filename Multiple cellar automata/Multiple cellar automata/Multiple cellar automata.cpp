#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <vector>
#include <math.h>
#include <array>
#include <thread>
#include <chrono>
#include <time.h>
#include <limits>
#include <thread>



using namespace std;



const float PI = 3.141592f;
const int window_size = 200; 

const float increasement = 2;

int noise_percentage = 70;

int mutation_change = -1;

const int neighbor_count = 5;

int max_r = 12;
int min_r = 1;

array<vector<array<int, 2>>, neighbor_count> neighborhoods;

array<array<float, 4>, neighbor_count> rules;

int tot = 0;

bool map[window_size][window_size] = {{ false }};
bool map2[window_size][window_size] = { { false } };

float blurred[window_size][window_size] = { { 0 } };
float min_blur = 0.5f;

float directions[9][3] = {
    { 0, 0, 0.25f },
    { 1, 0, 0.11f },
    { 0, -1, 0.11f },
    { -1, 0, 0.11f },
    { 0, 1, 0.11f },
    { 1, 1, 0.05f },
    { -1, 1, 0.05f },
    { -1, -1, 0.05f },
    { 1, -1, 0.05f }
};



vector<array<float, 3>> random_colors;



int RandomMinPlus() {
    if (rand() % 2 == 0) {
        return -1;
    }
    return 1;
}

float RandomFloat(float Max) {
    return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / Max));
}



void Blur() {

    float invert = 1.0f - min_blur;

    for (int i = 1; i < window_size - 1; i++) {
        for (int i2 = 1; i2 < window_size - 1; i2++) {

            float blur = min_blur;

            for (int i3 = 0; i3 < 9; i3++) {
                
                float map_value = map2[i + int(directions[i3][0])][i2 + int(directions[i3][1])];

                blur += directions[i3][2] * map_value * invert;
            }

            blurred[i][i2] = blur;
        }
    }
}

void DrawCube(int x, int y, int size, float r, float g, float b) {

    glBegin(GL_TRIANGLES);
    glColor3f(r, g, b);
    glVertex2i(x, y);
    glVertex2i(x, y + size);
    glVertex2i(x + size, y);

    glColor3f( r, g, b);
    glVertex2i(x + size, y + size);
    glVertex2i(x, y + size);
    glVertex2i(x + size, y);


    glEnd();
}


float CalculateAverage(int x, int y, vector<array<int, 2>> selected_neighbor) {
    float average = 0;

    int size = selected_neighbor.size();

    for (int i = 0; i < size; i++) {

        array<int, 2> dir = selected_neighbor[i];

        int new_x = x + dir[0];
        int new_y = y + dir[1];




        if (new_x < 0) {
            new_x += window_size;
        }
        else if (new_x >= window_size) {
            new_x -= window_size;
        }




        if (new_y < 0) {
            new_y += window_size;
        }
        else if (new_y >= window_size) {
            new_y -= window_size;
        }



        if (map[new_x][new_y]) {
            average++;
        }
       


    }

    average = average / size;

    return average;
}


bool Rules(array<float, neighbor_count> averages, bool selected) {

    int dead = 0;
    int live = 0;

    for (int i = 0; i < neighbor_count; i++) {
        float average = averages[i];
        array<float, 4> rule = rules[i];


        if (rule[0] <= average && rule[1] >= average) {
            dead++;
        }

        if (rule[2] <= average && rule[3] >= average) {
            live++;
        }
    }


    if (live > dead) {
        return true;
    }
    else if (live < dead) {
        return false;
    }

    return selected;

  


}


void TimeStep(int l, int l2) {

    for (int i = 0; i < window_size; i++) {
        for (int i2 = l; i2 < l2; i2++) {

            array<float, neighbor_count> averages;

            for (int i3 = 0; i3 < neighbor_count; i3++) {
                averages[i3] = CalculateAverage(i, i2, neighborhoods[i3]);
               
            }
           
            
            map2[i][i2] = Rules(averages, map[i][i2]);

            if (mutation_change >= rand() % 1000) {
                map2[i][i2] = !map2[i][i2];
            }

          
        }
    }

}

void RandomColors() {

    for (int i = 0; i < 2; i++) {
        array<float, 3> colors;


        colors[0] = RandomFloat(1);
        colors[1] = RandomFloat(1);
        colors[2] = RandomFloat(1);

        random_colors.insert(random_colors.begin(), colors);

    }

}

void GenerateNeighborhood1() {

    
    int MinMax[neighbor_count][2] = { { 0 } };

    
    for (int i = 0; i < neighbor_count; i++) {

        int min = rand() % max_r;
        int max = rand() % max_r;

        if (min < min_r) {
            min = min_r;
        }

        if (max < min) {
            max = min + rand() % (max_r - min + 1);
        }

        MinMax[i][0] = min;
        MinMax[i][1] = max;
    }

    bool occupied[50][50] = { { false } };
    
    for (int i = 0; i < neighbor_count; i++) {
        vector<array<int, 2>> neighborhood;

        
        
        for (int r = MinMax[i][0]; r <= MinMax[i][1]; r++) {
            float angle = 0.001f;
           
          
           

            for (int i2 = 0; i2 < 1000; i2++) {

                float x = (float(r) * cos(angle));
                float y = (float(r) * sin(angle));

                
                
                x = round(x);
                y = round(y);

              

                angle += (2 * PI) / 999;

                if (occupied[(int)x + 25][(int)y + 25] == true || (x == 0 && y == 0)) {
                   
                    continue;
                }

                

                tot++;

                occupied[(int)x + 25][(int)y + 25] = true;

                array<int, 2> dir = { x, y };
                neighborhood.insert(neighborhood.begin(), dir);

               
            }

           

        }
        neighborhoods[i] = neighborhood;
    }

}


void GenerateNoise() {

    noise_percentage = 10 + rand() % 40;


    for (int i = 0; i < window_size; i++) {
        for (int i2 = 0; i2 < window_size; i2++) {

            if (rand() % 100 <= noise_percentage) {
                map[i][i2] = 1;
            } else {
                map[i][i2] = 0;
            }

        }
    }

}



void GenerateRandomRules() {

    for (int i = 0; i < neighbor_count; i++) {

        array<float, 4> rule;



        float min_dead = RandomFloat(1.0f);
        float max_dead = min_dead + RandomFloat(1.0f - min_dead);

        float min_live = RandomFloat(1.0f);
        float max_live = min_live + RandomFloat(1.0f - min_live);

        rule[0] = min_dead;
        rule[1] = max_dead;
        rule[2] = min_live;
        rule[3] = max_live;

        rules[i] = rule;

    }


}


void init(int seed, bool color) {

    
    cout << seed << "\n";
    srand(seed);

    min_r = rand() % 5 + 1;
    max_r = min_r + rand() % 10;

   
    GenerateNeighborhood1();
    GenerateNoise();
    GenerateRandomRules();

    if (color == false) {
        return;
    }

    RandomColors();
    
}

int main(void)
{
    int seed = time(NULL);

    init(seed, true);
    

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_size * increasement, window_size * increasement, "enge dingen", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    glMatrixMode(GL_PROJECTION);
    

    glLoadIdentity();

    cout << "began\n";

    gluOrtho2D(0.0, window_size * increasement, window_size * increasement, 0.0);

    int pressed = 0;


    

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        
        pressed -= 1;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && pressed < 0) {
            seed = time(NULL);
            init(seed, true);
            pressed = 3;
        }

        else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && pressed < 0) {
           
            init(seed, false);
            pressed = 3;
        }

        else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && pressed < 0) {
            RandomColors();
            pressed = 3;
        }

        /* 
        int thread_count = 5;
        

        thread t0(TimeStep, (window_size / thread_count) * 0, (window_size / thread_count) * 1);
        thread t1(TimeStep, (window_size / thread_count) * 1, (window_size / thread_count) * 2);
        thread t2(TimeStep, (window_size / thread_count) * 2, (window_size / thread_count) * 3);
        thread t3(TimeStep, (window_size / thread_count) * 3, (window_size / thread_count) * 4);
        thread t4(TimeStep, (window_size / thread_count) * 4, (window_size / thread_count) * 5);

        t0.join();
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        */




        TimeStep(0, window_size);

       
        Blur();
        
        for (int i = 0; i < window_size * increasement; i += increasement) {
            for (int i2 = 0; i2 < window_size * increasement; i2 += increasement) {


                int x = int(i / increasement);
                int y = int(i2 / increasement);

            
   


                bool selected = map2[x][y];
                map[x][y] = selected;

                int color_index = 0;

                if (selected) {
                    color_index = 1;
                }

                float blur = blurred[x][y];

                float r = random_colors[color_index][0] * blur;
                float g = random_colors[color_index][1] * blur;
                float b = random_colors[color_index][2] * blur;

                DrawCube(i, i2, increasement, r, g, b);

            }
        }

       



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
