#include "extends.h"


DynamicData* create_dynamic_data(Byte *data, Byte type)
{
    DynamicData *dynamic_data = new(DynamicData);

    dynamic_data->data = data;
    dynamic_data->type = type;

    return dynamic_data;
}


void destroy_dynamic_data(DynamicData *dynamic_data)
{
    free(dynamic_data);
}
