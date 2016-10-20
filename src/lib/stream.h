#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED


#include "array.h"
#include "../extends.h"


typedef struct
{
    Byte          head;
    Byte         *source;
    Byte        (*get_byte)(Byte *source);
    void        (*put_byte)(Byte *source, Byte data);
    Byte        (*end_of_data)(Byte *source);

    Array        *local_buffer;
    Array        *buffer;
    unsigned int  begin_buffer;
}
Stream;


Stream* create_stream(Byte *source, Byte(*get_byte)(Byte *source), Byte(*end_of_data)(Byte *source));
Stream* create_output_stream(Byte *source, Byte(*put_byte)(Byte *source, Byte data));
void destroy_stream(Stream *stream);

void get_stream_byte(Stream *stream);
void put_stream_byte(Stream *stream, Byte data);

char end_stream(Stream *stream);

void skip_text_stream(Stream *stream);
unsigned int get_text_stream_uint(Stream *stream);
float get_text_stream_float(Stream *stream);

char read_if_on_stream_head(Stream *stream, char *word);


unsigned int get_bin_stream_uint(Stream *stream);
unsigned int get_bin_stream_ushort(Stream *stream);
void skip_stream(Stream *stream, unsigned int length);


#endif // STREAM_H_INCLUDED
