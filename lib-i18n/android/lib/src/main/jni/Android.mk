LOCAL_PATH  := $(call my-dir)
SRC_PATH := $(call my-dir)/../../../../../src

include $(CLEAR_VARS)

LOCAL_MODULE    := i18n
LOCAL_SRC_FILES := $(wildcard $(SRC_PATH)/*.cpp)

LOCAL_CPPFLAGS := -std=c++11 \
                  -fexceptions \
                  -I$(SRC_PATH) \
                  -O3 \
                  -DANDROID

LOCAL_LDFLAGS  := -llog

include $(BUILD_SHARED_LIBRARY)