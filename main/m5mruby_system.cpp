#include "m5mruby.h"
#include "m5mruby_app.h"


M5mrubySystem::M5mrubySystem()
{
  m_mruby_engine = new M5mrubyEngine();
}

M5mrubySystem::~M5mrubySystem()
{
  if(m_mruby_engine) delete m_mruby_engine;
}

const char* sample_script = 
#include "./sample/display.rb"
;
const char* launcher_script = 
#include "./app/launcher.rb"
;

void M5mrubySystem::run()
{
  //TODO
  /***
  * Launcher app (mruby)
  * Remote load/run script
  * Remote mirb
  ****/
  run_menu();

  m_mruby_engine->exec(sample_script);
}


void M5mrubySystem::run_menu()
{
  m_mruby_engine->exec(launcher_script);
}

