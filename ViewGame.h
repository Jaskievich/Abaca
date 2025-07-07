#pragma once
#include "glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>


#include <iostream>
#include <map>
#include <string>


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
   uniform vec2 textureSize =  vec2(1.5, 1.8);
    
    void main() 
   {

        const float textBottomPaddingY = -0.8; // поравочные смещение по y
        const float textBottomPaddingX = -0.2; // поравочные смещение по x

        // Определяем границы рамки
        bool isBorder = 
            TexCoord.x < borderWidth || 
            TexCoord.x > (1.0 - borderWidth) || 
            TexCoord.y < borderWidth || 
            TexCoord.y > (1.0 - borderWidth);
        
        if (isBorder) 
        {
            FragColor = borderColor;
        } 
        else 
        {                
                bool isPaddingArea = 
                    TexCoord.x < (borderWidth + padding) ||
                    TexCoord.x > (1.0 - borderWidth - padding) ||
                    TexCoord.y < (borderWidth + padding) || 
                    TexCoord.y > (1.0 - borderWidth - padding);
        
                // Масштабируем текстурные координаты для текста с учетом паддинга
               vec2 textCoords = (TexCoord - vec2(borderWidth + padding, borderWidth + padding)) /
                                    (1.0 - 2.0 * vec2(borderWidth + padding, borderWidth + padding))*textureSize; 
            
               textCoords.y += textBottomPaddingY;
               textCoords.x += textBottomPaddingX;

            // Корректируем вертикальное положение (центрируем)
      
      
                if (!isPaddingArea && textCoords.x >= 0.0 && textCoords.x <= 1.0 && 
                             textCoords.y >= 0.0 && textCoords.y <= 1.0)
                {
                    // Получаем альфа-значение (непрозрачность) из текстуры шрифта.
                    // Обычно для монохромных шрифтов используется красный канал.
                    float textAlpha = texture(textTexture, textCoords).r;

                    // Смешиваем цвет фона с цветом текста, используя альфа-канал текста.
                    // Формула: mix(фон, передний_план, альфа)
                    FragColor = mix(backgroundColor, _textColor, textAlpha);
 
                } 
                else 
                {
                    FragColor = backgroundColor;              
                }
        }
    }
)";

