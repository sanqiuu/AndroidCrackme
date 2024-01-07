LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libcrackme
LOCAL_CPPFLAGS += -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++17
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include


FILE_LIST += $(wildcard $(LOCAL_PATH)/src/*.c*)

LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDFLAGS += -lEGL -lGLESv2 -lGLESv3 -landroid -llog
include $(BUILD_EXECUTABLE)
