#ifndef CRAWLER_H
#define CRAWLER_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <curl/curl.h>
#include <gumbo.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

// Maximum number of URLs to track in hash table
#define HASH_TABLE_SIZE 10000
// Number of worker threads
#define NUM_THREADS 4

// Structure for a node in the URL queue
// Each node stores a URL, its depth, priority, and a pointer to its parent (for path reconstruction)
typedef struct URLQueueNode {
    char *url;                      // The URL string
    int depth;                      // Depth level from starting URL
    int priority;                   // Priority score (higher = more relevant)
    struct URLQueueNode *parent;    // Parent node for backtracking the path
    struct URLQueueNode *next;      // Next node in the queue
} URLQueueNode;

// Thread-safe queue for managing URLs to be crawled
typedef struct {
    URLQueueNode *head;             // Front of queue
    URLQueueNode *tail;             // End of queue
    pthread_mutex_t lock;           // Mutex for thread-safe access
    pthread_cond_t cond;            // Condition variable for thread coordination
    int active_threads;             // Number of threads currently working
    int found;                      // Flag: 1 if target URL found, 0 otherwise
    URLQueueNode *target_node;      // Pointer to the target node when found
} URLQueue;

// Node for the hash table (visited URLs tracking)
typedef struct VisitedNode {
    char *url;                      // The URL
    struct VisitedNode *next;       // Next node in chain (for collision handling)
} VisitedNode;

// Hash table to track which URLs have been visited
typedef struct {
    VisitedNode *table[HASH_TABLE_SIZE];  // Array of linked lists
    pthread_mutex_t lock;                  // Mutex for thread-safe access
} VisitedSet;

// Structure to store the HTTP response data
typedef struct {
    char *data;      // The response content
    size_t size;     // Size of the response
} HttpResponse;

// Structure to store a list of URLs
typedef struct {
    char **urls;        // Array of URL strings
    int count;          // Number of URLs
    int capacity;       // Capacity of the array
} URLList;

// Cache directory
#define CACHE_DIR ".cache"

// Global variables
extern URLQueue url_queue;                 // The shared queue of URLs to process
extern VisitedSet visited_set;             // Set of URLs already visited
extern char *target_url;                   // The destination URL we're searching for
extern int max_depth;                      // Maximum depth to search

// Function declarations
unsigned int hash_string(const char *str);
void init_visited_set();
int is_visited(const char *url);
void mark_visited(const char *url);

int calculate_priority(const char *url, const char *target);

void init_queue();
void enqueue(const char *url, int depth, URLQueueNode *parent);
URLQueueNode *dequeue();

void init_cache();
void url_to_cache_filename(const char *url, char *filename, size_t size);
char *read_from_cache(const char *url);
void write_to_cache(const char *url, const char *html);

char *fetch_url(const char *url);

URLList *create_url_list();
void add_url_to_list(URLList *list, const char *url);
int starts_with(const char *str, const char *prefix);
int is_blacklisted(const char *url);
int is_valid_wiki_link(const char *href);
void search_for_links(GumboNode *node, URLList *list);
URLList *parse_html(const char *html);
void free_url_list(URLList *list);

void print_path(URLQueueNode *target_node);

void *crawl_worker(void *arg);

#endif