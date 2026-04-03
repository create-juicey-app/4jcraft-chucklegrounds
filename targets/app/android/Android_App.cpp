#include "Android_App.h"

#include <android/log.h>
#include <assert.h>

#include "platform/sdl2/Render.h"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "MCLE_App", __VA_ARGS__)
#define LOGE(...) \
    __android_log_print(ANDROID_LOG_ERROR, "MCLE_App", __VA_ARGS__)

CConsoleMinecraftApp app;

CConsoleMinecraftApp::CConsoleMinecraftApp() : CMinecraftApp() {}

void CConsoleMinecraftApp::SetRichPresenceContext(int iPad, int contextId) {}

void CConsoleMinecraftApp::StoreLaunchData() {}

void CConsoleMinecraftApp::ExitGame() { RenderManager.Close(); }

void CConsoleMinecraftApp::FatalLoadError() {
    // eh
    app.DebugPrintf("Fatal load error in App\n");
}

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
