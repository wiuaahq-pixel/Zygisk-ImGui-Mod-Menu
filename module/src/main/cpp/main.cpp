#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <string>

#include "modmenu.h"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, AppSpecializeArgs *args) override {
        this->api = api;
        this->args = args;
    }

    void preServerSpecialize(ServerSpecializeArgs *args) override {}
    void postServerSpecialize(const ServerSpecializeArgs *args) override {}

    void preAppSpecialize(AppSpecializeArgs *args) override {
        // ANDROID 15 BOOTLOOP FIX: Required for HyperOS stability
        api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);

        // USE THE ISGAME FUNCTION FROM YOUR MODMENU.H
        // This is more reliable than checking nice_name on Android 15
        if (isGame(api->getJNIEnv(), args->app_data_dir)) {
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
