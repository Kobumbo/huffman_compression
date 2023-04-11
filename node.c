#include <stdio.h>
#include <stdlib.h>
#include "node.h"


node_t *create_node(int code, int occurrences) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->code = code;
    new_node->occurrences = occurrences;
    return new_node;
}


node_t *merge_nodes(node_t *node1, node_t *node2) {
    node_t *parent = (node_t *)malloc(sizeof(node_t));
    parent->left = node1;
    parent->right = node2;

    parent->code = -1;
    parent->occurrences = node1->occurrences + node2->occurrences;

    return parent;
}