#pragma once

#include <stdio.h>
#include <Arduino.h>
#include <M5Stack.h>


#define M5MRB_VERSION "0.1.0"
#define M5MRB_RELEASE "2020/02/22"

#define M5MRB_MAIN_TASK_PRIORITY  1

#define M5MRB_MAIN_TASK_STACK_SIZE (1024*16)

void* m5mrb_spi_malloc(size_t size);
void* m5mrb_spi_realloc(void* ptr, size_t size);
void m5mrb_free(void* ptr);


#define OVERLOAD_SPI_ALLOCATOR \
  void* operator new(std::size_t size){\
  void* ptr = m5mrb_spi_malloc(size);\
    if(ptr == nullptr) { throw std::bad_alloc(); }\
    return ptr; }\
  void operator delete(void* ptr){ m5mrb_free(ptr); }

#define M5MRB_BITMAP_HEADER_SIZE (4)

#define ENABLE_M5MRB_LOG

enum class M5MRB_LOG{
  DEBUG=0,
  MSG,
  INFO,
  ERR,
  RAW,
};

enum class M5MRB_RCODE:int{
  DEVICE_ERROR=-100,
  NOTFOUND_ERROR,
  NOTREADY_ERROR,
  MEMALLOC_ERROR,
  ERROR=-1,
  OK=0,
  OK_DONE,
  OK_CONTINUE,
  OK_EMPTY,
};

#define IP32TOINT(a) (a&0x000000FF), (a&0x0000FF00)>>8, (a&0x00FF0000)>>16, (a&0xFF000000)>>24


//debug
void m5mrb_debug_print(M5MRB_LOG lv,const char *fmt,const char* func,int line,...);
void m5mrb_dump_mem_stat(int diff=0);

#ifdef ENABLE_M5MRB_LOG
  #define M5MRB_DEBUG(lv,fmt, ...) m5mrb_debug_print(lv,fmt, __func__ , __LINE__ , ##__VA_ARGS__ )
#else
  #define M5MRB_DEBUG(...)
#endif

