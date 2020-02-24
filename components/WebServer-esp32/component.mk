SDKPATH = $(COMPONENT_PATH)/../arduino/tools/sdk/include
COREPATH = $(COMPONENT_PATH)/../arduino/cores
ARDLIBPATH = $(COMPONENT_PATH)/../arduino/libraries

COMPONENT_INCLUDES := $(COMPONENT_PATH)/src
COMPONENT_EXTRA_INCLUDES := $(SDKPATH)/esp32 $(SDKPATH)/driver $(SDKPATH)/soc $(SDKPATH)/freertos $(PROJECT_PATH)/build/include $(SDKPATH)/log $(SDKPATH)/ulp $(SDKPATH)/esp_adc_cal $(SDKPATH)/heap $(COREPATH)/esp32 $(COMPONENT_PATH)/../arduino/variants/esp32 $(COMPONENT_PATH)/src $(ARDLIBPATH)/WiFi/src $(SDKPATH)/esp_event $(SDKPATH)/tcpip_adapter $(SDKPATH)/lwip $(SDKPATH)/vfs $(SDKPATH)/newlib $(ARDLIBPATH)/FS/src

COMPONENT_SRCDIRS := src src/detail

CPPFLAGS += -std=gnu++11 -fno-rtti
