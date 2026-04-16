LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libdobby
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

MAIN_LOCAL_PATH := $(call my-dir)
LOCAL_C_INCLUDES += $(MAIN_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := modmenu

# Code optimization
LOCAL_CFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CFLAGS += -fno-rtti -fno-exceptions -fpermissive
LOCAL_CPPFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -Werror -s -std=c++17
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fms-extensions -fno-rtti -fno-exceptions -fpermissive
LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all, -llog
LOCAL_ARM_MODE := arm

LOCAL_STATIC_LIBRARIES := libdobby

# --- FIXED LOWERCASE PATHS ---
LOCAL_SRC_FILES := main.cpp \
				   kittymemory/KittyMemory.cpp \
				   kittymemory/MemoryPatch.cpp \
				   kittymemory/MemoryBackup.cpp \
				   kittymemory/KittyUtils.cpp \
				   imgui/backends/imgui_impl_opengl3.cpp \
				   imgui/backends/imgui_impl_android.cpp \
				   imgui/imgui.cpp \
				   imgui/imgui_demo.cpp \
				   imgui/imgui_draw.cpp \
				   imgui/imgui_tables.cpp \
				   imgui/imgui_widgets.cpp \

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3 -lGLESv2

include $(BUILD_SHARED_LIBRARY)
