

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

//
//
//int rollDice() {
//    return rand() % 6 + 1;
//}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
//
//
//GLuint CreateTextTexture(const std::string& text, const char* fontPath = "arial.ttf", int fontSize = 8) {
//	// Инициализация FreeType
//	FT_Library ft;
//	if (FT_Init_FreeType(&ft)) {
//		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
//		return 0;
//	}
//
//	// Загрузка шрифта
//	FT_Face face;
//	if (FT_New_Face(ft, fontPath, 0, &face)) {
//		std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
//		FT_Done_FreeType(ft);
//		return 0;
//	}
//
//	// Установка размера шрифта
//	FT_Set_Pixel_Sizes(face, 0, fontSize);
//
//	// 1. Рассчитываем размеры текстуры
//	int textureWidth = 0;
//	int textureHeight = 0;
//
//	// Проходим по всем символам строки для вычисления размеров
//	for (char c : text) {
//		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
//			std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
//			continue;
//		}
//
//		textureWidth += face->glyph->bitmap.width;
//		textureHeight = max(textureHeight, (int)face->glyph->bitmap.rows);
//	}
//
//	// Добавляем небольшой отступ между символами
//	textureWidth += text.length() * 2;
//
//	// 2. Создаём временный буфер для текстуры
//	std::vector<unsigned char> textureData(textureWidth * textureHeight, 0);
//
//	// 3. Рендерим каждый символ в буфер
//	int xOffset = 0;
//	for (char c : text) {
//		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
//			continue;
//		}
//
//		FT_Bitmap bitmap = face->glyph->bitmap;
//
//		for (unsigned int y = 0; y < bitmap.rows; y++) {
//			unsigned int flipped_y = bitmap.rows - 1 - y;
//			for (unsigned int x = 0; x < bitmap.width; x++) {
//				textureData[(flipped_y * textureWidth) + xOffset + x] =
//					bitmap.buffer[y * bitmap.width + x];
//			}
//		}
//		xOffset += bitmap.width + 2; // +2 пикселя отступ
//	}
//
//	// 4. Создаём текстуру OpenGL
//	GLuint textureID;
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//
//	// Загружаем данные в текстуру
//	glTexImage2D(
//		GL_TEXTURE_2D,
//		0,
//		GL_RED,
//		textureWidth,
//		textureHeight,
//		0,
//		GL_RED,
//		GL_UNSIGNED_BYTE,
//		textureData.data()
//	);
//
//	// Устанавливаем параметры текстуры
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// Очищаем ресурсы FreeType
//	FT_Done_Face(face);
//	FT_Done_FreeType(ft);
//
//	return textureID;
//}

int main() {
 
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
   
    setlocale(LC_ALL, "rus");

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
   
    float aspectRatio = (float)windowHeight / (float)windowWidth;
   
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ArrayCubeView arrayCubes;

    arrayCubes.Create();

    // Инициализация
    float cellWidth = 0.1f;
    float cellHeight = 0.1f;
    DoubleGridView doubleGrid(cellWidth, cellHeight, 0.14f);

    srand(time(0));

    // Создание проекционной матрицы
    glm::mat4 projection = glm::ortho(-0.1f, 2.5f, -0.5f, 2.0f); // Пример проекции

//  
	Text txt;

	doubleGrid.FillText();

    int highlightedDice = 0;

 
    while (!glfwWindowShouldClose(window)) {

       
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			arrayCubes.SetDiceRandom();
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            Sleep(100);
            highlightedDice = (highlightedDice + 1) % N_COUNT_DICE;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            Sleep(100);
            highlightedDice = (highlightedDice - 1 + N_COUNT_DICE) % N_COUNT_DICE;
        }
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			Sleep(100);
			arrayCubes.stateUp[highlightedDice] = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			Sleep(100);
			arrayCubes.stateUp[highlightedDice] = 0;
		}

  
       arrayCubes.Draw(highlightedDice);

     

	   float x = 25.0f;
	   float y = HEIGHT - 50.0f;

	   float scale = 0.5f;

	   txt.RenderText("player1", x, y, scale, glm::vec3(1.0f, 1.0f, 0.4f));

	   x = 335.0f;
	   txt.RenderText("player2", x, y, scale, glm::vec3(1.0f, 1.0f, 0.4f));

	 
	    doubleGrid.Draw(projection);
	

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    doubleGrid.Delete();
  
    glfwTerminate();
    return 0;
}


