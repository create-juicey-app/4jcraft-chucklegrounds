#include "java/InputOutputStream/InputStream.h"

#include <string>

#include "java/File.h"
#include "java/InputOutputStream/FileInputStream.h"

InputStream* InputStream::getResourceAsStream(const std::wstring& fileName) {
    File file(fileName);
    FileInputStream* stream = new FileInputStream(file);
    if (!stream->isOpen()) {
        delete stream;
        return nullptr;
    }
    return stream;
}