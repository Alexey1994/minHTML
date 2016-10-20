#include "minHTML.h"

Boolean is_debug=1;


static PrefixTree *once_tags;


void initialize_minHTML_parser()
{
    once_tags = create_prefix_tree();

    add_data_in_prefix_tree(once_tags, "!DOCTYPE", 1);
    add_data_in_prefix_tree(once_tags, "input", 1);
    add_data_in_prefix_tree(once_tags, "link", 1);
    add_data_in_prefix_tree(once_tags, "meta", 1);
    add_data_in_prefix_tree(once_tags, "area", 1);
    add_data_in_prefix_tree(once_tags, "base", 1);
    add_data_in_prefix_tree(once_tags, "br", 1);
    add_data_in_prefix_tree(once_tags, "basefont", 1);
    add_data_in_prefix_tree(once_tags, "bgsound", 1);
    add_data_in_prefix_tree(once_tags, "colgroup", 1);
    add_data_in_prefix_tree(once_tags, "command", 1);
    add_data_in_prefix_tree(once_tags, "col", 1);
    add_data_in_prefix_tree(once_tags, "embed", 1);
    add_data_in_prefix_tree(once_tags, "frame", 1);
    add_data_in_prefix_tree(once_tags, "hr", 1);
    add_data_in_prefix_tree(once_tags, "img", 1);
    add_data_in_prefix_tree(once_tags, "isindex", 1);
    add_data_in_prefix_tree(once_tags, "param", 1);
    add_data_in_prefix_tree(once_tags, "source", 1);
    add_data_in_prefix_tree(once_tags, "wbr", 1);

    add_data_in_prefix_tree(once_tags, "embed", 1);
    add_data_in_prefix_tree(once_tags, "embed", 1);
    add_data_in_prefix_tree(once_tags, "embed", 1);
    add_data_in_prefix_tree(once_tags, "embed", 1);
    add_data_in_prefix_tree(once_tags, "embed", 1);
}


typedef struct
{
    Tag    *current_tag;
    Stream *input_stream;
    Array  *previouse_tags;
    int     current_level;
    int     spaces_by_level;
    int     tabs_by_level;
}
MinHTML_parser;


void destroy_tag_structure(DynamicData *data)
{
    switch(data->type)
    {
    case TEXT_DATA:
        destroy_string(data->data);
        break;

    case TAG:
        destroy_tag(data->data);
        break;
    }

    free(data);
}


Attribute* create_attribute(String *name, String *data)
{
    Attribute *attribute = new(Attribute);

    attribute->name = name;
    attribute->data = data;

    return attribute;
}


void destroy_attribute(Attribute *attribute)
{
    destroy_string(attribute->data);
    destroy_string(attribute->name);
    free(attribute);
}


void destroy_tag(Tag *tag)
{
    destroy_string(tag->name);
    destroy_prefix_tree(tag->attributes, destroy_attribute);
    destroy_array(tag->structure);
    free(tag);
}


Tag* create_tag(String *name)
{
    Tag *tag=new(Tag);

    tag->is_once=find_data_in_prefix_tree(once_tags, name->begin);
    tag->name=name;
    tag->attributes=create_prefix_tree();
    tag->structure=create_array(2, destroy_tag_structure);

    return tag;
}


Boolean is_st(Character c)
{
    if(c=='>' || c=='/' || c=='\n')
        return 1;

    return 0;
}


Boolean is_breakeble_character(Character c)
{
    if(is_space(c) || is_st(c))
        return 1;

    return 0;
}


String* read_tag_name(Stream *input_stream)
{
    String *tag_name=create_string(2);

    while(!end_stream(input_stream) && !is_breakeble_character(input_stream->head))
    {
        push_in_string(tag_name, input_stream->head);
        get_stream_byte(input_stream);
    }

    return tag_name;
}


static String* parse_attribute_data(MinHTML_parser *parser)
{
    Stream    *input_stream    = parser->input_stream;
    String    *data            = create_string(2);
    Character  close_character;

    if(input_stream->head=='=')
    {
        get_stream_byte(input_stream);

        skip_text_stream(input_stream);

        if(input_stream->head=='\'' || input_stream->head=='"')
            close_character=input_stream->head;

        get_stream_byte(input_stream);

        while(!end_stream(input_stream) && input_stream->head!=close_character)
        {
            push_in_string(data, input_stream->head);
            get_stream_byte(input_stream);
        }

        if(input_stream->head == close_character)
            get_stream_byte(input_stream);
    }

    return data;
}


