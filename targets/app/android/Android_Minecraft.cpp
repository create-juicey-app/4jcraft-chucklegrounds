#include "Android_App.h"
#include "minecraft/client/Minecraft.h"

// minimal impl, change soon
int Android_MinecraftMain(int argc, const char* argv[]) {
    (void)argc;
    (void)argv;

    Minecraft::main();
    return 0;
}

int main(int argc, const char* argv[]) {
    return Android_MinecraftMain(argc, argv);
}
