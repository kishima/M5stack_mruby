//DO NOT include Arduino header to avoid conflicting definition

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "m5mruby_ota_server.h"

#include "ota_server.h"


/*
* OTA server
*/

#define WIFI_SSID "M5MRUBYOTA"
#define WIFI_PASSWORD "12345678"

static EventGroupHandle_t wifi_event_group;
static char ip_str[17];

static const int CONNECTED_BIT = BIT0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    printf("WIFI EVENT(%d)\n",event->event_id);
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
      printf("SYSTEM_EVENT_STA_START\n");
      esp_wifi_connect();
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      printf("SYSTEM_EVENT_STA_GOT_IP\n");
      xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
      sprintf(ip_str, IPSTR, IP2STR(&event->event_info.got_ip.ip_info.ip));
      printf("IP:%s\n",ip_str);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      printf("SYSTEM_EVENT_STA_DISCONNECTED\n");
      esp_wifi_connect();
      xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
      break;
    case SYSTEM_EVENT_AP_START: 
        printf("SYSTEM_EVENT_AP_START\n");
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_AP_STACONNECTED: 
        printf("SYSTEM_EVENT_AP_STACONNECTED\n");
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED: 
        printf("SYSTEM_EVENT_AP_STAIPASSIGNED\n");
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_AP_STOP: 
        printf("SYSTEM_EVENT_AP_STOP\n");
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED: 
        printf("SYSTEM_EVENT_AP_STADISCONNECTED\n");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi_station(void)
{
  printf("initialise_wifi_station\n");

  wifi_event_group = xEventGroupCreate();
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  const char *ssid = "BCW710J-B0B1A-G";
  const char *password = "a48f3888cff3a";
	wifi_config_t wifi_config;
  memset((void *)&wifi_config, 0, sizeof(wifi_config_t));
  //snprintf(wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", ssid);
  strcpy((char*)wifi_config.ap.ssid,ssid);

  //snprintf(wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", password);
  strcpy((char*)wifi_config.sta.password,password);

	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );

}


static void initialise_wifi(void)
{
    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_config_t sta_config;
    strcpy((char*)sta_config.ap.ssid,WIFI_SSID);
    sta_config.ap.ssid_len = strlen(WIFI_SSID);
    strcpy((char*)sta_config.ap.password,WIFI_PASSWORD);
    sta_config.ap.max_connection = 2;
    sta_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
   
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("wifi_init_softap finished.SSID:%s password:%s\n",
             WIFI_SSID, WIFI_PASSWORD);    
}

static void ota_server_task(void * param)
{
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    ota_server_start();
    vTaskDelete(NULL);
}



void prepare_ota_server(int mode)
{
  if(mode==0){
    initialise_wifi();
  }else{
    initialise_wifi_station();
  }
  printf("Init WiFi done\n");

  xTaskCreate(&ota_server_task, "ota_server_task", 4096, NULL, 5, NULL);

}

char* ota_sta_ip_addr()
{
  return ip_str;
}