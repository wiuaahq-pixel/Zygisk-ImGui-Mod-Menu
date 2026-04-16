#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

// These paths match your lowercase folders exactly
#include "includes/Dobby/dobby.h"
#include "imgui/imgui.h"
#include "kittymemory/KittyMemory.h" // Matches renamed folder
#include "modmenu.h"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

bool speed_toggle = false;
bool reload_toggle = false;
bool skill_toggle = false;
float speed_mult = 5.0f;

#define OFF_SPEED 0x4dc6914
#define OFF_RELOAD 0x4ec4190
#define OFF_SKILL 0x2d498a8

float (*old_Speed)(void *instance);
float new_Speed(void *instance) {
    if (instance != NULL && speed_toggle) {
        return old_Speed(instance) * speed_mult; 
    }
    return old_Speed(instance);
}

float new_Reload(void *instance) {
    return reload_toggle ? 0.0f : 1.5f; 
}

float new_Skill(void *instance) {
    return skill_toggle ? 0.0f : 10.0f;
}

void DrawMenu() {
    ImGui::Begin("Mech Arena Mod By Imran");
    ImGui::Checkbox("Fast Movement", &speed_toggle);
    if (speed_toggle) {
        ImGui::SliderFloat("Speed x", &speed_mult, 1.0f, 20.0f);
    }
    ImGui::Separator();
    ImGui::Checkbox("Instant Reload", &reload_toggle);
    ImGui::Checkbox("No Skill Cooldown", &skill_toggle);
    ImGui::End();
}


class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override { env_ = env; }
    void preServerSpecialize(ServerSpecializeArgs *args) override {}
    void postServerSpecialize(const ServerSpecializeArgs *args) override {}
    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;
        enable_hack = isGame(env_, args->app_data_dir);
    }
    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            pthread_t ntid;
            pthread_create(&ntid, NULL, hack_thread, NULL);
        }
    }
private:
    JNIEnv *env_{};
};

REGISTER_ZYGISK_MODULE(MyModule)
