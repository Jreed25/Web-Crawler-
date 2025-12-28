#include "crawler.h"

// ============================================================================
// CACHING SYSTEM
// ============================================================================

// Initialize cache directory
void init_cache() {
    mkdir(CACHE_DIR, 0755);  // Create if doesn't exist
}

// Convert URL to cache filename
// Uses hash of URL as filename to avoid filesystem issues with special chars
void url_to_cache_filename(const char *url, char *filename, size_t size) {
    unsigned int hash = hash_string(url);
    snprintf(filename, size, "%s/%u.html", CACHE_DIR, hash);
}

// Try to read HTML from cache
// Returns cached HTML if found, NULL if not cached
char *read_from_cache(const char *url) {
    char filename[512];
    url_to_cache_filename(url, filename, sizeof(filename));
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        return NULL;  // Not in cache
    }
    
    // Get file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    // Read entire file
    char *content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    
    fclose(f);
    return content;
}

// Write HTML to cache
void write_to_cache(const char *url, const char *html) {
    char filename[512];
    url_to_cache_filename(url, filename, sizeof(filename));
    
    FILE *f = fopen(filename, "w");
    if (f) {
        fputs(html, f);
        fclose(f);
    }
}
