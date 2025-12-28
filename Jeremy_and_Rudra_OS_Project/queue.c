#include "crawler.h"

// ============================================================================
// QUEUE FUNCTIONS (for managing URLs to crawl)
// ============================================================================

// Initialize the URL queue
// Sets up empty queue and initializes synchronization primitives
void init_queue() {
    url_queue.head = NULL;
    url_queue.tail = NULL;
    url_queue.active_threads = 0;
    url_queue.found = 0;
    url_queue.target_node = NULL;
    pthread_mutex_init(&url_queue.lock, NULL);
    pthread_cond_init(&url_queue.cond, NULL);
}

// Add a URL to the queue (priority-based insertion)
// Creates a new node with the URL, depth, priority, and parent pointer
// Higher priority URLs are inserted closer to the front
void enqueue(const char *url, int depth, URLQueueNode *parent) {
    // Create a new queue node
    URLQueueNode *new_node = malloc(sizeof(URLQueueNode));
    new_node->url = strdup(url);
    new_node->depth = depth;
    new_node->priority = calculate_priority(url, target_url);
    new_node->parent = parent;
    new_node->next = NULL;
    
    pthread_mutex_lock(&url_queue.lock);
    
    // Insert based on priority (higher priority = closer to front)
    if (url_queue.head == NULL) {
        // Queue is empty
        url_queue.head = new_node;
        url_queue.tail = new_node;
    } else if (new_node->priority > url_queue.head->priority) {
        // Insert at the front
        new_node->next = url_queue.head;
        url_queue.head = new_node;
    } else {
        // Find correct position in queue
        URLQueueNode *current = url_queue.head;
        while (current->next != NULL && current->next->priority >= new_node->priority) {
            current = current->next;
        }
        
        // Insert after current
        new_node->next = current->next;
        current->next = new_node;
        
        // Update tail if we inserted at the end
        if (new_node->next == NULL) {
            url_queue.tail = new_node;
        }
    }
    
    // Signal one waiting thread that there's work available
    pthread_cond_signal(&url_queue.cond);
    
    pthread_mutex_unlock(&url_queue.lock);
}

// Remove and return a URL from the queue
// Returns NULL if queue is empty and all threads are idle (work is done)
// Blocks if queue is empty but other threads are still working
URLQueueNode *dequeue() {
    pthread_mutex_lock(&url_queue.lock);
    
    while (1) {
        // If target found, all threads should stop
        if (url_queue.found) {
            pthread_mutex_unlock(&url_queue.lock);
            return NULL;
        }
        
        // If there's a URL in the queue, return it
        if (url_queue.head != NULL) {
            URLQueueNode *node = url_queue.head;
            url_queue.head = url_queue.head->next;
            
            // If queue is now empty, update tail
            if (url_queue.head == NULL) {
                url_queue.tail = NULL;
            }
            
            pthread_mutex_unlock(&url_queue.lock);
            return node;
        }
        
        // Queue is empty - check if we should wait or exit
        if (url_queue.active_threads == 0) {
            // No threads are working and queue is empty - we're done
            pthread_cond_broadcast(&url_queue.cond); // Wake up other waiting threads
            pthread_mutex_unlock(&url_queue.lock);
            return NULL;
        }
        
        // Other threads are still working, so wait for them to add more URLs
        pthread_cond_wait(&url_queue.cond, &url_queue.lock);
    }
}
