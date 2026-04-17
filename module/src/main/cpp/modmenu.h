#ifndef MODMENU_H
#define MODMENU_H

#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <string>

// Correcting paths based on your previous build successes
#include "kittymemory/MemoryPatch.h"
#include "includes/ESP.h"
#include "includes/Dobby/dobby.h"
#include "includes/Utils.h"
#include "includes/ImGui.h"


// --- Constants & Macros ---
#define targetLibName "libil2cpp.so"
#define OBFUSCATE(x) x 

// --- Offsets Provided by User ---
#define OFF_SPEED 0x4dc6914
#define OFF_RELOAD 0x4ec4190
#define OFF_SKILL 0x2d498a8

// --- Global Toggles & Variables ---
static bool speed_toggle = false;
static bool reload_toggle = false;
static bool skill_toggle = false;
static float speed_mult = 5.0f;
static char *game_package_name = "com.plarium.mechlegion";

// --- Hooked Functions (The Actual Hacks) ---

// 1. Fast Movement Hook
static float (*old_Speed)(void *instance);
float new_Speed(void *instance) {
    if (instance != NULL && speed_toggle) {
        return old_Speed(instance) * speed_mult; 
    }
    return old_Speed(instance);
}

// 2. Instant Reload Hook
float new_Reload(void *instance) {
    return reload_toggle ? 0.0f : 1.5f; 
}

// 3. No Skill Cooldown Hook
float new_Skill(void *instance) {
    return skill_toggle ? 0.0f : 10.0f;
}

// --- UI Integration (The Menu) ---

static void drawMenu() {
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

// --- Package Detection (The Gatekeeper) ---

static int isGame(JNIEnv *env, jstring appDataDir) {
    if (!appDataDir) return 0;
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    static char package_name[256];
    
    if (sscanf(app_data_dir, "/data/%*[^/]/%*d/%s", package_name) != 1) {
        if (sscanf(app_data_dir, "/data/%*[^/]/%s", package_name) != 1) {
            package_name[0] = '\0';
        }
    }
    
    int result = (strcmp(package_name, game_package_name) == 0);
    env->ReleaseStringUTFChars(appDataDir, app_data_dir);
    return result;
}

// --- The Brain (Hack Thread) ---

static void *hack_thread(void *) {
    // 1. Initialize UI (The bridge to drawMenu)
    // Note: Ensure your initModMenu matches this call in your modmenu.h or .cpp
    initModMenu((void *)drawMenu);

    // 2. Wait for game library to load
    unsigned long base = 0;
    do {
        base = KittyMemory::getModuleBaseAddr(targetLibName);
        sleep(1);
    } while (base == 0);

    // 3. Apply Hooks using Dobby
    DobbyHook((void *)(base + OFF_SPEED), (void *)new_Speed, (void **)&old_Speed);
    DobbyHook((void *)(base + OFF_RELOAD), (void *)new_Reload, nullptr);
    DobbyHook((void *)(base + OFF_SKILL), (void *)new_Skill, nullptr);

    return nullptr;
}

#endif // MODMENU_H
