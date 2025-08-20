#pragma once
#include "glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>


#include <iostream>
#include <map>



#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"


//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp" // <- критически важно!

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H



const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    uniform float xOffset = 0.0f; // Смещение по X для позиционирования кости
    uniform float yOffset = 0.0f; 
    uniform float scale = 1.0f; 
    uniform float aspectRatio = 1.0f; // Cоотношение сторон
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos.x*scale + xOffset, aPos.y*scale*aspectRatio + yOffset, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D texture1;
    uniform bool highlight;
    uniform vec3 highlightColor = vec3(1.0, 1.0, 0.0);
    uniform float highlightIntensity; // Теперь это uniform-переменная
    uniform float time; // Время для анимации

    void main() {
        vec4 texColor = texture(texture1, TexCoord);
        if (highlight) {
            // Пульсирующая подсветка
            float pulse = (sin(time * 3.0) + 1.0) * 0.25 + 0.5;
            vec3 mixedColor = mix(texColor.rgb, highlightColor, highlightIntensity * pulse);
            FragColor = vec4(mixedColor, texColor.a);
        } else {
            FragColor = texColor;
        }
    }
)";

const char* ramcaVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    uniform mat4 projection;
    uniform mat4 model;

    out vec2 TexCoord;
    void main() {
        gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char* ramcaFragmentShaderSource = R"(
   #version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform vec4 borderColor;
uniform vec4 backgroundColor;
uniform vec4 _textColor = vec4(0.0, 1.0, 0.0, 1.0);
uniform float borderWidth;
uniform bool hasText;
uniform sampler2D textTexture;

uniform float padding = 0.1f;
uniform vec2 textureSize = vec2(1.5, 1.8);

// Параметры для линий
uniform bool drawDiagonals = false;       // Включить/выключить линии
uniform vec4 lineColor = vec4(1.0, 1.0, 0.0, 1.0); // Цвет линий (желтый по умолчанию)
uniform float lineWidth = 0.01f;          // Ширина линий
uniform float aspectRatio = 1.0f; // width/height

void main()
{
    const float textBottomPaddingY = -0.8; // поравочные смещение по y
    const float textBottomPaddingX = -0.45; // поравочные смещение по x

    // Определяем границы рамки
    // Определяем границы рамки
    bool isBorder = 
        TexCoord.x < borderWidth || 
        TexCoord.x > (1.0 - borderWidth) || 
        TexCoord.y < borderWidth || 
        TexCoord.y > (1.0 - borderWidth);
    
    if (isBorder) 
    {
        FragColor = borderColor;
        return;
    }
    
    // Координаты внутри рамки (без учета borderWidth)
      vec2 innerCoord = TexCoord ;

    // Проверяем диагонали только если они включены
    if (drawDiagonals) 
    {
        // Нормализованные координаты с учетом пропорций
        vec2 normCoord = vec2(innerCoord.x, innerCoord.y );         
        // Главная диагональ (0,0) to (1,1) в нормализованных координатах
        float mainDiag = abs(normCoord.x*0.8f + 0.16f - normCoord.y);      
        // Побочная диагональ (1,0) to (0,1) в нормализованных координатах
        float antiDiag = abs(normCoord.x*0.75f + normCoord.y*0.85f - 1.0);
       
        if (mainDiag < lineWidth || antiDiag < lineWidth) 
        {
            FragColor = lineColor;
            return;
        }
    }
    
    // Обработка текста и фона
    bool isPaddingArea = 
        innerCoord.x < padding ||
        innerCoord.x > (1.0 - padding) ||
        innerCoord.y < padding || 
        innerCoord.y > (1.0 - padding);
    
    if (!isPaddingArea && hasText) 
    {
        vec2 textCoords = (innerCoord - padding) / (1.0 - 2.0 * padding) * textureSize;
        textCoords.y += textBottomPaddingY; // textBottomPaddingY
        textCoords.x += textBottomPaddingX; // textBottomPaddingX
        
        if (textCoords.x >= 0.0 && textCoords.x <= 1.0 && 
            textCoords.y >= 0.0 && textCoords.y <= 1.0) 
        {
            float textAlpha = texture(textTexture, textCoords).r;
            FragColor = mix(backgroundColor, _textColor, textAlpha);
            return;
        }
    }
    
    FragColor = backgroundColor;
}
)";



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


