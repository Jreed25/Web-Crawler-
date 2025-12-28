#include "crawler.h"

// Global variables
URLQueue url_queue;                 // The shared queue of URLs to process
VisitedSet visited_set;             // Set of URLs already visited
char *target_url;                   // The destination URL we're searching for
int max_depth;                      // Maximum depth to search

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(int argc, char *argv[]) {
    // Check for help flag
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        printf("USAGE: crawler <url-1> <url-2> <depth>\n");
        printf("\n");
        printf("Arguments:\n");
        printf("  <url-1>   Starting Wikipedia article URL\n");
        printf("  <url-2>   Target Wikipedia article URL\n");
        printf("  <depth>   Maximum depth to search\n");
        printf("\n");
        printf("Example:\n");
        printf("  crawler https://en.wikipedia.org/wiki/Linux ");
        printf("https://en.wikipedia.org/wiki/Rutgers_University-Camden 6\n");
        return 0;
    }
    
    // Check correct number of arguments
    if (argc != 4) {
        fprintf(stderr, "Error: Invalid number of arguments\n");
        fprintf(stderr, "Usage: %s <url-1> <url-2> <depth>\n", argv[0]);
        fprintf(stderr, "Use '%s -h' for more information\n", argv[0]);
        return 1;
    }
    
    // Parse arguments
    char *start_url = argv[1];
    target_url = argv[2];
    max_depth = atoi(argv[3]);
    
    // Validate depth
    if (max_depth <= 0) {
        fprintf(stderr, "Error: Depth must be a positive number\n");
        return 1;
    }
    
    // Record start time
    time_t start_time = time(NULL);
    
    // Initialize curl globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Initialize data structures
    init_queue();
    init_visited_set();
    init_cache();
    
    printf("Finding path from %s to %s.\n\n", start_url, target_url);
    
    // Mark start URL as visited and add to queue
    mark_visited(start_url);
    enqueue(start_url, 0, NULL);
    
    // Create worker threads
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, crawl_worker, NULL) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            return 1;
        }
    }
    
    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n");
    
    // Check if we found the target
    if (url_queue.found && url_queue.target_node != NULL) {
        print_path(url_queue.target_node);
    } else {
        printf("No path found from %s to %s.\n", start_url, target_url);
    }
    
    // Calculate and display runtime
    time_t end_time = time(NULL);
    double elapsed = difftime(end_time, start_time);
    
    printf("\n");
    printf("Total runtime: %.2f seconds\n", elapsed);
    
    // Cleanup
    curl_global_cleanup();
    
    return 0;
}
