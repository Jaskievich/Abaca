

#include "glad.h"

#include <GLFW/glfw3.h>

#include "modelgame.h"
#include "ViewGame.h"
//#define STB_IMAGE_IMPLEMENTATION
//
//#include "stb_image.h"
#include <cstdlib>

#include <Windows.h>



#include <iostream>

#include <string>
#include <unordered_map>
#include <functional>
#include <locale.h>
#include <codecvt>


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

class CtrlGameObjectView
{
public:

	~CtrlGameObjectView() {
		DeleteObjects();
	}

	void CreateObjects(Game& game, ArrayCube& arrayCubes)
	{
		// Создание кубиков
		std::shared_ptr<ArrayCubeView> arrayCubesView = std::make_shared<ArrayCubeView>(arrayCubes);
		arrayCubesView->Create();

		listObjects.push_back(arrayCubesView);

		// Создание проекционной матрицы
		projection = glm::ortho(-0.1f, 3.0f, -0.5f, 2.0f); // Пример проекции
		// Создание графики игровой таблицы
		float cellWidth = 0.1f;
		float cellHeight = 0.1f;
		stext = std::make_shared<SimpleText>();
		doubleGrid = std::make_shared<DoubleGridView>(*stext, cellWidth, cellHeight, 0.12f);
		doubleGrid->SetProjection(projection);
		doubleGrid->FillText();
		listObjects.push_back(doubleGrid);

		// Создание рамки для выбранных кубиков
		std::shared_ptr <RamcaView> rmCubeSelect = std::make_shared<RamcaView>();
		rmCubeSelect->SetBorderWidth(0.01f);
		rmCubeSelect->Create(0.35, 0.09f, 2.56f, 1.52f);
		rmCubeSelect->projection = &projection;
		listObjects.push_back(rmCubeSelect);


		txt = std::make_shared<Text>();
		std::shared_ptr<PlayerName> player1 = std::make_shared < PlayerName>(*txt);
	    listObjects.push_back(player1);

		std::shared_ptr<PlayerName> player2 = std::make_shared < PlayerName>(*txt);
		player2->x = 335.0f;
		listObjects.push_back(player2);

		// Связываем игровые доски
		Payer& currPayer = game.GetCurrentPayer();
		doubleGrid->boardGameLeft = &currPayer.boardGame;
		player1->name = currPayer.name;
		game.NextPayer();
		Payer& currPayer2 = game.GetCurrentPayer();
		player2->name = currPayer2.name;
		doubleGrid->boardGameRight = &currPayer2.boardGame;
		game.NextPayer();

		// Создание рамки для надписи игроков
		rm = std::make_shared < RamcaView>();
		rm->Create(0.45, 0.09f, 0.37f, 1.82f);
		rm->projection = &projection;
		listObjects.push_back(rm);

		ParamGrid par;
		par.col = 3;
		par.row = 3; 
		par.cellWidth = 0.15f;
		par.cellHeight = 0.1f;
		par.paddingX = 0.08f;
		par.paddingY = 0.07f;

		res = std::make_shared<TableResult>(*stext, par, game);
		res->SetProjection(projection);
		res->SetStartPos(2.35f, 0.0f);
		res->FillStaticText(player1->name, player2->name);
		listObjects.push_back(res);

		std::shared_ptr<CurrentStepNumber> currStepNumber = std::make_shared <CurrentStepNumber> (*txt, game);
		currStepNumber->x = 680.0f;
		currStepNumber->y = 250.0f;
		listObjects.push_back(currStepNumber);

	}

	void DrawObjects()
	{
		for (auto item : listObjects) {
			item->Draw();
		}
	}

	void FillDrawObject()
	{
		doubleGrid->FillBoardText();
		res->FillCeillText();
		if (rm->posX == 0.37f)  rm->posX = 1.47;
		else rm->posX = 0.37f;
	}

	void DeleteObjects()
	{
		/*for (auto* item : ListObjrcts) {
			item->Draw();
		}*/
	}

private:

	std::vector<std::shared_ptr<IObjectDraw>> listObjects;
	std::shared_ptr<Text> txt;
	std::shared_ptr < SimpleText> stext;
	std::shared_ptr <DoubleGridView> doubleGrid;
	std::shared_ptr<TableResult> res;
	std::shared_ptr<RamcaView> rm;
	glm::mat4 projection;
};


int main() {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

//	setlocale(LC_ALL, "rus");

	system("chcp 1251");
	
	// Создаем модели игры
	std::vector<std::wstring> names = { L"Player1", L"Player2" };
	Game game(2, names);
	ArrayCube arrayCubes;

	std::unordered_map<int, std::shared_ptr<ScoreCalculator>> calculators = {
		{GLFW_KEY_1 , std::make_shared<ScoolCalculator>()},
		{GLFW_KEY_2 , std::make_shared<ScoolCalculator>()},
		{GLFW_KEY_3 , std::make_shared<ScoolCalculator>()},
		{GLFW_KEY_4 , std::make_shared<ScoolCalculator>()},
		{GLFW_KEY_5 , std::make_shared<ScoolCalculator>()},
		{GLFW_KEY_6 , std::make_shared<ScoolCalculator>()},
		{GLFW_KEY_P , std::make_shared<PairCalculator>()},
		{GLFW_KEY_D , std::make_shared<TwoPairCalculator>()},
		{GLFW_KEY_T , std::make_shared<ThreeOfAKindCalculator>()},
		{GLFW_KEY_F , std::make_shared<FullHouseCalculator>()},
		{GLFW_KEY_E , std::make_shared<StreetSmallCalculator>()},
		{GLFW_KEY_S , std::make_shared<StreetBigCalculator>()},
		{GLFW_KEY_C , std::make_shared<FourOfAKindCalculator>()},
		{GLFW_KEY_A , std::make_shared<FiveOfAKindCalculator>()},
		{GLFW_KEY_ENTER, std::make_shared<SumKindCalculator>()}
	};


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

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	CtrlGameObjectView ctrlGameObjectView;
	ctrlGameObjectView.CreateObjects(game, arrayCubes);

	
	bool isGameContinue = true;


	while (!glfwWindowShouldClose(window) && isGameContinue ) {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// бросок костей
		if (game.currStep < Game::N_STEP) {
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				Sleep(200);
				arrayCubes.SetDiceRandom();
				game.currStep++;
			}
		}

		// Выбор кубиков
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			Sleep(200);
			arrayCubes.NextSelect();
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			Sleep(200);
			arrayCubes.PrevSelect();
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			Sleep(200);
			arrayCubes.SetUp();
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			Sleep(200);
			arrayCubes.SetDown();
		}

		// Выбор игрового символа и вычисления очков игры
		for (const auto& [key, action] : calculators) {
			if (glfwGetKey(window, key) == GLFW_PRESS && game.currStep > 0) {
				if (auto* scool = dynamic_cast<ScoolCalculator*>(action.get())) {
					scool->SetKey(key); 
				}
				int value = action->calculate(arrayCubes.diceValues);
				// заполнить таблицу
				if (game.SetValue(value, action->symbol)) {
					// Проверить закончена ли игра и передать ход следующему игроку
					ctrlGameObjectView.FillDrawObject();
					if (game.IsGameOver()) {
						// Вывести сообщение о победителе и выйти из цикла игры 
						isGameContinue = false;
					}
					else game.NextPayer();
					arrayCubes.Reset();
				}
				Sleep(200);
				break;
			}
		}

		// Вывод объектов
		ctrlGameObjectView.DrawObjects();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//DeleteObjects(ListObjrcts);

	glfwTerminate();
	return 0;
}