class Shader
{
public:
    // Состояние
    GLuint ID;

      // Устанавливаем текущий шейдер в качестве активного
    Shader& Use() {
        glUseProgram(this->ID);
        return *this;
    }

    void Compile(const char* vertexSource, const char* fragmentSource)
    {
        // Вершинный шейдер
        GLuint sVertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
        // Фрагментный шейдер
        GLuint sFragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
      
        // Шейдерная программа
        this->ID = glCreateProgram();
        glAttachShader(this->ID, sVertex);
        glAttachShader(this->ID, sFragment);

        glLinkProgram(this->ID);
        GLint success;
        glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
            std::cerr << "Program linking error:\n" << infoLog << std::endl;
        }
        // Удаляем шейдеры, так как теперь они связаны с нашей программой и больше не нужны
        glDeleteShader(sVertex);
        glDeleteShader(sFragment);

    }

   

private:

    GLuint CompileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        }
        return shader;
    }

};


class Texture2D
{
public:
    // Содержит ID объекта текстуры, используемый для всех операций с конкретной текстурой
    GLuint ID;

    GLuint loadTexture(const char* path) 
    {
    
        int width, height, channels;
        unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
        if (!data) {
            return 0;
        }
        GLuint textureID;
        glGenTextures(1, &textureID);
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return textureID;
    }
};

class CubeView
{
public:

    GLuint shaderProgram;

    CubeView():VAO(0), VBO(0),EBO(0)
    {
        shader.Compile(vertexShaderSource, fragmentShaderSource);
        shaderProgram = shader.ID;
    }


    ~CubeView() 
    {
        Delete();
    }

    void Create()
    {
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
       
        std::cout << "Cube = " << shaderProgram << std::endl;
        setupBuffers();
    }

    void setupBuffers() const {
        
        const float baseSize = 0.2f; // Базовый размер (будет масштабироваться)

      
        float vertices[] = {
            // positions      // texture coords
            -baseSize, -baseSize,  0.0f, 1.0f,
             baseSize, -baseSize,  1.0f, 1.0f,
             baseSize,  baseSize,  1.0f, 0.0f,
            -baseSize,  baseSize,  0.0f, 0.0f
        };
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    void drawDice(GLuint textureID, float xOffset, float yOffset, bool highlight = false) const
    {     
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), xOffset);
        glUniform1f(glGetUniformLocation(shaderProgram, "yOffset"), yOffset);
        glUniform1i(glGetUniformLocation(shaderProgram, "highlight"), highlight);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Delete() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteProgram(shaderProgram);
    }

private:
   
    Shader shader;

    GLuint VAO, VBO, EBO;
};


class IObjectDraw
{
public:
    virtual void Draw() = 0;
    virtual ~IObjectDraw(){}
};


class ArrayCubeView: public IObjectDraw
{
    
public:

    ArrayCubeView(ArrayCube& _arrayCube): arrayCube(_arrayCube)
    {
        cubeView.Create();
    }

    void Create() 
    {
       
        loadDiceTextures();
        startTime = glfwGetTime();
    }

    void Draw()
    {

        GLuint shaderProgram = cubeView.shaderProgram;
        float currentTime = glfwGetTime() - startTime;
  
        glUseProgram(shaderProgram);
   
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), currentTime);
        glUniform1f(glGetUniformLocation(shaderProgram, "highlightIntensity"), 0.5f);

        float scale = 0.2f;
        glUniform1f(glGetUniformLocation(shaderProgram, "scale"), scale);

        // Рассчитываем параметры для размещения 6 костей
        float diceWidth = 0.4f * scale; // ширина одной кости (0.4 влево + 0.4 вправо)
        float gap = 0.005f; // промежуток между костями
        float totalWidth = 6 * diceWidth + 5 * gap; // общая ширина
        float startX = 1.0f - totalWidth + 0.05;
        float yOffset = 0.4f;
        float UpYOffset = 0.2f;

        std::vector<int>& diceValues = arrayCube.diceValues;
        std::vector<int>& stateUp = arrayCube.stateUp;
        int& highlightedDice = arrayCube.highlightedDice;
        // Рисуем 6 костей с правильными смещениями
        for (int i = 0; i < diceValues.size(); ++i) {
            float xOffset = startX + i * (diceWidth + gap);
            float _yOffset = yOffset + stateUp[i] * UpYOffset;
            cubeView.drawDice(diceValues[i], xOffset, _yOffset, i == highlightedDice);
        }
        glBindVertexArray(0); // Добавить
        glBindTexture(GL_TEXTURE_2D, 0); // Добавить
  
    }

   
    void Delete() {
        cubeView.Delete();
    }
   

