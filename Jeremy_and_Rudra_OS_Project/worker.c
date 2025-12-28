#include "crawler.h"

// ============================================================================
// THREAD WORKER FUNCTION
// ============================================================================

// Worker thread function - each thread runs this
void *crawl_worker(void *arg) {
    (void)arg; // Unused parameter
    
    while (1) {
        // Increment active thread count before trying to get work
        pthread_mutex_lock(&url_queue.lock);
        url_queue.active_threads++;
        pthread_mutex_unlock(&url_queue.lock);
        
        // Get a URL from the queue
        URLQueueNode *node = dequeue();
        
        // Decrement active thread count
        pthread_mutex_lock(&url_queue.lock);
        url_queue.active_threads--;
        pthread_mutex_unlock(&url_queue.lock);
        
        // If no URL, we're done
        if (node == NULL) {
            break;
        }
        
        // Check if we've already reached max depth
        if (node->depth >= max_depth) {
            continue;
        }
        
        printf("Crawling: %s (depth %d)\n", node->url, node->depth);
        
        // Fetch the HTML content
        char *html = fetch_url(node->url);
        if (html == NULL) {
            // Error fetching - skip this URL
            continue;
        }
        
        // Parse HTML to extract links
        URLList *links = parse_html(html);
        free(html);
        
        // Process each link found
        for (int i = 0; i < links->count; i++) {
            char *link = links->urls[i];
            
            // Skip blacklisted URLs (common pages that lead everywhere)
            if (is_blacklisted(link)) {
                continue;
            }
            
            // Check if we've already visited this URL
            if (is_visited(link)) {
                continue;
            }
            
            // Mark as visited
            mark_visited(link);
            
            // Check if this is the target URL
            if (strcmp(link, target_url) == 0) {
                // Found it! Signal all threads to stop
                pthread_mutex_lock(&url_queue.lock);
                url_queue.found = 1;
                
                // Create a node for the target so we can backtrack
                URLQueueNode *target_node = malloc(sizeof(URLQueueNode));
                target_node->url = strdup(link);
                target_node->depth = node->depth + 1;
                target_node->parent = node;
                target_node->next = NULL;
                
                url_queue.target_node = target_node;
                
                // Wake up all waiting threads
                pthread_cond_broadcast(&url_queue.cond);
                pthread_mutex_unlock(&url_queue.lock);
                
                free_url_list(links);
                return NULL;
            }
            
            // Add to queue for processing
            enqueue(link, node->depth + 1, node);
        }
        
        free_url_list(links);
    }
    
    return NULL;
}
