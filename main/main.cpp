#include <Arduino.h>
#include <M5Stack.h>
#include <stdio.h>

void setup(){

  M5.begin();
  M5.Lcd.printf("mruby on M5stack fire");

}

void loop() {

  M5.update();

  vTaskDelay(1000);
  printf(" -- mem dump ------------------------\n");
  printf(" | Free size(DMA)     = %d\n",heap_caps_get_free_size(MALLOC_CAP_DMA));
  printf(" | Max free size(DMA) = %d\n",heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
  printf(" | Free size(32bit)   = %d\n",heap_caps_get_free_size(MALLOC_CAP_32BIT));
//  printf(" | Free size(SPI)     = %d\n",heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  printf(" | Free size(INTERNAL)= %d\n",heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  printf(" ------------------------------------\n");

}