private :


    void loadDiceTextures() {
        const std::vector<std::string> paths = {
            "textures/side_1_pips.png",
            "textures/side_2_pips.png",
            "textures/side_3_pips.png",
            "textures/side_4_pips.png",
            "textures/side_5_pips.png",
            "textures/side_6_pips.png"
        };

        for (const auto& path : paths) {
            GLuint texture = texture2D.loadTexture(path.c_str());
            if (texture != 0) {
                diceTextures.push_back(texture);
            }
        }
    }

    float startTime;
   
    CubeView cubeView;

    Texture2D texture2D;

    std::vector<GLuint> diceTextures;

    ArrayCube   &arrayCube;
   
};

class RamcaView : public IObjectDraw
{
public:

    GLuint shaderProgram;
    glm::mat4 *projection = nullptr;

    RamcaView():borderWidth(0.04f){

        shader.Compile(ramcaVertexShaderSource, ramcaFragmentShaderSource);
        shaderProgram = shader.ID;
    }

    ~RamcaView()
    {
        Delete();
    }

    void EnableDiagonals(bool enable) {
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "drawDiagonals"), enable ? 1 : 0);
    //    glUniform1f(glGetUniformLocation(shaderProgram, "aspectRatio"), width / height);
        glUseProgram(0);
    }

    void SetDiagonalColor(float r, float g, float b, float a = 1.0f) {
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram, "lineColor"), r, g, b, a);
        glUseProgram(0);
    }

    void SetDiagonalWidth(float width) {
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "lineWidth"), width);
        glUseProgram(0);
    }

    void Delete() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteProgram(shaderProgram);
    }

    void Create(float w, float h, float x = 0.0f, float y = 0.0f)
    {
        width = w;
        height = h;
        posX = x;
        posY = y;      
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     
        setupBuffers();
      
    }

  
    void Draw() {
        if (projection == nullptr) return;
        // Сохраняем текущие состояния
        GLint prevProgram;
        GLboolean prevBlend;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
        glGetBooleanv(GL_BLEND, &prevBlend);

        // Наш рендеринг
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(shaderProgram);
  
        // Устанавливаем матрицы
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Параметры рамки
        glUniform4f(glGetUniformLocation(shaderProgram, "borderColor"), 1.0f, 0.0f, 0.0f, 1.0f); // Красная рамка
        glUniform4f(glGetUniformLocation(shaderProgram, "backgroundColor"), 0.2f, 0.2f, 0.2f, 1.0f); // Серый фон
        glUniform1f(glGetUniformLocation(shaderProgram, "borderWidth"), borderWidth); // Ширина рамки 5%

        // Текст
        glUniform1i(glGetUniformLocation(shaderProgram, "hasText"), textTexture ? 1 : 0);
        if (textTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textTexture);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisable(GL_BLEND);

       glBindTexture(GL_TEXTURE_2D, 0);

       glBindVertexArray(0);

       // Восстанавливаем состояния
       if (!prevBlend) glDisable(GL_BLEND);
       glUseProgram(prevProgram);

    }

    void SetBorderWidth(float _borderWidth) {
        borderWidth = _borderWidth;
    }

public:

    void setupBuffers() {
        // Создаем прямоугольник
        float vertices[] = {
            // positions   // texCoords
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f
        };
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    Shader shader;

    GLuint VAO, VBO, EBO;

    GLuint textTexture;
    float width, height;
    float posX, posY;
    float borderWidth;

};



class GridView {
public:
    std::vector<std::vector<RamcaView>> grid;
    int cols, rows;
    float cellWidth, cellHeight;
    float paddingX, paddingY;
    glm::mat4* projection = nullptr;

