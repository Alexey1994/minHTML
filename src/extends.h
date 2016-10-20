#ifndef EXTENDS_H_INCLUDED
#define EXTENDS_H_INCLUDED


#include <stdlib.h>


#define new(type) malloc(sizeof(type))
#define new_array(type, length) malloc(sizeof(type)*length)

#define reallocate(data, type) data=realloc(data, sizeof(type))
#define reallocate_array(data, type, length) data=realloc(data, sizeof(type)*length)


#define loop for(;;)


typedef unsigned char Byte;
typedef unsigned char Boolean;
typedef unsigned int  Pointer;


typedef struct
{
    Byte *data;
    Byte  type;
}
DynamicData;


DynamicData* create_dynamic_data(Byte *data, Byte type);
void destroy_dynamic_data(DynamicData *data);


#endif // EXTENDS_H_INCLUDED
