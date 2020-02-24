#pragma once

#include "mruby.h"

#define M5MRB_DBG_MSG_MAX_LEN (1024)
class M5mrubyEngine{
public:
  char m_error_msg[M5MRB_DBG_MSG_MAX_LEN];
  int m_error_line;

  OVERLOAD_SPI_ALLOCATOR
  M5mrubyEngine();
  ~M5mrubyEngine();
  M5MRB_RCODE exec(const char* script);

private:
  void check_backtrace(mrb_state *mrb,mrb_value result_val);
  static void* mrb_esp32_psram_allocf(mrb_state *mrb, void *p, size_t size, void *ud);


};

class M5mrubySystem{
public:
  OVERLOAD_SPI_ALLOCATOR
  M5mrubySystem();
  ~M5mrubySystem();
  void run();

private:
  M5mrubyEngine *m_mruby_engine;

  void run_menu();
};

