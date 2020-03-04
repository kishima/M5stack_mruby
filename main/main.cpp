#include "m5mruby.h"
#include "m5mruby_app.h"

#include <string.h>
#include "nvs_flash.h"

extern "C" {
#include "m5mruby_ota_server.h"
}

#include "WiFi.h"

/*
* Application
*/

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

static void smart_config()
{
  //Init WiFi as Station, start SmartConfig
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  printf("Waiting for SmartConfig.\n");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    printf(".");
  }

  printf("\n");
  printf("SmartConfig received.\n");

  //Wait for WiFi to connect to AP
  printf("Waiting for WiFi\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    printf(".");
  }

  printf("\nWiFi Connected.\n");

  printf("IP Address: ");
  printf("%d.%d.%d.%d\n",IP32TOINT(WiFi.localIP()));
}

static void bootstrap_check(){ 
  int btn_a = M5.BtnA.isPressed();
  int btn_b = M5.BtnB.isPressed();
  int btn_c = M5.BtnC.isPressed();

  if(btn_a||btn_b){
    M5MRB_DEBUG(M5MRB_LOG::INFO,"=====================\n");
    M5MRB_DEBUG(M5MRB_LOG::INFO,"   OTA Update mode   \n");
    M5MRB_DEBUG(M5MRB_LOG::INFO,"=====================\n");
  
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("[OTA Update mode]\n");
    M5.update();
    if(btn_a){
      prepare_ota_server(0);
    }else{
      prepare_ota_server(1);
    }

    if(btn_a){
      vTaskDelay(1000);
      tcpip_adapter_ip_info_t info;
      tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP,&info);
      M5MRB_DEBUG(M5MRB_LOG::INFO,"LocalIP: %d.%d.%d.%d\n",IP32TOINT(info.ip.addr));
      M5.Lcd.printf(" %d.%d.%d.%d\n",IP32TOINT(info.ip.addr));
    }else{
      vTaskDelay(5000);
      M5MRB_DEBUG(M5MRB_LOG::INFO,"LocalIP: %s\n",ota_sta_ip_addr());
      M5.Lcd.printf(" %s\n",ota_sta_ip_addr());
    }
    while(true){
      vTaskDelay(1000);
    }
  }

  if(btn_c){
    smart_config();
    M5MRB_DEBUG(M5MRB_LOG::INFO,"=====================\n");
    M5MRB_DEBUG(M5MRB_LOG::INFO,"  WiFi Setting mode  \n");
    M5MRB_DEBUG(M5MRB_LOG::INFO,"=====================\n");
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("[WiFi Setting mode]");
    M5.update();
    smart_config();
    while(true){
      vTaskDelay(10);
    }
  }

  m5mrb_dump_mem_stat();
}

static void mainTask(void *pvParameters)
{
  M5MRB_DEBUG(M5MRB_LOG::INFO,"=======================================\n");
  M5MRB_DEBUG(M5MRB_LOG::INFO," mruby for M5Stack ver:%s (%s)\n",M5MRB_VERSION,M5MRB_RELEASE);
  M5MRB_DEBUG(M5MRB_LOG::INFO,"=======================================\n");
  m5mrb_dump_mem_stat();

  M5.begin();
  bootstrap_check();

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
  ESP_ERROR_CHECK( nvs_flash_init() );

  m5mrb_dump_mem_stat();
  M5MRB_DEBUG(M5MRB_LOG::INFO,"Init Arduino\n");
  initArduino();

  M5MRB_DEBUG(M5MRB_LOG::INFO,"Run Main Task\n");
  xTaskCreateUniversal(mainTask, "mainTask", M5MRB_MAIN_TASK_STACK_SIZE, NULL, 1, &mainTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);
}

