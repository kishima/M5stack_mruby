#include "m5mruby.h"
#include "m5mruby_app.h"

static TaskHandle_t mainTaskHandle = NULL;
static M5mrubySystem *m5mrb_system;

void* m5mrb_spi_malloc(size_t size)
{
  void *p = heap_caps_malloc(size,MALLOC_CAP_SPIRAM);
  if(!p) throw "m5mrb_spi_malloc error!";
  return p;
}

void* m5mrb_spi_realloc(void* ptr, size_t size)
{
  void *p = heap_caps_realloc(ptr,size,MALLOC_CAP_SPIRAM);
  if(!p) throw "m5mrb_spi_realloc error!";
  return p;
}

void m5mrb_free(void* ptr){
  free(ptr);
}

void m5mrb_dump_mem_stat(int diff){
  static size_t a1 = 0;
  static size_t a2 = 0;
  static size_t a3 = 0;
  static size_t a4 = 0;
  static size_t a5 = 0;
  if(diff==0 || diff==1){
    M5MRB_DEBUG(M5MRB_LOG::INFO," -- mem dump ------------------------\n");
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(DMA)     = %d\n",heap_caps_get_free_size(MALLOC_CAP_DMA));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Max free size(DMA) = %d\n",heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(32bit)   = %d\n",heap_caps_get_free_size(MALLOC_CAP_32BIT));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(SPI)     = %d\n",heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(INTERNAL)= %d\n",heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    M5MRB_DEBUG(M5MRB_LOG::INFO," ------------------------------------\n");
    a1 = heap_caps_get_free_size(MALLOC_CAP_DMA);
    a2 = heap_caps_get_largest_free_block(MALLOC_CAP_DMA);
    a3 = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    a4 = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    a5 = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  }else{
    M5MRB_DEBUG(M5MRB_LOG::INFO," -- mem dump(Diff) ------------------\n");
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(DMA)     = %d\n",a1-heap_caps_get_free_size(MALLOC_CAP_DMA));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Max free size(DMA) = %d\n",a2-heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(32bit)   = %d\n",a3-heap_caps_get_free_size(MALLOC_CAP_32BIT));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(SPI)     = %d\n",a4-heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    M5MRB_DEBUG(M5MRB_LOG::INFO," | Free size(INTERNAL)= %d\n",a5-heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    M5MRB_DEBUG(M5MRB_LOG::INFO," ------------------------------------\n");
  }
}

void m5mrb_debug_print(M5MRB_LOG lv,const char *fmt,const char* func,int line, ...)
{
  va_list arg;
  switch(lv){
    case M5MRB_LOG::DEBUG:
    printf("[DBG]");break;
    case M5MRB_LOG::MSG:
    printf("[MSG]");break;
    case M5MRB_LOG::INFO:
    printf("[INF]");break;
    case M5MRB_LOG::ERR:
    printf("[ERR]");break;
    case M5MRB_LOG::RAW:
    break;
  }

  if(lv==M5MRB_LOG::ERR){
    printf("[%s:%d]",func,line);
  } 

  va_start(arg, fmt);
  vprintf(fmt, arg);
  va_end(arg);
}

static void show_fatal_error(const char* msg,const char* detail){
  M5MRB_DEBUG(M5MRB_LOG::ERR,"msg:%s\n",msg);
  if(detail){
    M5MRB_DEBUG(M5MRB_LOG::ERR,"detail:%s\n",detail);
  }
}


static void mainTask(void *pvParameters)
{
  M5MRB_DEBUG(M5MRB_LOG::INFO,"=======================================\n");
  M5MRB_DEBUG(M5MRB_LOG::INFO," mruby for M5Stack ver:%s (%s)\n",M5MRB_VERSION,M5MRB_RELEASE);
  M5MRB_DEBUG(M5MRB_LOG::INFO,"=======================================\n");
  m5mrb_dump_mem_stat();

  M5.begin();
  //M5.Lcd.printf("mruby on M5stack fire");
  M5.update();

  try{
    m5mrb_system = new M5mrubySystem();
    m5mrb_system->run();
  }catch(std::bad_alloc& e) {
    show_fatal_error("bad_alloc exception!",e.what());
  }catch(char* msg){
    show_fatal_error("exception!",msg);
  }catch(...){
    show_fatal_error("exception!",nullptr);
  }

  while(true){
	  vTaskDelay(1000);
  }

}


extern "C" void app_main()
{
  //Follow Arduino manner
  initArduino();
  xTaskCreateUniversal(mainTask, "mainTask", M5MRB_MAIN_TASK_STACK_SIZE, NULL, 1, &mainTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
}