static void parse_attribute(MinHTML_parser *parser)
{
    Stream    *input_stream     = parser->input_stream;
    String    *attribute_name;
    String    *attribute_data;
    String    *finded_attribute;
    Attribute *attribute;

    skip_text_stream(input_stream);

    if(is_st(input_stream->head))
        return;

    attribute_name=create_string(2);

    while(!end_stream(input_stream) && !is_st(input_stream->head) && input_stream->head!='=')
    {
        push_in_string(attribute_name, input_stream->head);
        get_stream_byte(input_stream);
    }


    attribute_data = parse_attribute_data(parser);
    finded_attribute = find_data_in_prefix_tree(parser->current_tag->attributes, attribute_name->begin);

    if(finded_attribute)
        destroy_string(finded_attribute);

    attribute = create_attribute(attribute_name, attribute_data);
    add_data_in_prefix_tree(parser->current_tag->attributes, attribute_name->begin, attribute);
}


static void parse_attributes(MinHTML_parser *parser)
{
    Stream *input_stream = parser->input_stream;

    while(!end_stream(input_stream) && !is_st(input_stream->head))
    {
        parse_attribute(parser);
        skip_text_stream(input_stream);
    }

    while(!end_stream(input_stream) && input_stream->head!='>')
        get_stream_byte(input_stream);

    if(!end_stream(input_stream))
        get_stream_byte(input_stream);
}


static void parse_comment(Stream *input_stream)
{
    while(!end_stream(input_stream) && !read_if_on_stream_head(input_stream, "-->"))
        get_stream_byte(input_stream);
}


static void parse_text(MinHTML_parser *parser, Boolean is_empty_tag)
{
    Stream *input_stream   = parser->input_stream;
    String *text           = create_string(2);
    Boolean is_skip_spaces = 0;

    if(is_empty_tag)
        push_in_string(text, '<');

    while(!end_stream(input_stream) && input_stream->head!='<' && input_stream->head != '\n')
    {
        if(is_space(input_stream->head))
        {
            if(!is_skip_spaces)
                push_in_string(text, input_stream->head);

            is_skip_spaces=1;
        }
        else
        {
            push_in_string(text, input_stream->head);
            is_skip_spaces=0;
        }

        get_stream_byte(input_stream);
    }

    push_in_array(parser->current_tag->structure, create_dynamic_data(text, TEXT_DATA));
}


static void parse_open_tag(MinHTML_parser *parser)
{
    Stream *input_stream = parser->input_stream;
    String *tag_name;
    Tag    *tag;

    if(is_space(input_stream->head))
        parse_text(parser, 1);

    else if(read_if_on_stream_head(input_stream, "!--"))
        parse_comment(input_stream);

    else
    {
        tag_name = read_tag_name(input_stream);
        tag = create_tag(tag_name);

        push_in_array(parser->previouse_tags, parser->current_tag);
        push_in_array(parser->current_tag->structure, create_dynamic_data(tag, TAG));
        parser->current_tag=tag;

        parse_attributes(parser);

        if(tag->is_once)
            parser->current_tag = pop_from_array(parser->previouse_tags);
    }
}


static void parse_close_tag(MinHTML_parser *parser)
{
    Stream *input_stream = parser->input_stream;

    read_tag_name(input_stream);

    while(!end_stream(input_stream) && input_stream->head!='>')
        get_stream_byte(input_stream);

    if(!end_stream(input_stream))
        get_stream_byte(input_stream);

    if(parser->previouse_tags->length)
        parser->current_tag = pop_from_array(parser->previouse_tags);
}


static void change_level(MinHTML_parser *parser, int level)
{
    if(level > parser->previouse_tags->length)
    {
        printf("error levels: too many\n");
        return;
    }

    while(level < parser->previouse_tags->length)
        parser->current_tag = pop_from_array(parser->previouse_tags);
}


