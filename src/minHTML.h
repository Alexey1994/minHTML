#ifndef MIN_HTML_H_INCLUDED
#define MIN_HTML_H_INCLUDED


#include "lib/prefix tree.h"
#include "lib/stream.h"
#include "lib/string.h"


typedef struct
{
    String *name;
    String *data;
}
Attribute;


typedef enum
{
    TEXT_DATA = 0x01,
    TAG       = 0x02
}
TagStructure;


typedef struct
{
    Boolean     is_once;
    String     *name;
    PrefixTree *attributes;
    Array      *structure;
}
Tag;


void initialize_minHTML_parser();
Tag* parse_minHTML(Stream *input_stream);
void print_html(Tag *root_tag, Stream *output_stream);


#endif // MIN_HTML_H_INCLUDED
