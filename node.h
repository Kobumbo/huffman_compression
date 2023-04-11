#ifndef NODE_H
#define NODE_H


typedef struct node_t {
    struct node_t* left;
    struct node_t* right;
    int code;
    int occurrences;
} node_t;

node_t *create_node(int code, int occurrences);
node_t *merge_nodes(node_t *node1, node_t *node2);

#endif //NODE_H
