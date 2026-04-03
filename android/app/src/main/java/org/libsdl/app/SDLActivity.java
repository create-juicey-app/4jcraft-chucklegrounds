package org.libsdl.app;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

import java.util.concurrent.atomic.AtomicInteger;

public class SDLActivity extends Activity {
    // SDL needs this shit apparently.
    public static native String nativeGetVersion();
    public static native int nativeSetupJNI();
    public static native int nativeRunMain(String library, String function,
                                           Object arguments);
    public static native void onNativeDropFile(String filename);
    public static native void nativeSetScreenResolution(int surfaceWidth,
                                                        int surfaceHeight,
                                                        int deviceWidth,
                                                        int deviceHeight,
                                                        float rate);
    public static native void onNativeResize();
    public static native void onNativeSurfaceCreated();
    public static native void onNativeSurfaceChanged();
    public static native void onNativeSurfaceDestroyed();
    public static native void onNativeKeyDown(int keycode);
    public static native void onNativeKeyUp(int keycode);
    public static native boolean onNativeSoftReturnKey();
    public static native void onNativeKeyboardFocusLost();
    public static native void onNativeTouch(int touchDevId, int pointerFingerId,
                                            int action, float x, float y,
                                            float p);
    public static native void onNativeMouse(int button, int action, float x,
                                            float y, boolean relative);
    public static native void onNativeAccel(float x, float y, float z);
    public static native void onNativeClipboardChanged();
    public static native void nativeLowMemory();
    public static native void onNativeLocaleChanged();
    public static native void nativeSendQuit();
    public static native void nativeQuit();
    public static native void nativePause();
    public static native void nativeResume();
    public static native void nativeFocusChanged(boolean hasFocus);
    public static native String nativeGetHint(String name);
    public static native boolean nativeGetHintBoolean(String name,
                                                      boolean defaultValue);
    public static native void nativeSetenv(String name, String value);
    public static native void onNativeOrientationChanged(int orientation);
    public static native void nativeAddTouch(int touchId, String name);
    public static native void nativePermissionResult(int requestCode,
                                                     boolean result);

    private static SDLActivity sActivity;
    private static final AtomicInteger sCursorIds = new AtomicInteger(1);

    private Thread sdlThread;
    private SDLSurfaceBridge surface;
    private volatile boolean nativeStarted = false;

    protected String[] getLibraries() {
        return new String[]{"main"};
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sActivity = this;

        surface = new SDLSurfaceBridge(this);
        setContentView(surface);

        String[] libs = getLibraries();
        for (String lib : libs) {
            System.loadLibrary(lib);
        }

        nativeSetupJNI();
        startNativeMainOnce();
    }

    public static String clipboardGetText() {
        SDLActivity activity = sActivity;
        if (activity == null) {
            return "";
        }
        ClipboardManager clipboard = (ClipboardManager) activity.getSystemService(Context.CLIPBOARD_SERVICE);
        if (clipboard == null || !clipboard.hasPrimaryClip()) {
            return "";
        }
        ClipData clip = clipboard.getPrimaryClip();
        if (clip == null || clip.getItemCount() == 0) {
            return "";
        }
        CharSequence text = clip.getItemAt(0).coerceToText(activity);
        return text != null ? text.toString() : "";
    }

    public static boolean clipboardHasText() {
        return !clipboardGetText().isEmpty();
    }

    public static void clipboardSetText(String text) {
        SDLActivity activity = sActivity;
        if (activity == null) {
            return;
        }
        ClipboardManager clipboard = (ClipboardManager) activity.getSystemService(Context.CLIPBOARD_SERVICE);
        if (clipboard != null) {
            clipboard.setPrimaryClip(ClipData.newPlainText("SDL", text != null ? text : ""));
        }
    }

    public static int createCustomCursor(int[] colors, int width, int height,
                                         int hotSpotX, int hotSpotY) {
        return sCursorIds.getAndIncrement();
    }

    public static void destroyCustomCursor(int cursorID) {
    }

    public static Context getContext() {
        return sActivity;
    }

    public static DisplayMetrics getDisplayDPI() {
        DisplayMetrics metrics = new DisplayMetrics();
        SDLActivity activity = sActivity;
        if (activity != null) {
            Display display = activity.getWindowManager().getDefaultDisplay();
            display.getRealMetrics(metrics);
        }
        return metrics;
    }

    public static boolean getManifestEnvironmentVariables() {
        return false;
    }

    public static Surface getNativeSurface() {
        SDLActivity activity = sActivity;
        if (activity == null || activity.surface == null) {
            return null;
        }
        return activity.surface.getHolder().getSurface();
    }

    public static void initTouch() {
    }

    public static boolean isAndroidTV() {
        return false;
    }

    public static boolean isChromebook() {
        return false;
    }

    public static boolean isDeXMode() {
        return false;
    }

