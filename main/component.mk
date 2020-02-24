#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_DEPENDS := mruby_component arduino M5Stack ota_server

# COMPONENT_EXTRA_INCLUDES := $(PROJECT_PATH)/components/FabGL_component/src

CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_DEBUG
CPPFLAGS += -fpermissive

