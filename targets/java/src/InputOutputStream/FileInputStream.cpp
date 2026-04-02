
#include "java/InputOutputStream/FileInputStream.h"

#include <SDL2/SDL_rwops.h>
#include <assert.h>

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "console_helpers/StringHelpers.h"
#include "java/File.h"

namespace {
int64_t FileTell(SDL_RWops* file) { return SDL_RWtell(file); }

bool FileSeek(SDL_RWops* file, int64_t offset, int origin) {
    return SDL_RWseek(file, offset, origin) >= 0;
}
}  // namespace

// Creates a FileInputStream by opening a connection to an actual file, the file
// named by the File object file in the file system. A new FileDescriptor object
// is created to represent this file connection. First, if there is a security
// manager, its checkRead method is called with the path represented by the file
// argument as its argument.
//
// If the named file does not exist, is a directory rather than a regular file,
// or for some other reason cannot be opened for reading then a
// FileNotFoundException is thrown.
//
// Parameters:
// file - the file to be opened for reading.
// Throws:
// FileNotFoundException - if the file does not exist, is a directory rather
// than a regular file, or for some other reason cannot be opened for reading.
// SecurityException - if a security manager exists and its checkRead method
// denies read access to the file.
FileInputStream::FileInputStream(const File& file) : m_fileHandle(nullptr) {
    const std::string nativePath = wstringtofilename(file.getPath());
    m_fileHandle = SDL_RWFromFile(nativePath.c_str(), "rb");
}

FileInputStream::~FileInputStream() {
    if (m_fileHandle != nullptr) {
        SDL_RWclose(m_fileHandle);
    }
}

bool FileInputStream::isOpen() const { return m_fileHandle != nullptr; }

// Reads a byte of data from this input stream. This method blocks if no input
// is yet available. Returns: the next byte of data, or -1 if the end of the
// file is reached.
int FileInputStream::read() {
    if (m_fileHandle == nullptr) {
        return -1;
    }

    std::uint8_t byteRead = static_cast<std::uint8_t>(0);
    const size_t numberOfBytesRead = SDL_RWread(m_fileHandle, &byteRead, 1, 1);

    if (numberOfBytesRead == 0) {
        // File pointer is past the end of the file
        return -1;
    }

    return static_cast<int>(byteRead);
}

// Reads up to b.size() bytes of data from this input stream into an array of
// bytes. This method blocks until some input is available. Parameters: b - the
// buffer into which the data is read. Returns: the total number of bytes read
// into the buffer, or -1 if there is no more data because the end of the file
// has been reached.
int FileInputStream::read(std::vector<uint8_t>& b) {
    if (m_fileHandle == nullptr) {
        return -1;
    }

    const size_t numberOfBytesRead =
        SDL_RWread(m_fileHandle, b.data(), 1, b.size());

    if (numberOfBytesRead == 0) {
        // File pointer is past the end of the file
        return -1;
    }

    return numberOfBytesRead;
}

// Reads up to len bytes of data from this input stream into an array of bytes.
// If len is not zero, the method blocks until some input is available;
// otherwise, no bytes are read and 0 is returned. Parameters: b - the buffer
// into which the data is read. off - the start offset in the destination array
// b len - the maximum number of bytes read. Returns: the total number of bytes
// read into the buffer, or -1 if there is no more data because the end of the
// file has been reached.
int FileInputStream::read(std::vector<uint8_t>& b, unsigned int offset,
                          unsigned int length) {
    // 4J Stu - We don't want to read any more than the array buffer can hold
    assert(length <= (b.size() - offset));

    if (m_fileHandle == nullptr) {
        return -1;
    }

    const size_t numberOfBytesRead =
        SDL_RWread(m_fileHandle, &b[offset], 1, length);

    if (numberOfBytesRead == 0) {
        // File pointer is past the end of the file
        return -1;
    }

    return numberOfBytesRead;
}

// Closes this file input stream and releases any system resources associated
// with the stream. If this stream has an associated channel then the channel is
// closed as well.
void FileInputStream::close() {
    if (m_fileHandle == nullptr) {
        // printf("\n\nFileInputStream::close - TRYING TO CLOSE AN INVALID FILE
        // void*\n\n");
        return;
    }

    int result = SDL_RWclose(m_fileHandle);

    if (result != 0) {
        // TODO 4J Stu - Some kind of error handling
    }

    // Stop the dtor from trying to close it again
    m_fileHandle = nullptr;
}

// Skips n bytes of input from this input stream. Fewer bytes might be skipped
// if the end of the input stream is reached. The actual number k of bytes to be
// skipped is equal to the smaller of n and count-pos. The value k is added into
// pos and k is returned. Overrides: skip in class InputStream Parameters: n -
// the number of bytes to be skipped. Returns: the actual number of bytes
// skipped.
int64_t FileInputStream::skip(int64_t n) {
    if (m_fileHandle == nullptr || n <= 0) {
        return 0;
    }

    const int64_t start = FileTell(m_fileHandle);
    if (start < 0) {
        return 0;
    }

    if (!FileSeek(m_fileHandle, 0, RW_SEEK_END)) {
        return 0;
    }

    const int64_t end = FileTell(m_fileHandle);
    if (end < 0) {
        return 0;
    }

    const int64_t offset = std::min(n, std::max<int64_t>(0, end - start));
    const int64_t target = start + offset;
    if (!FileSeek(m_fileHandle, target, RW_SEEK_SET)) {
        return 0;
    }

    return offset;
}
