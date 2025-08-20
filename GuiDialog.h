#pragma once
#include <GLFW/glfw3.h>
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "MyUtility.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>


struct ImgGuiUtility
{
    // Функция инициализации ImGui
    static void initImGui(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Загрузка шрифта с кириллицей
        ImFont* font = io.Fonts->AddFontFromFileTTF(
            "fonts/arial.ttf",
            18.0f,
            nullptr,
            io.Fonts->GetGlyphRangesCyrillic()
        );
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
        ImGui::StyleColorsDark();
    }

    // Функция очистки
    static void cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};


// Состояние диалогового окна
struct DialogState {
    char input1[256] = u8"p1";
    char input2[256] = u8"p2";
    bool showDialog = true;
    bool okPressed = false;
    bool cancelPressed = false;


    // Простой кроссплатформенный метод
    std::wstring getInput1AsWString() const {
        return MyUtility::simpleCharToWString(input1);
    }

    std::wstring getInput2AsWString() const {
        return MyUtility::simpleCharToWString(input2);
    }

};


class DialogInput
{
public:
  
    DialogState dialogState;
    // В классе DialogInput добавьте:
    bool showModalDialog(GLFWwindow* window) {
        dialogState.showDialog = true;
        dialogState.okPressed = false;
        dialogState.cancelPressed = false;

        while (!glfwWindowShouldClose(window) && dialogState.showDialog) {
            glfwPollEvents();
            renderDialogOnly();
            glfwSwapBuffers(window);
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        return dialogState.okPressed;
    }


    void loadFontsSimple(float fontSize = 18.0f) {
        ImGuiIO& io = ImGui::GetIO();

        //// Очищаем существующие шрифты (осторожно!)
        //io.Fonts->Clear();
        // Просто добавляем шрифт без очистки существующих
        font = io.Fonts->AddFontFromFileTTF(
            "fonts/arial.ttf",
            fontSize,
            nullptr,
            io.Fonts->GetGlyphRangesCyrillic()
        );
      
        // Устанавливаем как шрифт по умолчанию
        if (font != nullptr) {
            //// Перестраиваем атлас текстур
            io.Fonts->Build();
            // Обновляем текстуры шрифтов в рендерере
            ImGui_ImplOpenGL3_DestroyDeviceObjects();
            ImGui_ImplOpenGL3_CreateDeviceObjects();
            io.FontDefault = font;
        }
    }

private:

    void renderDialog() {
        if (!dialogState.showDialog)
            return;

        // Получаем размер viewport (области отрисовки ImGui)
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Устанавливаем позицию в центре экрана
        ImVec2 center = viewport->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        // Принудительно устанавливаем размер БЕЗ условий
        ImGui::SetNextWindowSize(ImVec2(600, 250));

        // Флаги окна - убираем resize и scrollbar
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoMove;

        ImGui::Begin(u8"Введите игроков", &dialogState.showDialog, flags);

        // Увеличиваем масштаб содержимого
        ImGui::SetWindowFontScale(1.3f);

        // Получаем доступный размер области содержимого
        ImVec2 contentSize = ImGui::GetContentRegionAvail();

        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None;
        // Текстовые поля ввода - адаптивная ширина
        ImGui::Text(u8"Игрок 1:");
        ImGui::SetNextItemWidth(contentSize.x - 20); // С запасом
        
        ImGui::InputText("##input1", dialogState.input1, IM_ARRAYSIZE(dialogState.input1), inputFlags);

        ImGui::Text(u8"Игрок 2:");
        ImGui::SetNextItemWidth(contentSize.x - 20);
        ImGui::InputText("##input2", dialogState.input2, IM_ARRAYSIZE(dialogState.input2), inputFlags);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Центрируем кнопки
        float buttonWidth = 150;
        float totalButtonsWidth = buttonWidth * 2 + ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetCursorPosX((contentSize.x - totalButtonsWidth) * 0.5f);

        if (ImGui::Button(u8"Играть", ImVec2(buttonWidth, 50))) {
            dialogState.okPressed = true;
            dialogState.showDialog = false;
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"Отмена", ImVec2(buttonWidth, 50))) {
            dialogState.cancelPressed = true;
            dialogState.showDialog = false;
        }

        ImGui::End();
    }

