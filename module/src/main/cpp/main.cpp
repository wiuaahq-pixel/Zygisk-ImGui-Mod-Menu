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
        // Force the option to prevent bootloops on Android 15
        api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        
        // Don't check package here; just enable and let hack_thread decide
        enable_hack = 1; 
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
