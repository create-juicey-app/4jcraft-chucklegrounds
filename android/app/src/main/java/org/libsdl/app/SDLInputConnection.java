package org.libsdl.app;

public final class SDLInputConnection {
    private SDLInputConnection() {}

    public static native void nativeCommitText(String text, int newCursorPosition);

    public static native void nativeGenerateScancodeForUnichar(char c);
}
