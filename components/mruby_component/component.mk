
COMPONENT_OWNBUILDTARGET := true
COMPONENT_OWNCLEANTARGET := true

COMPONENT_EXTRA_INCLUDES := $(PROJECT_PATH)/components/M5Stack/src $(PROJECT_PATH)/main $(IDF_PATH)/components/lwip/port/esp32/include

COMPONENT_ADD_INCLUDEDIRS := mruby/include/

CFLAGS += -Wno-char-subscripts -Wno-pointer-sign -DM5STACK_MPU6886

build:
	cd $(COMPONENT_PATH)/mruby && COMPONENT_EXTRA_INCLUDES="$(COMPONENT_EXTRA_INCLUDES)" MRUBY_CONFIG=../esp32_build_config.rb $(MAKE)
	cp $(COMPONENT_PATH)/mruby/build/esp32/lib/libmruby.a $(COMPONENT_LIBRARY)

clean: 
	cd $(COMPONENT_PATH)/mruby && MRUBY_CONFIG=../esp32_build_config.rb $(MAKE) clean

