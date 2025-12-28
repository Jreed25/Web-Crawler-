#include "crawler.h"

// ============================================================================
// HASH TABLE FUNCTIONS (for tracking visited URLs)
// ============================================================================

// Simple hash function for strings
// Returns a hash value between 0 and HASH_TABLE_SIZE-1
unsigned int hash_string(const char *str) {
    unsigned int hash = 5381;
    int c;
    
    // Use djb2 hash algorithm
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % HASH_TABLE_SIZE;
}

// Initialize the visited set
// Sets all table entries to NULL and initializes the mutex
void init_visited_set() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        visited_set.table[i] = NULL;
    }
    pthread_mutex_init(&visited_set.lock, NULL);
}

// Check if a URL has been visited
// Returns 1 if visited, 0 if not visited
int is_visited(const char *url) {
    unsigned int index = hash_string(url);
    
    pthread_mutex_lock(&visited_set.lock);
    
    // Walk through the chain at this hash index
    VisitedNode *current = visited_set.table[index];
    while (current != NULL) {
        if (strcmp(current->url, url) == 0) {
            pthread_mutex_unlock(&visited_set.lock);
            return 1; // Found it - already visited
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&visited_set.lock);
    return 0; // Not found - not visited yet
}

// Mark a URL as visited
// Adds the URL to the hash table
void mark_visited(const char *url) {
    unsigned int index = hash_string(url);
    
    pthread_mutex_lock(&visited_set.lock);
    
    // Create a new node for this URL
    VisitedNode *new_node = malloc(sizeof(VisitedNode));
    new_node->url = strdup(url);
    
    // Insert at the beginning of the chain
    new_node->next = visited_set.table[index];
    visited_set.table[index] = new_node;
    
    pthread_mutex_unlock(&visited_set.lock);
}
