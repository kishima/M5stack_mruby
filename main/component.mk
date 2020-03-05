
#COMPONENT_EXTRA_INCLUDES := $(PROJECT_PATH)/components/wifi_manager/src

COMPONENT_DEPENDS := mruby_component arduino M5Stack ota_server

CPPFLAGS += -fpermissive -DM5STACK_MPU6886

