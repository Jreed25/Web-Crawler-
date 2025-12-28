#include "crawler.h"

// ============================================================================
// PATH RECONSTRUCTION
// ============================================================================

// Print the path from start to target by backtracking through parent pointers
void print_path(URLQueueNode *target_node) {
    // First, count how many nodes in the path
    int path_length = 0;
    URLQueueNode *current = target_node;
    while (current != NULL) {
        path_length++;
        current = current->parent;
    }
    
    // Allocate array to store path
    URLQueueNode **path = malloc(sizeof(URLQueueNode *) * path_length);
    
    // Fill array in reverse order (from target to start)
    current = target_node;
    for (int i = path_length - 1; i >= 0; i--) {
        path[i] = current;
        current = current->parent;
    }
    
    // Print path from start to target
    for (int i = 0; i < path_length; i++) {
        printf("%s\n", path[i]->url);
    }
    
    free(path);
}
