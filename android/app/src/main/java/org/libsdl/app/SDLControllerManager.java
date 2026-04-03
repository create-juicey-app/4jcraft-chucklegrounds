package org.libsdl.app;

public final class SDLControllerManager {
    private SDLControllerManager() {}

    public static native int nativeSetupJNI();

    public static native int onNativePadDown(int deviceId, int keycode);

    public static native int onNativePadUp(int deviceId, int keycode);

    public static native void onNativeJoy(int deviceId, int axis, float value);

    public static native void onNativeHat(int deviceId, int hatId, int x, int y);

    public static native int nativeAddJoystick(int deviceId, String deviceName,
                                               String deviceDesc, int vendorId,
                                               int productId,
                                               boolean isAccelerometer,
                                               int buttonMask, int naxes,
                                               int axisMask, int nhats,
                                               int nballs);

    public static native int nativeRemoveJoystick(int deviceId);

    public static native int nativeAddHaptic(int deviceId, String deviceName);

    public static native int nativeRemoveHaptic(int deviceId);

    public static void pollInputDevices() {
    }

    public static void pollHapticDevices() {
    }

    public static void hapticRun(int deviceId, float intensity, int length) {
    }

    public static void hapticStop(int deviceId) {
    }
}
