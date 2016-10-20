
#include <stdio.h>
#include "minHTML.h"


void put_file_byte(FILE *file_out, Byte byte)
{
    fputc(byte, file_out);
}


void translate_minHTML_to_HTML(char *file_name_minHTML, char *file_name_HTML)
{
    FILE   *file_in       = fopen(file_name_minHTML, "rb");

    if(!file_in)
        return;

    FILE   *file_out      = fopen(file_name_HTML, "wb");
    Stream *input_stream  = create_stream(file_in, fgetc, feof);
    Stream *output_stream = create_output_stream(file_out, put_file_byte);
    Tag    *document;

    initialize_minHTML_parser();
    document = parse_minHTML(input_stream);

    print_html(document, output_stream);

    destroy_tag(document);
    destroy_stream(input_stream);
    destroy_stream(output_stream);
    fclose(file_in);
    fclose(file_out);
}


String* get_HTML_file_name(char *minHTML_file_name)
{
    String *HTML_file_name = create_string(2);

    copy_char_array_to_string(HTML_file_name, minHTML_file_name);

    while(HTML_file_name->length-1 && HTML_file_name->begin[HTML_file_name->length-1] != '.')
        pop_from_string(HTML_file_name);

    push_in_string(HTML_file_name, 'h');
    push_in_string(HTML_file_name, 't');
    push_in_string(HTML_file_name, 'm');
    push_in_string(HTML_file_name, 'l');

    return HTML_file_name;
}


int main(int arguments_length, char **arguments)
{
    int     i;
    String *HTML_file_name;

    for(i=1; i<arguments_length; i++)
    {
        printf("%s\n", arguments[i]);

        HTML_file_name=get_HTML_file_name(arguments[i]);
        translate_minHTML_to_HTML(arguments[i], HTML_file_name->begin);

        destroy_string(HTML_file_name);
    }

    return 0;
}

