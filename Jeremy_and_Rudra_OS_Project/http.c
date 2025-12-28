#include "crawler.h"

// ============================================================================
// HTTP FETCHING (using libcurl)
// ============================================================================

// Callback function for libcurl to write received data
// This gets called by libcurl as data arrives
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    HttpResponse *response = (HttpResponse *)userp;
    
    // Expand the buffer to fit the new data
    char *ptr = realloc(response->data, response->size + real_size + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Not enough memory to store response\n");
        return 0;
    }
    
    response->data = ptr;
    // Copy the new data to the end of the buffer
    memcpy(&(response->data[response->size]), contents, real_size);
    response->size += real_size;
    response->data[response->size] = 0; // Null terminate
    
    return real_size;
}

// Fetch the HTML content of a URL (with caching)
// Returns the HTML as a string, or NULL on error
char *fetch_url(const char *url) {
    // Try to read from cache first
    char *cached = read_from_cache(url);
    if (cached != NULL) {
        return cached;  // Cache hit!
    }
    
    // Not in cache - fetch from network
    CURL *curl;
    CURLcode res;
    HttpResponse response;
    
    // Initialize response buffer
    response.data = malloc(1);
    response.size = 0;
    
    // Initialize curl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        free(response.data);
        return NULL;
    }
    
    // Set curl options
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; WikiCrawler/1.0)");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);       // 10 second timeout
    
    // Perform the request
    res = curl_easy_perform(curl);
    
    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "Error fetching %s: %s\n", url, curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(response.data);
        return NULL;
    }
    
    curl_easy_cleanup(curl);
    
    // Save to cache for future use
    write_to_cache(url, response.data);
    
    return response.data;
}
