LOCAL_PATH  := $(call my-dir)
SRC_PATH := $(call my-dir)/../../../../../src

include $(CLEAR_VARS)

LOCAL_MODULE    := style
LOCAL_SRC_FILES := $(wildcard $(SRC_PATH)/*.cpp) \
                   $(wildcard $(SRC_PATH)/css/*.cpp) \
                   $(wildcard $(SRC_PATH)/dls/*.cpp) \
                   $(wildcard $(SRC_PATH)/value/*.cpp)

LOCAL_CPPFLAGS := -std=c++11 \
                  -fexceptions \
                  -I$(SRC_PATH) \
                  -I$(SRC_PATH)/css \
                  -I$(SRC_PATH)/dls \
                  -I$(SRC_PATH)/value \
                  -O3 \
                  -DANDROID

LOCAL_LDFLAGS  := -llog

include $(BUILD_SHARED_LIBRARY)