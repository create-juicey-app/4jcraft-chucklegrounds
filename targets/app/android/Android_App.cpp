#include "Android_App.h"

#include <assert.h>

#include "platform/sdl2/Render.h"

CConsoleMinecraftApp app;

CConsoleMinecraftApp::CConsoleMinecraftApp() : CMinecraftApp() {}

void CConsoleMinecraftApp::SetRichPresenceContext(int iPad, int contextId) {}

void CConsoleMinecraftApp::StoreLaunchData() {}

void CConsoleMinecraftApp::ExitGame() { RenderManager.Close(); }

void CConsoleMinecraftApp::FatalLoadError() { assert(0); }

void CConsoleMinecraftApp::CaptureSaveThumbnail() {}

void CConsoleMinecraftApp::GetSaveThumbnail(std::uint8_t** thumbnailData,
                                            unsigned int* thumbnailSize) {}

void CConsoleMinecraftApp::ReleaseSaveThumbnail() {}

void CConsoleMinecraftApp::GetScreenshot(int iPad,
                                         std::uint8_t** screenshotData,
                                         unsigned int* screenshotSize) {}

int CConsoleMinecraftApp::GetLocalTMSFileIndex(wchar_t* wchTMSFile,
                                               bool bFilenameIncludesExtension,
                                               eFileExtensionType eEXT) {
    return -1;
}

int CConsoleMinecraftApp::LoadLocalTMSFile(wchar_t* wchTMSFile) { return -1; }

int CConsoleMinecraftApp::LoadLocalTMSFile(wchar_t* wchTMSFile,
                                           eFileExtensionType eExt) {
    return -1;
}

void CConsoleMinecraftApp::FreeLocalTMSFiles(eTMSFileType eType) {}

void CConsoleMinecraftApp::TemporaryCreateGameStart() {}
