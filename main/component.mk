
#COMPONENT_EXTRA_INCLUDES := $(PROJECT_PATH)/components/wifi_manager/src

COMPONENT_DEPENDS := mruby_component arduino M5Stack ota_server

CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_DEBUG
CPPFLAGS += -fpermissive

