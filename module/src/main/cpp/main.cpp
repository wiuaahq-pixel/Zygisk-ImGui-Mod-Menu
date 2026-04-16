#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <Includes/Dobby/dobby.h>
#include "modmenu.h"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

// --- 1. MECH ARENA OFFSETS ---
#define OFF_SPEED 0x4dc6914
#define OFF_RELOAD 0x4ec4190
#define OFF_SKILL 0x2d498a8

// --- 2. HOOK LOGIC ---
float (*old_Speed)(void *instance);
float new_Speed(void *instance) {
    if (instance != NULL) {
        // Multiplies movement speed by 5.0
        return old_Speed(instance) * 5.0f; 
    }
    return old_Speed(instance);
}

// Instant Reload and Skill Cooldown
float new_Reload(void *instance) { return 0.0f; }
float new_Skill(void *instance) { return 0.0f; }

// --- 3. THE HACK THREAD (Defined before use) ---
void *hack_thread(void *) {
    uintptr_t base = 0;
    do {
        base = get_base("libil2cpp.so");
        if (!base) sleep(1);
    } while (!base);

    DobbyHook((void *)(base + OFF_SPEED), (void *)new_Speed, (void **)&old_Speed);
    DobbyHook((void *)(base + OFF_RELOAD), (void *)new_Reload, NULL);
    DobbyHook((void *)(base + OFF_SKILL), (void *)new_Skill, NULL);

    return NULL;
}

// --- 4. ZYGISK MODULE CLASS ---
class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        env_ = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;
        enable_hack = isGame(env_, args->app_data_dir);
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            pthread_t ntid;
            // Now line 49 (this line) knows what hack_thread is!
            pthread_create(&ntid, NULL, hack_thread, NULL);
        }
    }
private:
    JNIEnv *env_{};
};

REGISTER_ZYGISK_MODULE(MyModule)
