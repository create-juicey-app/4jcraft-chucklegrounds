#pragma once

#include <stdarg.h>

#include <string>

#include "Language.h"

class Language;

class I18n {
public:
    static Language* lang;
    static std::wstring get(std::wstring id, ...);
    static std::wstring get(const std::wstring& id, va_list args);
};
