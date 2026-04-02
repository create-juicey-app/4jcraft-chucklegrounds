package org.libsdl.app;

import android.app.Activity;
import android.os.Bundle;

public class SDLActivity extends Activity {
    protected String[] getLibraries() {
        return new String[]{"main"};
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        String[] libs = getLibraries();
        for (String lib : libs) {
            System.loadLibrary(lib);
        }
    }
}
