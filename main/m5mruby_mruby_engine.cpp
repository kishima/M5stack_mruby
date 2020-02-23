
#include "m5mruby.h"
#include "m5mruby_app.h"

#include "mruby.h"
#include "mruby/irep.h"
#include "mruby/compile.h"
#include "mruby/error.h"
#include "mruby/string.h"
#include "mruby/array.h"
#include "mruby/variable.h"

M5mrubyEngine::M5mrubyEngine()
{

}

M5mrubyEngine::~M5mrubyEngine()
{

}

extern "C" mrb_value
mrb_unpack_backtrace(mrb_state *mrb, mrb_value backtrace);

void* M5mrubyEngine::mrb_esp32_psram_allocf(mrb_state *mrb, void *p, size_t size, void *ud)
{
  if (size == 0) {
    m5mrb_free(p);
    return NULL;
  }
  else {
    return m5mrb_spi_realloc(p, size);
  }
}


void M5mrubyEngine::check_backtrace(mrb_state *mrb,mrb_value result_val) {
  mrb_value exc = mrb_obj_value(mrb->exc);
  if (mrb_undef_p(result_val)) {
    //Syntax error ?
    mrb_value msg = mrb_iv_get(mrb,exc,mrb_intern_lit(mrb,"mesg"));
    msg = mrb_obj_as_string(mrb, msg);
    if(RSTRING_LEN(msg)>0){
      //M5MRB_DEBUG(M5MRB_LOG::RAW,">> %s\n", RSTRING_PTR(msg));
      snprintf(m_error_msg,M5MRB_DBG_MSG_MAX_LEN-1,RSTRING_PTR(msg));
      const char *head = RSTRING_PTR(msg)+5;
      const char* div = strchr(head,(int)':');
      printf("X:%s\n",head);
      if(div && (div-head<9) ){
        char num[10]={0};
        memcpy(num,head,div-head);
        m_error_line = atoi(num);
        printf("m_error_line:%d\n",m_error_line);
      }
    }else{
      sprintf(m_error_msg,"Error!");
    }
    return;
  }

  //Exception
  mrb_value backtrace = mrb_obj_iv_get(mrb, mrb->exc, mrb_intern_lit(mrb, "backtrace"));
  if (mrb_nil_p(backtrace)) return;
  if (!mrb_array_p(backtrace)) backtrace = mrb_unpack_backtrace(mrb, backtrace);

  mrb_int depth = RARRAY_LEN(backtrace);

  mrb_value s = mrb_funcall(mrb, exc, "inspect", 0);
  int i;
  mrb_value *loc;
  int log_len = 0;
  int csr = 0;

  snprintf(m_error_msg,M5MRB_DBG_MSG_MAX_LEN-csr-1,"  <Exception occurred>");
  csr = strlen(m_error_msg);

  for (i=depth-1,loc=&RARRAY_PTR(backtrace)[i]; i>0; i--,loc--) {
    if (mrb_string_p(*loc)) {
      M5MRB_DEBUG(M5MRB_LOG::ERR,"  [%d] %.*s\n",
                i, (int)RSTRING_LEN(*loc), RSTRING_PTR(*loc));
      if(M5MRB_DBG_MSG_MAX_LEN - csr >1){
        snprintf(m_error_msg+csr,M5MRB_DBG_MSG_MAX_LEN-csr-1," [%d] %.*s\n",
                i, (int)RSTRING_LEN(*loc), RSTRING_PTR(*loc));
        csr = strlen(m_error_msg);
      }
    }
  }
  if (mrb_string_p(*loc)) {
    log_len = (int)RSTRING_LEN(*loc);
    const char *head = RSTRING_PTR(*loc)+5;
    const char* div = strchr(head,(int)':');
      printf("X:%s\n",head);
    if(div && (div-head<9) ){
      char num[10]={0};
      memcpy(num,head,div-head);
      m_error_line = atoi(num);
      printf("m_error_line:%d\n",m_error_line);
    }

    M5MRB_DEBUG(M5MRB_LOG::ERR,"%.*s> ", log_len, RSTRING_PTR(*loc));
    if(M5MRB_DBG_MSG_MAX_LEN - csr >1){
      snprintf(m_error_msg+csr,M5MRB_DBG_MSG_MAX_LEN-csr-1,"%.*s: ",
                log_len, RSTRING_PTR(*loc));
      csr = strlen(m_error_msg);
    }
  }

  M5MRB_DEBUG(M5MRB_LOG::RAW,"%s\n", RSTRING_PTR(s));
  if(M5MRB_DBG_MSG_MAX_LEN - csr >1){
    log_len = (int)RSTRING_LEN(s);
    snprintf(m_error_msg+csr,M5MRB_DBG_MSG_MAX_LEN-csr-1,"%s",RSTRING_PTR(s));
    csr = strlen(m_error_msg);
  }

}

M5MRB_RCODE M5mrubyEngine::exec(const char* script)
{

  M5MRB_DEBUG(M5MRB_LOG::INFO,"<M5mrubyEngine::exec>\n");
  mrb_state *mrb = mrb_open_allocf(mrb_esp32_psram_allocf,NULL);

  int ai = mrb_gc_arena_save(mrb);

  mrbc_context *cxt = mrbc_context_new(mrb);
  cxt->capture_errors = TRUE;
  mrbc_filename(mrb, cxt, "m5rb");

  mrb_parser_state* parser = mrb_parse_nstring(mrb, script, strlen(script), cxt);

  m5mrb_dump_mem_stat();
  
  mrb_value result_val = mrb_load_exec(mrb, parser, cxt);
  if (mrb->exc) {
    //M5MRB_DEBUG(M5MRB_LOG::DEBUG,"Exception occurred: %s\n", mrb_str_to_cstr(mrb, mrb_inspect(mrb, mrb_obj_value(mrb->exc))));
    M5MRB_DEBUG(M5MRB_LOG::ERR,"<Exception occurred>\n");
    check_backtrace(mrb,result_val);
    mrb->exc = 0;
  } else {
    M5MRB_DEBUG(M5MRB_LOG::INFO,"<Execute mruby completed successfully>\n");
  }

  mrb_gc_arena_restore(mrb, ai);
  mrb_close(mrb);



  return M5MRB_RCODE::OK;
}