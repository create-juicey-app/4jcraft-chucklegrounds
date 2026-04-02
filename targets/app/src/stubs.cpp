
#include "app/include/stubs.h"

#include <cstring>

#include "SDL.h"
#include "app/common/ShutdownManager.h"
#include "app/linux/Linux_App.h"
#include "java/FloatBuffer.h"
#include "platform/sdl2/Render.h"

#undef glVertexPointer
#undef glTexCoordPointer
#undef glNormalPointer
#undef glClientActiveTexture
#undef glActiveTexture
#undef glTexGeni

#if APP_PLATFORM_LINUX

void LinuxLogStubLightmapProbe() {
    static bool logged = false;
    if (logged) return;

    logged = true;
    app.DebugPrintf(
        "[linux-lightmap] stubs.cpp: Linux excludes the no-op multitexture "
        "stubs in this file; the runtime uses libGL/4jlibs symbols.\n");
}

#else

void LinuxLogStubLightmapProbe() {}
void LinuxGLLogLightmapState(const char*, int, bool) {}

void Display::update() {}
void Display::swapBuffers() {}

void ShutdownManager::Initialise() {}
void ShutdownManager::StartShutdown() {}
void ShutdownManager::MainThreadHandleShutdown() {}
void ShutdownManager::HasStarted(EThreadId) {}
void ShutdownManager::HasStarted(EThreadId, C4JThread::EventArray*) {}
bool ShutdownManager::ShouldRun(EThreadId) { return true; }
void ShutdownManager::HasFinished(EThreadId) {}

#if APP_PLATFORM_ANDROID && defined(GLES)
extern "C" {

void glGenVertexArrays(GLsizei n, GLuint* arrays) {
    typedef void (*Fn)(GLsizei, GLuint*);
    static Fn fn = (Fn)SDL_GL_GetProcAddress("glGenVertexArrays");
    if (!fn) fn = (Fn)SDL_GL_GetProcAddress("glGenVertexArraysOES");
    if (fn) fn(n, arrays);
}

void glBindVertexArray(GLuint array) {
    typedef void (*Fn)(GLuint);
    static Fn fn = (Fn)SDL_GL_GetProcAddress("glBindVertexArray");
    if (!fn) fn = (Fn)SDL_GL_GetProcAddress("glBindVertexArrayOES");
    if (fn) fn(array);
}

void glDeleteVertexArrays(GLsizei n, const GLuint* arrays) {
    typedef void (*Fn)(GLsizei, const GLuint*);
    static Fn fn = (Fn)SDL_GL_GetProcAddress("glDeleteVertexArrays");
    if (!fn) fn = (Fn)SDL_GL_GetProcAddress("glDeleteVertexArraysOES");
    if (fn) fn(n, arrays);
}

void glVertexAttribIPointer(GLuint index, GLint size, GLenum type,
                            GLsizei stride, const void* pointer) {
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, pointer);
}

void glGenQueries(GLsizei n, GLuint* ids) {
    typedef void (*Fn)(GLsizei, GLuint*);
    static Fn fn = (Fn)SDL_GL_GetProcAddress("glGenQueries");
    if (!fn) fn = (Fn)SDL_GL_GetProcAddress("glGenQueriesEXT");
    if (fn) {
        fn(n, ids);
        return;
    }
    for (GLsizei i = 0; i < n; ++i) ids[i] = 0;
}

void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params) {
    typedef void (*Fn)(GLuint, GLenum, GLuint*);
    static Fn fn = (Fn)SDL_GL_GetProcAddress("glGetQueryObjectuiv");
    if (!fn) fn = (Fn)SDL_GL_GetProcAddress("glGetQueryObjectuivEXT");
    if (fn) {
        fn(id, pname, params);
        return;
    }
    if (params) *params = 1;
}
}
#endif

void glReadPixels(int, int, int, int, int, int, ByteBuffer*) {}

void(glClearDepth)(double) {}

void glVertexPointer(int, int, int, int) {}

void glVertexPointer(int, int, FloatBuffer*) {}

void glTexCoordPointer(int, int, int, int) {}

void glTexCoordPointer(int, int, FloatBuffer*) {}

void glNormalPointer(int, int, int) {}

void glNormalPointer(int, ByteBuffer*) {}

void glEnableClientState(int) {}

void glDisableClientState(int) {}

void glColorPointer(int, int, int, int) {}

void glColorPointer(int, bool, int, ByteBuffer*) {}

void glDrawArrays(int, int, int) {}

#if !APP_PLATFORM_ANDROID
void glNormal3f(float, float, float) {}
#endif

void glGenQueriesARB(IntBuffer*) {}

void glBeginQueryARB(int, int) {}

void glEndQueryARB(int) {}

void glGetQueryObjectuARB(int, int, IntBuffer*) {}

void glShadeModel(int) {}

#if !APP_PLATFORM_ANDROID
void glColorMaterial(int, int) {}
#endif

void glGetFloat(int pname, FloatBuffer* params) {
    if (!params) return;

    const float* matrix = nullptr;
    if (pname == GL_MODELVIEW_MATRIX)
        matrix = RenderManager.MatrixGet(GL_MODELVIEW);
    else if (pname == GL_PROJECTION_MATRIX)
        matrix = RenderManager.MatrixGet(GL_PROJECTION);

    if (!matrix) return;

    std::memcpy(params->_getDataPointer(), matrix, sizeof(float) * 16);
}

// 1.8.2
void glClientActiveTexture(int) {}

void glActiveTexture(int) {}

void glFlush() {}

void glTexGeni(int, int, int) {}

#endif
