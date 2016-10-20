#ifndef PREFIX_TREE_H_INCLUDED
#define PREFIX_TREE_H_INCLUDED


typedef struct
{
    char              *data;
    struct PrefixTree *prefixes[256];
}
PrefixTree;


PrefixTree* create_prefix_tree();
void add_data_in_prefix_tree(PrefixTree *tree, unsigned char *word, char *data);
char* find_data_in_prefix_tree(PrefixTree *tree, unsigned char *word);
void destroy_prefix_tree(PrefixTree *tree, void (*destroy_data)(char *data));


#endif // PREFIX_TREE_H_INCLUDED