    // Функция рендеринга только диалога
    void renderDialogOnly() {
        // Начало нового кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Отрисовка только диалога
        renderDialog();

        // Рендеринг ImGui
        ImGui::Render();

        // Очистка экрана прозрачным цветом
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Полупрозрачный черный
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Включаем blending для прозрачности
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Отрисовка ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glDisable(GL_BLEND);
    }

  
    // Добавляем шрифт
    ImFont* font = nullptr;

};


enum class MessageBoxType {
    INFO,
    ERROR
};

struct MessageBoxResult {
    bool okClicked = false;
};

class MessageBox {
public:
    static void ShowInfo(const std::string& title, const std::string& message) {
        s_data = { title, message, MessageBoxType::INFO, true };
    }
    static void ShowError(const std::string& title, const std::string& message) {
        s_data = { title, message, MessageBoxType::ERROR, true };
    }

    static bool Render() {
        if (s_data.isOpen) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            renderMessageBox();
            // Рендеринг ImGui
            ImGui::Render();

            //// Очистка экрана прозрачным цветом
            //glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Полупрозрачный черный
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Включаем blending для прозрачности
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Отрисовка ImGui
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glDisable(GL_BLEND);        

        }
        return s_data.isOpen;
    }
    static bool IsVisible(){
        return s_data.isOpen;
    }
    static void Close() {
        s_data.isOpen = false;
    }

private:
    struct MessageBoxData {
        std::string title;
        std::string message;
        MessageBoxType type = MessageBoxType::INFO;
        bool isOpen = false;
    };

   static MessageBoxData s_data;

    static void renderMessageBox() {
        // Затемнение фона
        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(0, 0), viewportSize, IM_COL32(0, 0, 0, 128));

        // Центрируем окно
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        // Устанавливаем размер
        ImGui::SetNextWindowSize(ImVec2(400, 140), ImGuiCond_Appearing);

        // Флаги окна
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoMove;
    

        // Стиль в зависимости от типа
        ImVec4 borderColor = getMessageColor(s_data.type);
        ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, borderColor);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, borderColor);

        if (ImGui::Begin(s_data.title.c_str(), &s_data.isOpen, flags)) {
            // Иконка и сообщение
            ImGui::Spacing();

            ImGui::Columns(2, "messagebox", false);
            ImGui::SetColumnWidth(0, 50);

            //// Иконка
            //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
            //ImGui::TextColored(getMessageColor(s_data.type), u8"%s", getIcon(s_data.type));
            ImGui::NextColumn();

            // Сообщение
            ImGui::TextWrapped(u8"%s", s_data.message.c_str());
            ImGui::NextColumn();

            ImGui::Columns(1);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Кнопка OK по центру
            float buttonWidth = 100;
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.5f);

            if (ImGui::Button("OK", ImVec2(buttonWidth, 30))) {
                s_data.isOpen = false;
            }

            // Автозакрытие при нажатии Enter
            if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) {
                s_data.isOpen = false;
            }
        }

        ImGui::End();
        ImGui::PopStyleColor(3);
    }
    static ImVec4 getMessageColor(MessageBoxType type) {
        static std::map<MessageBoxType, ImVec4> colors = {
            {MessageBoxType::INFO, ImVec4(0.0f, 0.5f, 1.0f, 1.0f)},      // Синий
            {MessageBoxType::ERROR, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)}      // Красный
        };
        return colors[type];
    }
    static const char* getIcon(MessageBoxType type) {
        static std::map<MessageBoxType, const char*> icons = {
            {MessageBoxType::INFO, u8"i"},    // Информация
            {MessageBoxType::ERROR, u8"❌"}    // Ошибка
        };
        return icons[type];
    }
};