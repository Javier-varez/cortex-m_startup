LOCAL_DIR := $(call current-dir)

## We use the cortex-m0 target for broadest compatibility.

include $(CLEAR_VARS)
LOCAL_NAME := cortex_m_startup
LOCAL_CFLAGS := \
    -mcpu=cortex-m0 \
    -mfloat-abi=soft \
    -mthumb \
    -Os \
    -g3 \
    -I$(LOCAL_DIR)/inc \
    -Wall \
    -Werror \
    -Wextra \
    -Wno-gnu-string-literal-operator-template
LOCAL_CXXFLAGS := \
    $(LOCAL_CFLAGS) \
    -std=gnu++17 \
    -fno-exceptions \
    -fno-rtti
LOCAL_ARFLAGS := \
    -rcs
LOCAL_LINKER_FILE := \
    $(LOCAL_DIR)/cortex-m.ld
LOCAL_SRC := \
    $(LOCAL_DIR)/src/startup.cpp
LOCAL_COMPILER := arm_clang
include $(BUILD_STATIC_LIB)
