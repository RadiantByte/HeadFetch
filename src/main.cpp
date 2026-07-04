#include "ui/TabList.h"

extern "C" [[gnu::visibility("default")]]
void mod_preinit(){
	hf::PlayerBoard::instance().preinit();
}

extern "C" [[gnu::visibility("default")]]
void mod_init(){
	hf::PlayerBoard::instance().init();
}