    // Новый конструктор с явным указанием размеров сетки
    GridView(int cols, int rows,
        float cellWidth, float cellHeight,
        float paddingX = 0.02f, float paddingY = 0.02f)
        : cols(cols), rows(rows),
        cellWidth(cellWidth), cellHeight(cellHeight),
        paddingX(paddingX), paddingY(paddingY)
    {
        grid.resize(rows);
        for (int y = 0; y < rows; ++y) {
            grid[y].resize(cols);
            for (int x = 0; x < cols; ++x) {
                float posX = x * (cellWidth + paddingX);
                float posY = y * (cellHeight + paddingY);
                grid[y][x].Create(cellWidth, cellHeight, posX, posY);
            }
        }
    }

    void Draw() {
        if (projection == nullptr) return;
        for (auto& row : grid) {
            for (auto& cell : row) {
                cell.projection = projection;
                cell.Draw();
            }
        }
    }

    void Delete() {
        for (auto& row : grid) {
            for (auto& cell : row) {
                cell.Delete();
            }
        }
    }

};


class SimpleText
{
public:

    SimpleText()
    {
        ft = nullptr;
        face = nullptr;
        // Инициализация FreeType
        if (FT_Init_FreeType(&ft)) {
            std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            //    return 0;
        }
    }

    ~SimpleText()
    {
        // Очищаем ресурсы FreeType
        if( face )    FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }


    GLuint CreateTextTexture(const std::wstring& text) {

        // 1. Рассчитываем размеры текстуры
        int textureWidth = 0;
        int textureHeight = 0;

        // Проходим по всем символам строки для вычисления размеров
        for (wchar_t c : text) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT)) {
                std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
                continue;
            }

            textureWidth += face->glyph->bitmap.width;
            textureHeight = std::max(textureHeight, (int)face->glyph->bitmap.rows);
        }

        // Добавляем небольшой отступ между символами
        textureWidth += text.length() * 2;
      //  textureHeight += 30;

         // 2. Создаём временный буфер для текстуры
        std::vector<unsigned char> textureData(textureWidth * textureHeight, 0);

        // 3. Рендерим каждый символ в буфер
        int xOffset = 0;
        for (wchar_t c : text) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT)/*FT_Load_Char(face, c, FT_LOAD_RENDER)*/) {
                continue;
            }

            FT_Bitmap bitmap = face->glyph->bitmap;

            for (unsigned int y = 0; y < bitmap.rows; y++) {
                unsigned int flipped_y = bitmap.rows - 1 - y;
                for (unsigned int x = 0; x < bitmap.width; x++) {
                    textureData[(flipped_y * textureWidth) + xOffset + x] =
                        bitmap.buffer[y * bitmap.width + x];
                }
            }
            xOffset += bitmap.width + 2; // +2 пикселя отступ
        }

        // 4. Создаём текстуру OpenGL
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Загружаем данные в текстуру
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            textureWidth,
            textureHeight,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            textureData.data()
        );

        // Устанавливаем параметры текстуры
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Очищаем ресурсы FreeType
     /*   FT_Done_Face(face);
        face = nullptr;*/
        return textureID;
    }

    int SetTextParametr(const char* fontPath = "arial.ttf", int fontSize = 8)
    {
        if (face) FT_Done_Face(face);
        if (FT_New_Face(ft, fontPath, 0, &face)) {
            std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
            FT_Done_FreeType(ft);
            return 0;
        }
        // Установка размера шрифта
     //   FT_Set_Pixel_Sizes(face, 0, fontSize);
          // Установка размера в пунктах (1/72 дюйма)
        FT_Set_Char_Size(face, 0, fontSize * 64, 96, 96); // 96 DPI

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        return 1;
    }
private:
    FT_Library ft;
    // Загрузка шрифта
    FT_Face face;
};


class DoubleGridView : public IObjectDraw
{
public:
    GridView leftGrid;
    GridView rightGrid;
    float gapSize; // Размер зазора между сетками
    BoardGame* boardGameLeft = nullptr;
    BoardGame* boardGameRight = nullptr;

