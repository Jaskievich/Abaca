

#include "glad.h"

#include <GLFW/glfw3.h>


#include "ViewGame.h"
//#define STB_IMAGE_IMPLEMENTATION
//
//#include "stb_image.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <Windows.h>



#include <iostream>

#include <string>

#include <locale.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void DrawObjects(std::vector<IObjectDraw*> &ListObjrcts) 
{
	for (auto *item : ListObjrcts) {
		item->Draw();
	}
}


int main() {
 
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
   
    setlocale(LC_ALL, "rus");
	srand(time(0));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Создаем окно размером 80% от экрана

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    int screenWidth = mode->width;
    int screenHeight = mode->height;
    int windowWidth = screenWidth * 0.8;
    int windowHeight = screenHeight * 0.8;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Six Dice in a Row", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, windowWidth, windowHeight);

	std::vector<IObjectDraw*> ListObjrcts;
   
    float aspectRatio = (float)windowHeight / (float)windowWidth;
   
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    

  

	// Создание кубиков
    ArrayCubeView arrayCubes;
    arrayCubes.Create();

	ListObjrcts.push_back(&arrayCubes);

	// Создание проекционной матрицы
	glm::mat4 projection = glm::ortho(-0.1f, 2.5f, -0.5f, 2.0f); // Пример проекции
    // Создание графики игровой таблицы
    float cellWidth = 0.1f;
    float cellHeight = 0.1f;
    SimpleText stext;
    DoubleGridView doubleGrid(stext, cellWidth, cellHeight, 0.14f);
	doubleGrid.SetProjection(projection);
    doubleGrid.FillText();
	ListObjrcts.push_back(&doubleGrid);

	

	// Создание рамки для выбранных кубиков
	RamcaView rmCubeSelect;
	rmCubeSelect.SetBorderWidth(0.01f);
	rmCubeSelect.Create(0.3, 0.09f, 2.12f, 1.52f);
	rmCubeSelect.projection = &projection;
	ListObjrcts.push_back(&rmCubeSelect);

    Text txt;
    PlayerName player1(txt);

    player1.isActive = true;

    player1.name = L"Игрок1";

    ListObjrcts.push_back(&player1);

    PlayerName player2(txt);

    player2.name = L"Игрок2";
    player2.x = 335.0f;

    ListObjrcts.push_back(&player2);

    // Создание рамки для надписи игроков
    RamcaView rm;
    rm.Create(0.45, 0.09f, 0.37f, 1.82f);
    rm.projection = &projection;
    ListObjrcts.push_back(&rm);

    ParamGrid par;
    par.col = 3;
    par.row = 3;
    par.cellWidth = 0.11f;
    par.cellHeight = 0.1f;
    par.paddingX = 0.08f;
    par.paddingY = 0.07f;
   
    TableResult res(stext, par);
    res.SetProjection(projection);
    res.SetStartPos(1.95f, 0.0f);
    res.FillStaticText(player1.name, player2.name);
    ListObjrcts.push_back(&res);

 
    while (!glfwWindowShouldClose(window)) {

       
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			arrayCubes.SetDiceRandom();
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            Sleep(100);
			arrayCubes.NextSelect();
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            Sleep(100);
			arrayCubes.PrevSelect();
        }
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			Sleep(100);
			arrayCubes.SetUp();
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			Sleep(100);
			arrayCubes.SetDown();
		}
	
	   DrawObjects(ListObjrcts);

       glfwSwapBuffers(window);
       glfwPollEvents();
    }
    doubleGrid.Delete();
  
    glfwTerminate();
    return 0;
}

