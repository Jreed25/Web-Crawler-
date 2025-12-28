#include "crawler.h"

// ============================================================================
// HTML PARSING (using gumbo-parser)
// ============================================================================

// Initialize a URL list
URLList *create_url_list() {
    URLList *list = malloc(sizeof(URLList));
    list->capacity = 100;
    list->count = 0;
    list->urls = malloc(sizeof(char *) * list->capacity);
    return list;
}

// Add a URL to the list
void add_url_to_list(URLList *list, const char *url) {
    // Expand array if needed
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->urls = realloc(list->urls, sizeof(char *) * list->capacity);
    }
    
    list->urls[list->count] = strdup(url);
    list->count++;
}

// Check if a string starts with a given prefix
int starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

// List of common pages that link to too many articles (causes exponential explosion)
// These generic pages should be skipped to focus the search
const char *BLACKLIST[] = {
    "Main_Page",
    "United_States",
    "United_Kingdom", 
    "England",
    "France",
    "Germany",
    "China",
    "India",
    "Japan",
    "World_War_II",
    "World_War_I",
    "Latin",
    "Ancient_Greece",
    "Ancient_Rome",
    "English_language",
    "International_Standard_Book_Number",
    "Digital_object_identifier",
    "Geographic_coordinate_system",
    "Library_of_Congress_Control_Number",
    NULL  // Sentinel value
};

// Check if a URL is in the blacklist
int is_blacklisted(const char *url) {
    for (int i = 0; BLACKLIST[i] != NULL; i++) {
        if (strstr(url, BLACKLIST[i]) != NULL) {
            return 1;  // Found in blacklist
        }
    }
    return 0;  // Not blacklisted
}

// Check if this is a valid Wikipedia article link
// We only want /wiki/ links, not special pages
int is_valid_wiki_link(const char *href) {
    // Must start with /wiki/
    if (!starts_with(href, "/wiki/")) {
        return 0;
    }
    
    // Filter out special pages
    if (strstr(href, "File:") || 
        strstr(href, "Special:") ||
        strstr(href, "Wikipedia:") ||
        strstr(href, "Help:") ||
        strstr(href, "Category:") ||
        strstr(href, "Template:") ||
        strstr(href, "Talk:") ||
        strstr(href, "Portal:")) {
        return 0;
    }
    
    return 1;
}

// Recursively search for <a> tags in the HTML tree
void search_for_links(GumboNode *node, URLList *list) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    
    // If this is an <a> tag, extract the href
    if (node->v.element.tag == GUMBO_TAG_A) {
        GumboAttribute *href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (href && is_valid_wiki_link(href->value)) {
            // Build full URL
            char full_url[1024];
            snprintf(full_url, sizeof(full_url), "https://en.wikipedia.org%s", href->value);
            
            // Remove anchor (everything after #)
            char *anchor = strchr(full_url, '#');
            if (anchor) {
                *anchor = '\0';
            }
            
            add_url_to_list(list, full_url);
        }
    }
    
    // Recursively search children
    GumboVector *children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++) {
        search_for_links((GumboNode *)children->data[i], list);
    }
}

// Parse HTML and extract Wikipedia links
// Returns a URLList containing all found links
URLList *parse_html(const char *html) {
    URLList *list = create_url_list();
    
    // Parse the HTML
    GumboOutput *output = gumbo_parse(html);
    
    // Search for links starting from the root
    search_for_links(output->root, list);
    
    // Clean up gumbo
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    
    return list;
}

// Free a URL list
void free_url_list(URLList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->urls[i]);
    }
    free(list->urls);
    free(list);
}