    DoubleGridView(SimpleText& _text, float cellWidth, float cellHeight,
        float gap = 0.01f,  // Добавляем параметр зазора
        float paddingX = 0.05f, float paddingY = 0.02f)
        :text(_text), leftGrid(7, 16, cellWidth, cellHeight, paddingX, paddingY),
        rightGrid(7, 16, cellWidth, cellHeight, paddingX, paddingY),
        gapSize(gap)
    {
        float singleGridWidth = cellWidth * 7 + paddingX * 6;

        const float offsetX = 0.0, offsetY = -0.2;
        // Позиционируем левую сетку с учетом зазора
        for (auto& row : leftGrid.grid) {
            for (auto& cell : row) {
                cell.posX += offsetX;
                cell.posY += offsetY;
            }
        }
        // Позиционируем правую сетку с учетом зазора
        for (auto& row : rightGrid.grid) {
            for (auto& cell : row) {
                cell.posX += offsetX + singleGridWidth + gapSize;
                cell.posY += offsetY;
            }
        }

        text.SetTextParametr("fonts/arial.ttf", 48);
    }

    void SetProjection(glm::mat4 &projection) 
    {
        leftGrid.projection = &projection;
        rightGrid.projection = &projection;
    }

    void FillText() {
   
        FillSymbol(leftGrid);
        FillSymbol(rightGrid);
      
    }

    void FillBoardText() {
        if (boardGameLeft != nullptr) FillBoard(leftGrid, *boardGameLeft);
        if (boardGameRight != nullptr) FillBoard(rightGrid, *boardGameRight);
    }


    // Отрисовка обеих сеток
    void Draw() {
      
        leftGrid.Draw();
        rightGrid.Draw();
      
    }

    // Очистка ресурсов
    void Delete() {
        leftGrid.Delete();
        rightGrid.Delete();
    }



    private:
    
        void FillSymbol(GridView &grid)
        {
            int i = 0;
            for (; i < 6; ++i) {
                auto& view = grid.grid[15 - i][0];
                view.textTexture = text.CreateTextTexture(std::to_wstring(i + 1));
            }
            std::wstring arrSymb[10]= { L"P", L"D", L"T", L"F", L"e", L"S", L"C", L"A", L"Sm",L"Pr"};
            for (int j = 0; i < 16; ++i, ++j) {
                auto& view = grid.grid[15 - i][0];
                view.textTexture = text.CreateTextTexture(arrSymb[j]);
            }
        }

        void FillBoard(GridView& grid, BoardGame& board)
        {
          
            for (int i = 0; i < BoardGame::N_ROW; ++i) {
                for (int j = 0; j < BoardGame::N_COLUMN; ++j) {
                    if (board.arr[i][j] == 0) continue;
                    auto& view = grid.grid[15 - i][j+1];
                    if (board.arr[i][j] != INT_MAX) {
                        view.textTexture = text.CreateTextTexture(std::to_wstring(board.arr[i][j]));
                    }
                    else {
                        view.EnableDiagonals(true); // Включить линии
                        view.SetDiagonalColor(1.0f, 0.0f, 0.0f, 1.0f); // Красные линии
                        view.SetDiagonalWidth(0.02f); // Более тонкие линии
                    }
                }
            }
        }

        SimpleText &text;
    
};

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

class Text
{
public:

    Text()
    {
        Shader shader;
        shader.Compile(_vertexShaderSource, _fragmentShaderSource);
        shaderProgram = shader.ID;

        initializeTextRendering();
    }

    ~Text()
    {
        for (auto& c : Characters) {
            glDeleteTextures(1, &c.second.TextureID);
        }
    }

    void RenderText(std::wstring text, float x, float y, float scale, glm::vec3 color)
    {
        // Сохраняем состояния
        GLint prevProgram;
        GLboolean prevBlend;
        GLint prevVAO;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
        glGetBooleanv(GL_BLEND, &prevBlend);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);

        // Наш рендеринг
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

         // Восстанавливаем состояния
    if (!prevBlend) glDisable(GL_BLEND);
    glUseProgram(prevProgram);
    glBindVertexArray(prevVAO);
    }

