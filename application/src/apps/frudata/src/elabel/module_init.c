

#include "elabel/module_init.h"
#include "elabel/elabel.h"
#include "elabel/base.h"

void elabel_module_init(void)
{
    init_update_elabel_lock();

    init_elabel_base();
}