//const char* ramcaFragmentShaderSource = R"(
//   #version 330 core
//in vec2 TexCoord;
//in vec2 WorldPos;
//out vec4 FragColor;
//
//// Параметры стиля
//uniform vec4 borderColor;
//uniform vec4 backgroundColor;
//uniform vec4 textColor = vec4(0.0, 1.0, 0.0, 1.0);
//uniform float borderWidth = 0.05;
//uniform float padding = 0.2;
//
//// Параметры текста
//uniform sampler2D textTexture;
//uniform vec2 textureSize = vec2(1.0);
//uniform vec2 textOffset = vec2(-0.1, -0.3); // Смещение текста
//uniform float textScale = 1.0; // Масштаб текста
//uniform float textVerticalOffset = 1.0;
////uniform float textDescent = 0.0;
//
//// Параметры SDF-рендеринга (опционально)
//uniform bool useSDF = false;
//uniform float sdfEdge = 0.5;
//uniform float sdfOutline = 0.1;
//
//void main() {
//    // 1. Рассчитываем расстояния до границ
//    vec2 borderDist = min(TexCoord, 1.0 - TexCoord);
//    float borderFactor = smoothstep(0.0, borderWidth, min(borderDist.x, borderDist.y));
//    
//    // 2. Рассчитываем расстояния до внутренней границы
//    vec2 paddingBorder = vec2(borderWidth + padding);
//    vec2 innerDist = min(TexCoord - borderWidth, 1.0 - borderWidth - TexCoord);
//    float paddingFactor = smoothstep(0.0, padding, min(innerDist.x, innerDist.y));
//
//    // 3. Определяем цвет рамки и фона
//    vec4 color = mix(borderColor, backgroundColor, borderFactor);
//    
//    // 4. Обработка текста (только в центральной области)
//    if (paddingFactor > 0.0) {
//        // Масштабированные координаты текста
//        vec2 textCoords = (TexCoord - paddingBorder) / 
//                         (1.0 - 2.0 * paddingBorder);
//        // Учет смещения и масштаба
//        textCoords = (textCoords - 0.5) * textScale + 0.5 + textOffset;
//
//        textCoords.y = (textCoords.y - 0.5) * (1.0 + textVerticalOffset) + 0.5;
//        textCoords *= textureSize;
//        
//        if (all(greaterThanEqual(textCoords, vec2(0))) && 
//            all(lessThanEqual(textCoords, vec2(1)))) {
//            
//            if (useSDF) {
//                // SDF-рендеринг для четкого текста
//                float sdfValue = texture(textTexture, textCoords).r;
//                float alpha = smoothstep(sdfEdge - sdfOutline, sdfEdge + sdfOutline, sdfValue);
//                color = mix(color, textColor, alpha);
//            } else {
//                // Обычный рендеринг текста
//                float textAlpha = texture(textTexture, textCoords).r;
//                color = mix(color, textColor, textAlpha);
//            }
//        }
//    }
//
//    FragColor = color;
//}
//)";
//

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

    //// Размеры текстурного изображения
    //unsigned int Width, Height; // ширина и высота (в пикселях) загруженного изображения

    //// Формат текстуры
    //unsigned int Internal_Format; // внутренний формат текстуры
    //unsigned int Image_Format; // формат загружаемого изображения

    //// Конфигурация текстуры
    //unsigned int Wrap_S; // режим наложения по оси S
    //unsigned int Wrap_T; // режим наложения по оси T
    //unsigned int Filter_Min; // режим фильтрации, если пикселей текстуры < пикселей экрана
    //unsigned int Filter_Max; // режим фильтрации, если пикселей текстуры > пикселей экрана

    //Texture2D(): Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
    //{
    //    glGenTextures(1, &this->ID);
    //}

    //// Генерируем текстуру из изображения
    //void Generate(unsigned int width, unsigned int height, unsigned char* data)
    //{
    //    this->Width = width;
    //    this->Height = height;

    //    // Создаем текстуру
    //    glBindTexture(GL_TEXTURE_2D, this->ID);
    //    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);

    //    // Задаем для текстуры режимы наложения и фильтрации
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

    //    // Отвязываем текстуру
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //}

    //// Связываем текстуру в виде текущего активного GL_TEXTURE_2D текстурного объекта
    //void Bind() const
    //{
    //    glBindTexture(GL_TEXTURE_2D, this->ID);
    //}

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

    CubeView() {
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

    void setupBuffers() {
        
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


    void drawDice(GLuint textureID, float xOffset, float yOffset, bool highlight = false)
    {     
     //   glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "xOffset"), xOffset);
        glUniform1f(glGetUniformLocation(shaderProgram, "yOffset"), yOffset);
        glUniform1i(glGetUniformLocation(shaderProgram, "highlight"), highlight);
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

const int N_COUNT_DICE = 6;

class ArrayCubeView
{
    
public:

    std::vector<int> diceValues;
    std::vector<int> stateUp;

    ArrayCubeView()
    {
        diceValues.resize(N_COUNT_DICE, 1);
        stateUp.resize(N_COUNT_DICE, 0);
        cubeView.Create();
    }

    void Create() 
    {
       
        loadDiceTextures();
        startTime = glfwGetTime();
    }

    void Draw( int highlightedDice )
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
        //  float startX = -totalWidth / 2.0f + diceWidth / 2.0f; // начальное смещение
        float startX = 1.0f - totalWidth + 0.02;
        float yOffset = 0.4f;
        float UpYOffset = 0.2f;

        // Рисуем 6 костей с правильными смещениями
        for (int i = 0; i < diceValues.size(); ++i) {
            float xOffset = startX + i * (diceWidth + gap);
            float _yOffset = yOffset + stateUp[i] * UpYOffset;
            cubeView.drawDice(diceValues[i], xOffset, _yOffset, i == highlightedDice);
        }
        glBindVertexArray(0); // Добавить
        glBindTexture(GL_TEXTURE_2D, 0); // Добавить
  
    }

    void SetDiceRandom() 
    {
        for (int i = 0; i < N_COUNT_DICE; ++i) {
            if (stateUp[i] == 0)
                diceValues[i] = rollDice();
        }
    }

    void Delete() {
        cubeView.Delete();
    }
   

private :



    int rollDice() {
        return rand() % 6 + 1;
    }

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
   
};

class RamcaView
{
public:

    GLuint shaderProgram;

    RamcaView() {

        shader.Compile(ramcaVertexShaderSource, ramcaFragmentShaderSource);
        shaderProgram = shader.ID;
    }

    ~RamcaView()
    {
        Delete();
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

    void DeleteView() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteProgram(shaderProgram);
    }

    void Draw(glm::mat4 projection) {

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

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Параметры рамки
        glUniform4f(glGetUniformLocation(shaderProgram, "borderColor"), 1.0f, 0.0f, 0.0f, 1.0f); // Красная рамка
        glUniform4f(glGetUniformLocation(shaderProgram, "backgroundColor"), 0.2f, 0.2f, 0.2f, 1.0f); // Серый фон
        glUniform1f(glGetUniformLocation(shaderProgram, "borderWidth"), 0.05f); // Ширина рамки 5%

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


};

//
//class RamcaView
//{
//public:
//
//    float posX, posY;
//    enum class TextAlignment {
//        LEFT,    // По левому краю
//        CENTER,  // По центру
//        RIGHT    // По правому краю
//    };
//   
//    GLuint shaderProgram;
//
//    RamcaView() {
//        shader.Compile(ramcaVertexShaderSource, ramcaFragmentShaderSource);
//        // Текстовый шейдер
//        textShader.Compile(_vertexShaderSource, _fragmentShaderSource);
//        shaderProgram = shader.ID;
//        textShaderProgram = textShader.ID;
//        initializeTextRendering();
//    }
//
//    ~RamcaView() {
//        Delete();
//    }
//
//    void Delete() {
//        glDeleteVertexArrays(1, &VAO);
//        glDeleteBuffers(1, &VBO);
//        glDeleteBuffers(1, &EBO);
//        glDeleteProgram(shaderProgram);
//        if (texture) glDeleteTextures(1, &texture);
//        if (textVAO) glDeleteVertexArrays(1, &textVAO);
//        if (textVBO) glDeleteBuffers(1, &textVBO);
//    }
//
//    void Create(float w, float h, float x = 0.0f, float y = 0.0f) {
//        width = w;
//        height = h;
//        posX = x;
//        posY = y;
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glGenBuffers(1, &EBO);
//        glBindVertexArray(VAO);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//        setupBuffers();
//    }
//
//    void DeleteView() {
//        glDeleteVertexArrays(1, &VAO);
//        glDeleteBuffers(1, &VBO);
//        glDeleteBuffers(1, &EBO);
//        glDeleteProgram(shaderProgram);
//        if ( texture) glDeleteTextures(1, &texture);
//    }
//
//    void Draw(glm::mat4 projection) {
//        glUseProgram(shaderProgram);
//
//        // Настройка матриц преобразования
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
//        model = glm::scale(model, glm::vec3(width, height, 1.0f));
//
//        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
//
//        // Параметры рамки и фона
//        glUniform4f(glGetUniformLocation(shaderProgram, "borderColor"), borderColor.r, borderColor.g, borderColor.b, borderColor.a);
//        glUniform4f(glGetUniformLocation(shaderProgram, "backgroundColor"), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
//        glUniform1f(glGetUniformLocation(shaderProgram, "borderWidth"), borderWidth);
//
//        // Отрисовка текста
//        glUniform1i(glGetUniformLocation(shaderProgram, "hasText"), texture ? 1 : 0);
//        if (texture) {
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, texture);
//        }
//
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//        // Отрисовка текста, если он есть
//        if (!textString.empty()) {
//            renderText();
//        }
//
//        glDisable(GL_BLEND);
//        glBindTexture(GL_TEXTURE_2D, 0);
//        glBindVertexArray(0);
//    }
//
//    // Методы для работы с текстом
//    void SetText(const std::string& text, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f)) {
//        textString = text;
//        textScale = scale;
//        textColor = color;
//        updateTextTexture();
//    }
//
//    void SetTextPosition(float x, float y) {
//        textPosX = x;
//        textPosY = y;
//    }
//
//    void SetTextAlignment(TextAlignment alignment) {
//        textAlignment = alignment;
//    }
//
//    void SetBorderColor(glm::vec4 color) {
//        borderColor = color;
//    }
//
//    void SetBackgroundColor(glm::vec4 color) {
//        bgColor = color;
//    }
//
//    void SetBorderWidth(float width) {
//        borderWidth = width;
//    }
//
//private:
//   
//
//    void initializeTextRendering() {
//
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        // Инициализация FreeType и загрузка шрифта
//        if (FT_Init_FreeType(&ft)) {
//            std::cerr << "ERROR::FREETYPE: Не удалось инициализировать FreeType" << std::endl;
//            return;
//        }
//
//        if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
//            std::cerr << "ERROR::FREETYPE: Не удалось загрузить шрифт" << std::endl;
//            return;
//        }
//
//        FT_Set_Pixel_Sizes(face, 0, 48);
//
//        LoadCharacters(face);
//
//        FT_Done_Face(face);
//        FT_Done_FreeType(ft);
//
//        // Настройка VAO/VBO для текста
//        glGenVertexArrays(1, &textVAO);
//        glGenBuffers(1, &textVBO);
//        glBindVertexArray(textVAO);
//        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//
//
//        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
//        glUseProgram(textShaderProgram);
//        glUniformMatrix4fv(glGetUniformLocation(textShaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
//    }
//
//
//    void LoadCharacters(FT_Face face) {
//
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//        for (unsigned char c = 0; c < 128; c++) {
//            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
//                std::cerr << "Failed to load Glyph " << c << std::endl;
//                continue;
//            }
//
//         //   GLuint texture;
//            glGenTextures(1, &texture);
//            glBindTexture(GL_TEXTURE_2D, texture);
//            glTexImage2D(
//                GL_TEXTURE_2D,
//                0,
//                GL_RED,
//                face->glyph->bitmap.width,
//                face->glyph->bitmap.rows,
//                0,
//                GL_RED,
//                GL_UNSIGNED_BYTE,
//                face->glyph->bitmap.buffer
//            );
//
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//            Character character = {
//                texture,
//                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
//                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//                static_cast<GLuint>(face->glyph->advance.x)
//            };
//
//            Characters.insert(std::pair<char, Character>(c, character));
//        }
//        glBindTexture(GL_TEXTURE_2D, 0);
//    }
//
//    void renderText() {
//
// 
//        glUseProgram(textShaderProgram);
//        glUniform3f(glGetUniformLocation(textShaderProgram, "textColor"), textColor.x, textColor.y, textColor.z);
//        glActiveTexture(GL_TEXTURE0);
//        glBindVertexArray(textVAO);
//
//        // Вычисляем позицию текста в зависимости от выравнивания
//        float x = textPosX;
//        float y = textPosY;
//
//        if (textAlignment != TextAlignment::LEFT) {
//            float textWidth = 0.0f;
//            for (std::string::const_iterator c = textString.begin(); c != textString.end(); c++) {
//                Character ch = Characters[*c];
//                textWidth += (ch.Advance >> 6) * textScale;
//            }
//
//            if (textAlignment == TextAlignment::CENTER) {
//                x -= textWidth / 2.0f;
//            }
//            else if (textAlignment == TextAlignment::RIGHT) {
//                x -= textWidth;
//            }
//        }
//
//        // Отрисовываем каждый символ
//        for (std::string::const_iterator c = textString.begin(); c != textString.end(); c++) {
//            Character ch = Characters[*c];
//
//            float xpos = x + ch.Bearing.x * textScale;
//            float ypos = y - (ch.Size.y - ch.Bearing.y) * textScale;
//
//            float w = ch.Size.x * textScale;
//            float h = ch.Size.y * textScale;
//
//            // Обновляем VBO для каждого символа
//            float vertices[6][4] = {
//                { xpos,     ypos + h,   0.0f, 0.0f },
//                { xpos,     ypos,       0.0f, 1.0f },
//                { xpos + w, ypos,       1.0f, 1.0f },
//
//                { xpos,     ypos + h,   0.0f, 0.0f },
//                { xpos + w, ypos,       1.0f, 1.0f },
//                { xpos + w, ypos + h,   1.0f, 0.0f }
//            };
//
//            // Отрисовываем текстуру глифа на кваде
//            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//
//            // Обновляем VBO
//            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
//            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//         //   glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//            // Отрисовываем квад
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//
//            // Перемещаем курсор для следующего символа
//            x += (ch.Advance >> 6) * textScale;
//        }
//
//        glBindVertexArray(0);
//        glBindTexture(GL_TEXTURE_2D, 0);
//    }
//
//    void updateTextTexture() {
//        // Здесь можно реализовать рендеринг текста в текстуру
//    }
//
//    void setupBuffers() {
//        float vertices[] = {
//            // позиции    // текстурные координаты
//            -1.0f, -1.0f, 0.0f, 0.0f,
//             1.0f, -1.0f, 1.0f, 0.0f,
//             1.0f,  1.0f, 1.0f, 1.0f,
//            -1.0f,  1.0f, 0.0f, 1.0f
//        };
//
//        unsigned int indices[] = {
//            0, 1, 2,
//            2, 3, 0
//        };
//
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//        glEnableVertexAttribArray(1);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//    }
//
//private:
//    // Компоненты для рендеринга
//    Shader shader;
//    Shader textShader;      // Специально для текста
//    GLuint VAO, VBO, EBO;
//  //  GLuint textTexture;
//    GLuint texture;
//
//    // Компоненты для отрисовки текста
//    struct Character {
//        GLuint TextureID;    // ID текстуры глифа
//        glm::ivec2 Size;     // Размер глифа
//        glm::ivec2 Bearing;  // Смещение от базовой линии
//        GLuint Advance;      // Горизонтальное смещение до следующего глифа
//    };
//
//    std::map<char, Character> Characters;
//    GLuint textVAO, textVBO;
//    GLuint textShaderProgram;
//    FT_Library ft;
//    FT_Face face;
//
//    // Свойства вида
//    float width, height;
//  
//
//    // Свойства текста
//    std::string textString;
//    float textScale = 1.0f;
//    glm::vec3 textColor;
//    float textPosX = 0.0f, textPosY = 0.0f;
//
//    // Рамка и фон
//    glm::vec4 borderColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//    glm::vec4 bgColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
//    float borderWidth = 0.05f;
//
//    TextAlignment textAlignment = TextAlignment::LEFT;
//};
//
//

class GridView {
public:
    std::vector<std::vector<RamcaView>> grid;
    int cols, rows;
    float cellWidth, cellHeight;
    float paddingX, paddingY;

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

    void Draw(glm::mat4 projection) {
        for (auto& row : grid) {
            for (auto& cell : row) {
                cell.Draw(projection);
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


    GLuint CreateTextTexture(const std::string& text) {


        // 1. Рассчитываем размеры текстуры
        int textureWidth = 0;
        int textureHeight = 0;

        // Проходим по всем символам строки для вычисления размеров
        for (char c : text) {
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
        for (char c : text) {
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


class DoubleGridView {
public:
    GridView leftGrid;
    GridView rightGrid;
    float gapSize; // Размер зазора между сетками

    DoubleGridView(float cellWidth, float cellHeight,
        float gap = 0.05f,  // Добавляем параметр зазора
        float paddingX = 0.02f, float paddingY = 0.02f)
        : leftGrid(7, 16, cellWidth, cellHeight, paddingX, paddingY),
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

    void FillText() {
   
        FillSymbol(leftGrid);
        FillSymbol(rightGrid);
      
    }


    // Отрисовка обеих сеток
    void Draw(glm::mat4 projection) {

        leftGrid.Draw(projection);
        rightGrid.Draw(projection);
      
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
                view.textTexture = text.CreateTextTexture(std::to_string(i + 1));
            }
            std::string arrSymb[10]= { "P", "D", "T", "F", "e", "S", "C", "A", "Sm","Pr"};
            for (int j = 0; i < 16; ++i, ++j) {
                auto& view = grid.grid[15 - i][0];
                view.textTexture = text.CreateTextTexture(arrSymb[j]);
            }
        }


        SimpleText text;
    
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

    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
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

    std::map<GLchar, Character> Characters;

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
};