static void update_tag_level(MinHTML_parser *parser)
{
    Stream *input_stream = parser->input_stream;
    int     current_level;
    int     i;

    if(!parser->spaces_by_level && !parser->tabs_by_level)
    {
        switch(input_stream->head)
        {
        case ' ':

            while(input_stream->head == ' ')
            {
                parser->spaces_by_level++;
                get_stream_byte(input_stream);
            }

            current_level = 1;
            break;

        case '\t':

            while(input_stream->head == '\t')
            {
                parser->tabs_by_level++;
                get_stream_byte(input_stream);
            }

            current_level = 1;
            break;

        default:
            current_level = 0;
        }

        return;
    }

    if(parser->tabs_by_level)
    {
        for(current_level=0; input_stream->head == '\t'; current_level++)
        {
            for(i=0; i<parser->tabs_by_level; i++)
                get_stream_byte(input_stream);
        }
    }
    else if(parser->spaces_by_level)
    {
        for(current_level=0; input_stream->head == ' '; current_level++)
        {
            for(i=0; i<parser->spaces_by_level; i++)
                get_stream_byte(input_stream);
        }
    }

    if(input_stream->head == '\r')
        return;

    change_level(parser, current_level);
}


static void skip_spaces(MinHTML_parser *parser)
{
    Stream *input_stream = parser->input_stream;
    int     current_level;

    while(input_stream->head!='\n' && is_space(input_stream->head))
        get_stream_byte(input_stream);

    if(input_stream->head=='\n')
    {
        get_stream_byte(input_stream);
        update_tag_level(parser);
    }
}


Tag* parse_minHTML(Stream *input_stream)
{
    MinHTML_parser *parser = new(MinHTML_parser);
    String         *name_root_tag = create_string(2);
    Tag            *root_tag;

    copy_char_array_to_string(name_root_tag, "document");
    root_tag=create_tag(name_root_tag);

    parser->current_tag     = root_tag;
    parser->previouse_tags  = create_array(2, 0);
    parser->input_stream    = input_stream;
    parser->current_level   = 0;
    parser->spaces_by_level = 0;
    parser->tabs_by_level   = 0;

    while(!end_stream(input_stream))
    {
        skip_spaces(parser);
        parse_text(parser, 0);

        if(input_stream->head=='<')
        {
            get_stream_byte(input_stream);
            parse_open_tag(parser);
        }
    }

    return root_tag;
}


static void write_string(String *string, Stream *output_stream)
{
    int i;

    for(i=0; i<string->length; i++)
        put_stream_byte(output_stream, string->begin[i]);
}


static void print_attributes(PrefixTree *attributes, Stream *output_stream)
{
    if(!attributes)
        return;

    Attribute *attribute;
    int        i;

    if(attributes->data)
    {
        attribute=attributes->data;

        put_stream_byte(output_stream, ' ');
        write_string(attribute->name, output_stream);
        put_stream_byte(output_stream, '=');
        put_stream_byte(output_stream, '"');
        write_string(attribute->data, output_stream);
        put_stream_byte(output_stream, '"');
    }

    for(i=0; i<256; i++)
        print_attributes(attributes->prefixes[i], output_stream);
}


void print_html(Tag *root_tag, Stream *output_stream)
{
    static int   level=0;
    DynamicData *data;
    String      *text;
    int          i;
    int          j;

    for(i=0; level && i<level-1; i++)
        put_stream_byte(output_stream, '\t');

    if(level)
    {
        put_stream_byte(output_stream, '<');
        write_string(root_tag->name, output_stream);
        print_attributes(root_tag->attributes, output_stream);
        put_stream_byte(output_stream, '>');
        put_stream_byte(output_stream, '\r');
        put_stream_byte(output_stream, '\n');
    }

    if(!root_tag->is_once)
        level++;

    for(i=0; i<root_tag->structure->length; i++)
    {
        data=root_tag->structure->data[i];

        switch(data->type)
        {
            case TAG:
                print_html(data->data, output_stream);
                break;

            case TEXT_DATA:
                text=data->data;

                if(text->length)
                {
                    for(j=0; level && j<level-1; j++)
                        put_stream_byte(output_stream, '\t');

                    write_string(text, output_stream);
                }
                break;
        }
    }

    if(!root_tag->is_once)
        level--;

    if(level && !root_tag->is_once)
    {
        for(j=0; level && j<level-1; j++)
            put_stream_byte(output_stream, '\t');

        put_stream_byte(output_stream, '<');
        put_stream_byte(output_stream, '/');
        write_string(root_tag->name, output_stream);
        put_stream_byte(output_stream, '>');
        put_stream_byte(output_stream, '\r');
        put_stream_byte(output_stream, '\n');
    }
}
