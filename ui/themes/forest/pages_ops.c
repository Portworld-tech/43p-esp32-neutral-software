/** Forest - energy/gateway/network/points via shared FE pages */
#include "theme_local.h"
#include "hub_pages_common.h"
#include "hub_device_ui.h"

void build_energy(lv_obj_t *parent)
{
    hub_build_energy(parent);
}

void build_gateway(lv_obj_t *parent)
{
    hub_build_gateway(parent);
}

void build_network(lv_obj_t *parent)
{
    hub_build_network(parent);
}

void build_points(lv_obj_t *parent)
{
    hub_build_points(parent);
}
