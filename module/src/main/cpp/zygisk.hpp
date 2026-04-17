#pragma once

#include <jni.h>

#define ZYGISK_API_VERSION 4

namespace zygisk {

struct AppSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jobjectArray &rlimits;
    jint &mount_external;
    jstring &se_info;
    jstring &nice_name;
    jboolean &is_child_zygote;
    jstring &instruction_set;
    jstring &app_data_dir;
    jboolean &is_top_app;
    jobjectArray &pkg_data_info_list;
    jobjectArray &whitelisted_data_info_list;
    jboolean &mount_data_dirs;
    jboolean &use_app_image_startup_cache;
};

struct ServerSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jlong &capabilities;
    jlong &permitted_capabilities;
    jlong &effective_capabilities;
};

enum class Option : int {
    FORCE_DENYLIST_UNMOUNT = 0,
    DLCLOSE_MODULE_LIBRARY = 1,
};

class Api {
public:
    virtual int getApiVersion() = 0;
    virtual void setOption(Option opt) = 0;
    virtual uint32_t getFlags() = 0;
    virtual void connectCompanion() = 0;
    virtual void setAppSpecializeArgs(AppSpecializeArgs *args) = 0;
    virtual void setServerSpecializeArgs(ServerSpecializeArgs *args) = 0;
    virtual const char *getArgString(jstring str) = 0;
};

class ModuleBase {
public:
    virtual void onLoad(Api *api, AppSpecializeArgs *args) {}
    virtual void preAppSpecialize(AppSpecializeArgs *args) {}
    virtual void postAppSpecialize(const AppSpecializeArgs *args) {}
    virtual void preServerSpecialize(ServerSpecializeArgs *args) {}
    virtual void postServerSpecialize(const ServerSpecializeArgs *args) {}
};

typedef void (*RegisterModule_t)(ModuleBase *);

} // namespace zygisk

#define REGISTER_ZYGISK_MODULE(clazz) \
extern "C" [[gnu::visibility("default")]] \
void zygisk_module_entry(zygisk::Api *api, zygisk::RegisterModule_t reg) { \
    static clazz module; \
    reg(&module); \
}
