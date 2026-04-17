#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include "zygisk.hpp"
#include "modmenu.h"

using zygisk::Api;
using zygisk::AppSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, AppSpecializeArgs *args) override {
        this->api = api;
        this->args = args;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        // Android 15 Safety: Tell Zygisk to clean up properly
        api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        // Launch everything in a separate thread so Zygote can finish instantly
        pthread_t ntid;
        pthread_create(&ntid, NULL, hack_thread, NULL);
    }

private:
    Api *api;
    AppSpecializeArgs *args;
};

REGISTER_ZYGISK_MODULE(MyModule)