private:


    struct Character {
        GLuint TextureID;
        glm::ivec2 Size;      // Размер символа
        glm::ivec2 Bearing;   // Смещение от базовой линии
        GLuint Advance;       // Смещение до следующего символа
    };

    std::map<wchar_t, Character> Characters;

    GLuint VAO, VBO;
    GLuint shaderProgram;

    void initializeTextRendering()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
            std::cerr << "Failed to load font\n";
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, 48);

        LoadCharacters(face);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    }

  
    void LoadCharacters(FT_Face face)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Загружаем базовый набор ASCII (0-127)
        for (unsigned char c = 0; c < 128; c++) {
            LoadCharacter(face, c);
        }

        // Загружаем кириллицу (Unicode диапазон для русских букв)
        // Это коды символов от U+0400 до U+04FF
        for (wchar_t c = 0x0400; c <= 0x04FF; c++) {
            LoadCharacter(face, c);
        }
    }

    void LoadCharacter(FT_Face face, wchar_t c)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load Glyph " << (int)c << std::endl;
            return;
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

        Characters.insert(std::pair<wchar_t, Character>(c, character));
    }
};


class PlayerName : public IObjectDraw
{
public:
    std::wstring name;
    float x;
    float y;
    float scale;

    PlayerName(Text& _txt) 
        : txt(_txt),x(25.0f), y(HEIGHT - 50.0f), scale(0.5f)
    {
    }
 
    void Draw()
    {
        txt.RenderText(name, x, y, scale, glm::vec3(1.0f, 1.0f, 0.4f));
    }

private:
    Text &txt;
};

class CurrentStepNumber : public IObjectDraw
{
public:
    Game& game;
    float x;
    float y;
    float scale;

    CurrentStepNumber(Text& _txt, Game &_game)
        : txt(_txt),game(_game), x(25.0f), y(HEIGHT - 50.0f), scale(0.5f)
    {
    }

    void Draw()
    {
        txt.RenderText(std::to_wstring(game.currStep), x, y, scale, glm::vec3(1.0f, 1.0f, 0.4f));
    }

private:
    Text& txt;
};

struct ParamGrid
{
    int row, col;
    float cellWidth, cellHeight;
    float paddingX = 0.0f;
    float paddingY = 0.02f;
};


class TableResult : public IObjectDraw
{
public:
    TableResult(SimpleText& _text, ParamGrid &param, Game &_game)
        : text(_text),grid(param.col, param.row, param.cellWidth, param.cellHeight, param.paddingX, param.paddingY), game(_game)
    {
        text.SetTextParametr("fonts/arial.ttf", 48);
    }

    void SetProjection(glm::mat4& projection)
    {
        grid.projection = &projection;
    }

    void FillStaticText(const std::wstring &name1 = L"1", const std::wstring& name2 = L"2") {
        auto& view = grid.grid[2][1];
        view.textTexture = text.CreateTextTexture(L"СУММА");
        auto& view2 = grid.grid[2][2];
        view2.textTexture = text.CreateTextTexture(L"ШКОЛА");
        auto& view3 = grid.grid[1][0];
        view3.textTexture = text.CreateTextTexture(name1);
        auto& view4 = grid.grid[0][0];
        view4.textTexture = text.CreateTextTexture(name2);
    }

    void FillCeillText() {
        std::vector<Payer>& vPaers = game.vPaers;
        int n = vPaers.size();
        for (int i = 0; i < n; ++i) {
            auto& view = grid.grid[n - 1 - i][1];
            view.textTexture = text.CreateTextTexture(std::to_wstring(vPaers[i].boardGame.sumTotal));
            auto& view2 = grid.grid[n - 1 - i][2];
            view2.textTexture = text.CreateTextTexture(std::to_wstring(vPaers[i].boardGame.sumScool));
        }

    }

    void SetStartPos(float x, float y) 
    {
        for (auto& row : grid.grid) {
            for (auto& cell : row) {
                cell.posX += x;
                cell.posY += y;
            }
        }
    }

    // Отрисовка обеих сеток
    void Draw() {
       // FillCeillText();
        grid.Draw();
    }

    // Очистка ресурсов
    void Delete() {
        grid.Delete();
    }

private:

    GridView grid;
    SimpleText& text;
    Game& game;
};