    public static boolean isScreenKeyboardShown() {
        return false;
    }

    public static boolean isTablet() {
        SDLActivity activity = sActivity;
        if (activity == null) {
            return false;
        }
        DisplayMetrics metrics = activity.getResources().getDisplayMetrics();
        float widthDp = metrics.widthPixels / metrics.density;
        float heightDp = metrics.heightPixels / metrics.density;
        float smallestWidthDp = Math.min(widthDp, heightDp);
        return smallestWidthDp >= 600.0f;
    }

    public static void manualBackButton() {
        SDLActivity activity = sActivity;
        if (activity != null) {
            activity.onBackPressed();
        }
    }

    public static void minimizeWindow() {
        SDLActivity activity = sActivity;
        if (activity != null) {
            activity.moveTaskToBack(true);
        }
    }

    public static int openURL(String url) {
        SDLActivity activity = sActivity;
        if (activity == null || url == null || url.isEmpty()) {
            return -1;
        }
        try {
            Intent intent = new Intent(Intent.ACTION_VIEW, android.net.Uri.parse(url));
            activity.startActivity(intent);
            return 0;
        } catch (Exception ignored) {
            return -1;
        }
    }

    public static void requestPermission(String permission, int requestCode) {
        nativePermissionResult(requestCode, false);
    }

    public static int showToast(String message, int duration, int gravity,
                                int xOffset, int yOffset) {
        SDLActivity activity = sActivity;
        if (activity == null) {
            return -1;
        }
        activity.runOnUiThread(() -> {
            Toast toast = Toast.makeText(activity, message != null ? message : "",
                    duration > 0 ? Toast.LENGTH_LONG : Toast.LENGTH_SHORT);
            toast.setGravity(gravity, xOffset, yOffset);
            toast.show();
        });
        return 0;
    }

    public static boolean sendMessage(int command, int param) {
        return false;
    }

    public static boolean setActivityTitle(String title) {
        SDLActivity activity = sActivity;
        if (activity == null) {
            return false;
        }
        activity.runOnUiThread(() -> activity.setTitle(title != null ? title : ""));
        return true;
    }

    public static boolean setCustomCursor(int cursorID) {
        return true;
    }

    public static void setOrientation(int w, int h, boolean resizable,
                                      String hint) {
    }

    public static boolean setRelativeMouseEnabled(boolean enabled) {
        return false;
    }

    public static boolean setSystemCursor(int cursorID) {
        return true;
    }

    public static void setWindowStyle(boolean fullscreen) {
    }

    public static boolean shouldMinimizeOnFocusLoss() {
        return false;
    }

    public static boolean showTextInput(int x, int y, int w, int h) {
        return false;
    }

    public static boolean supportsRelativeMouse() {
        return false;
    }

    private void startNativeMainOnce() {
        if (nativeStarted) return;
        nativeStarted = true;

        final String mainLib = getApplicationInfo().nativeLibraryDir + "/libmain.so";
        sdlThread = new Thread(() -> nativeRunMain(mainLib, "SDL_main", new String[0]),
                               "SDLThread");
        sdlThread.start();
    }

    @Override
    protected void onPause() {
        super.onPause();
        nativePause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        nativeResume();
    }

    @Override
    protected void onDestroy() {
        nativeSendQuit();
        nativeQuit();
        if (sActivity == this) {
            sActivity = null;
        }
        super.onDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        nativeFocusChanged(hasFocus);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            onNativeKeyDown(event.getKeyCode());
            return true;
        }
        if (event.getAction() == KeyEvent.ACTION_UP) {
            onNativeKeyUp(event.getKeyCode());
            return true;
        }
        return super.dispatchKeyEvent(event);
    }

    private static final class SDLSurfaceBridge extends SurfaceView
            implements SurfaceHolder.Callback, View.OnTouchListener {
        SDLSurfaceBridge(Activity context) {
            super(context);
            getHolder().addCallback(this);
            setOnTouchListener(this);
            setFocusable(true);
            setFocusableInTouchMode(true);
            requestFocus();
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            onNativeSurfaceCreated();
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format,
                                   int width, int height) {
            onNativeSurfaceChanged();
            nativeSetScreenResolution(width, height, width, height, 60.0f);
            onNativeResize();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            onNativeSurfaceDestroyed();
        }

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            int action = event.getActionMasked();
            int index = event.getActionIndex();
            int pointerId = event.getPointerId(index);
            float x = getWidth() > 0 ? (event.getX(index) / (float) getWidth()) : 0.0f;
            float y = getHeight() > 0 ? (event.getY(index) / (float) getHeight()) : 0.0f;
            float p = Math.min(1.0f, event.getPressure(index));
            onNativeTouch(event.getDeviceId(), pointerId, action, x, y, p);
            return true;
        }
    }
}
