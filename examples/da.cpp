#define ARENA_IMPLEMENTATION
#include "../src/arena.h"

Arena arena = {0};

typedef struct node_s {
    int a;
    struct node_s* next;
}Node;

void node_append(Node* node, int num) {
    if (node->next != NULL) {
        node_append(node->next, num);
    } else {
        node->next = arena_alloc(&arena, Node);
        node->next->a = num;
    }
}

void node_traverse(Node* node) {
    printf("%d\n", node->a);
    if (node->next != NULL) {
        node_traverse(node->next);
    }
}

int main(void) {
    arena_prealloc(&arena, 1024 * 1024);

    Node root = {.a = 1};
    node_append(&root, 2);
    node_append(&root, 3);

    node_traverse(&root);

    arena_free(&arena);
    return 0;
}
