#include "GaudiKernel/DllMain.icpp"

void GaudiDll::initialize(void* /* hinstDLL */ )    {
}

void GaudiDll::finalize(void* /* hinstDLL */ )      {
}

extern void GRB_load();

#include "GaudiKernel/FactoryTable.h"

extern "C" FactoryTable::EntryList* getFactoryEntries() {
    static bool first = true;
    if ( first ) {
        GRB_load();
        first = false;
    }
    return FactoryTable::instance()->getEntries();
} 


