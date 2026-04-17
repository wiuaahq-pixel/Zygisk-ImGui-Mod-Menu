#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <string>

#include "includes/Dobby/dobby.h"
#include "imgui/imgui.h"
#include "kittymemory/KittyMemory.h"
#include "modmenu.h"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs; // Added back as requested

// --- Global Variables & Toggles ---
bool speed_toggle = false;
bool reload_toggle = false;
bool skill_toggle = false;
float speed_mult = 5.0f;

#define OFF_SPEED 0x4dc6914
#define OFF_RELOAD 0x4ec4190
#define OFF_SKILL 0x2d498a8

// --- Hooked Functions ---
float (*old_Speed)(void *instance);
float new_Speed(void *instance) {
    if (instance != NULL && speed_toggle) return old_Speed(instance) * speed_mult;
    return old_Speed(instance);
}

float new_Reload(void *instance) { return reload_toggle ? 0.0f : 1.5f; }
float new_Skill(void *instance) { return skill_toggle ? 0.0f : 10.0f; }

// --- Menu Drawing ---
void drawMenu() {
    ImGui::Begin("Mech Arena Mod By Imran");
    ImGui::Checkbox("Fast Movement", &speed_toggle);
    if (speed_toggle) ImGui::SliderFloat("Speed x", &speed_mult, 1.0f, 20.0f);
    ImGui::Separator();
    ImGui::Checkbox("Instant Reload", &reload_toggle);
    ImGui::Checkbox("No Skill Cooldown", &skill_toggle);
    ImGui::End();
}

// --- Hack Thread ---
void *hack_thread(void *) {
    while (KittyMemory::getModuleBaseAddr("libil2cpp.so") == 0) sleep(1);

    DobbyHook((void *)KittyMemory::getAbsoluteAddress("libil2cpp.so", OFF_SPEED), (void *)new_Speed, (void **)&old_Speed);
    DobbyHook((void *)KittyMemory::getAbsoluteAddress("libil2cpp.so", OFF_RELOAD), (void *)new_Reload, NULL);
    DobbyHook((void *)KittyMemory::getAbsoluteAddress("libil2cpp.so", OFF_SKILL), (void *)new_Skill, NULL);

    initModMenu((void *)drawMenu);
    return nullptr;
}

// --- Zygisk Module ---
class MyModule : public zygisk::ModuleBase {
public:
    // Android 15 uses this signature
    void onLoad(Api *api, AppSpecializeArgs *args) override {
        this->api = api;
        this->args = args;
    }

    // Server-side methods (empty but required for some Zygisk versions)
    void preServerSpecialize(ServerSpecializeArgs *args) override {}
    void postServerSpecialize(const ServerSpecializeArgs *args) override {}

    void preAppSpecialize(AppSpecializeArgs *args) override {
        // ANDROID 15 SAFETY PIN
        api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);

        const char *process = api->getArgString(args->nice_name);
        if (process && std::string(process) == "com.plarium.mechlegion") {
            enable_hack = 1;
        }
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        if (enable_hack) {
            pthread_t ntid;
            pthread_create(&ntid, NULL, hack_thread, NULL);
        }
    }

private:
    Api *api;
    AppSpecializeArgs *args;
    int enable_hack = 0;
};

REGISTER_ZYGISK_MODULE(MyModule)
