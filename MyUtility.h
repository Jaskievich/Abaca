#pragma once
#include <string>

struct MyUtility
{
    static std::wstring simpleCharToWString(const char* narrowStr) {
        if (narrowStr == nullptr) return L"";

        std::wstring result;
        while (*narrowStr) {
            result += static_cast<wchar_t>(*narrowStr++);
        }
        return result;
    }

    // Преобразование wstring в string (кроссплатформенное)
    static std::string wstringToString(const std::wstring& wstr) {
        if (wstr.empty()) {
            return "";
        }

        std::string result;
        result.reserve(wstr.size());

        for (wchar_t wc : wstr) {
            // Простое преобразование (работает для ASCII символов)
            if (wc <= 0x7F) {
                result += static_cast<char>(wc);
            }
            else {
                // Для не-ASCII символов заменяем на '?' или опускаем
                result += '?';
            }
        }

        return result;
    }
};