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

    void preAppSpecialize(AppSpecializeArgs *args) override {
        //api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);

        // Modified to be safer for different Zygisk versions
        if (args && args->app_data_dir) {
            // We pass nullptr for the Env because isGame doesn't strictly need it to check strings
            if (isGame(nullptr, args->app_data_dir)) {
                enable_hack = 1;
            }
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
