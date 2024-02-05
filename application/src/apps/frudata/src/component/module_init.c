

#include "component/module_init.h"
#include "component/base.h"
#include "component/connector.h"
#include "prepare_fru.h"

void component_module_init(void)
{
    init_component_base();

    lock_connector();
    set_fru_callback_flag(TRUE);
    init_connectors();
    unlock_connector();
}
