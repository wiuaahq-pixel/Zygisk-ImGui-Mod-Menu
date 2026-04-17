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
using zygisk::ServerSpecializeArgs;

// These are already defined in your modmenu.h or other files, 
// so we just declare them here so main.cpp knows they exist.
extern void drawMenu();
extern void *hack_thread(void *);

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, AppSpecializeArgs *args) override {
        this->api = api;
        this->args = args;
    }

    void preServerSpecialize(ServerSpecializeArgs *args) override {}
    void postServerSpecialize(const ServerSpecializeArgs *args) override {}

    void preAppSpecialize(AppSpecializeArgs *args) override {
        // ANDROID 15 BOOTLOOP FIX
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
