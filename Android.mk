LOCAL_PATH := $(call my-dir)

###########################
#
# Solarus shared library
#
###########################

include $(CLEAR_VARS)

LOCAL_MODULE := Solarus

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/include/solarus/third_party $(LOCAL_PATH)/third_party/snes_spc/src $(LOCAL_PATH)/third_party/hqx/src jni/lua51/include jni/glm
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/src/audio/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/core/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/entities/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/graphics/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/graphics/sdlrenderer/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/graphics/glrenderer/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/hero/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/lua/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/movements/*.cpp) \
	$(wildcard $(LOCAL_PATH)/third_party/snes_spc/src/*.cpp) \
	$(wildcard $(LOCAL_PATH)/third_party/hqx/src/init.c) \
	$(wildcard $(LOCAL_PATH)/third_party/hqx/src/hq2x.c) \
	$(wildcard $(LOCAL_PATH)/third_party/hqx/src/hq3x.c) \
	$(wildcard $(LOCAL_PATH)/third_party/hqx/src/hq4x.c))

LOCAL_CFLAGS += -DGLM_ENABLE_EXPERIMENTAL
LOCAL_CPPFLAGS += --std=c++17 -fexceptions -frtti
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf modplug openal  PhysicsFS libvorbis lua
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog -landroid

cmd-strip :=

include $(BUILD_SHARED_LIBRARY)