/*
#include <iostream>
#include <map>
#include <string>


#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Character {
	GLuint TextureID;
	glm::ivec2 Size;      // Размер символа
	glm::ivec2 Bearing;   // Смещение от базовой линии
	GLuint Advance;       // Смещение до следующего символа
};

std::map<GLchar, Character> Characters;

GLuint VAO, VBO;
GLuint shaderProgram;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const char* _vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec4 vertex; // <pos.xy, texCoord.xy>

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
}
)glsl";

const char* _fragmentShaderSource = R"glsl(
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	float alpha = texture(text, TexCoords).r;
	FragColor = vec4(textColor, alpha);
}
)glsl";


void LoadCharacters(FT_Face face) {

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cerr << "Failed to load Glyph " << c << std::endl;
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<GLuint>(face->glyph->advance.x)
		};

		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderText(std::string text, float x, float y, float scale, glm::vec3 color) {
	glUseProgram(shaderProgram);
	glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	for (auto c : text) {
		Character ch = Characters[c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


int main() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Text in Frame without GLEW", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader;
	shader.Compile(_vertexShaderSource, _fragmentShaderSource);
	shaderProgram = shader.ID;


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "Failed to init FreeType\n";
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
		std::cerr << "Failed to load font\n";
		return -1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	LoadCharacters(face);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	std::string text = "Hello OpenGL!";

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float x = 2.0f;
		float y = HEIGHT - 150.0f;

		float scale = 0.5f;

		RenderText(text, x, y, scale, glm::vec3(1.0f, 1.0f, 0.4f));

		//// Рассчитаем размеры текста для рамки
		//float textWidth = 0.0f;
		//for (auto c : text) {
		//    Character ch = Characters[c];
		//    textWidth += (ch.Advance >> 6) * scale;
		//}
		//float textHeight = 48.0f * scale;

		//// Рисуем рамку с помощью GL_LINES
		//glUseProgram(0);
		//glLineWidth(2.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		//glBegin(GL_LINE_LOOP);
		//glVertex2f(x - 5.0f, y + 5.0f);
		//glVertex2f(x + textWidth + 5.0f, y + 5.0f);
		//glVertex2f(x + textWidth + 5.0f, y - textHeight - 5.0f);
		//glVertex2f(x - 5.0f, y - textHeight - 5.0f);
		//glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (auto& c : Characters) {
		glDeleteTextures(1, &c.second.TextureID);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

*/

/*

#include <iostream>
#include <map>
#include <string>


#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//
//const unsigned int WIDTH = 800;
//const unsigned int HEIGHT = 600;
//
//const char* _vertexShaderSource = R"glsl(
//#version 330 core
//layout (location = 0) in vec4 vertex; // <pos.xy, texCoord.xy>
//
//out vec2 TexCoords;
//
//uniform mat4 projection;
//
//void main()
//{
//    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
//    TexCoords = vertex.zw;
//}
//)glsl";
//
//const char* _fragmentShaderSource = R"glsl(
//#version 330 core
//in vec2 TexCoords;
//out vec4 FragColor;
//
//uniform sampler2D text;
//uniform vec3 textColor;
//
//void main()
//{
//    float alpha = texture(text, TexCoords).r;
//    FragColor = vec4(textColor, alpha);
//}
//)glsl";




int main() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Text in Frame without GLEW", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}



	std::string text = "Hello OpenGL!";

	Text txt;


	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float x = 2.0f;
		float y = HEIGHT - 150.0f;

		float scale = 0.5f;

		txt.RenderText(text, x, y, scale, glm::vec3(1.0f, 1.0f, 0.4f));

		//// Рассчитаем размеры текста для рамки
		//float textWidth = 0.0f;
		//for (auto c : text) {
		//    Character ch = Characters[c];
		//    textWidth += (ch.Advance >> 6) * scale;
		//}
		//float textHeight = 48.0f * scale;

		//// Рисуем рамку с помощью GL_LINES
		//glUseProgram(0);
		//glLineWidth(2.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		//glBegin(GL_LINE_LOOP);
		//glVertex2f(x - 5.0f, y + 5.0f);
		//glVertex2f(x + textWidth + 5.0f, y + 5.0f);
		//glVertex2f(x + textWidth + 5.0f, y - textHeight - 5.0f);
		//glVertex2f(x - 5.0f, y - textHeight - 5.0f);
		//glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


//*/