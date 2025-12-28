#include "crawler.h"

// ============================================================================
// PRIORITY CALCULATION
// ============================================================================

// Calculate priority score for a URL based on relevance to target
// Higher score = more likely to lead to target
int calculate_priority(const char *url, const char *target) {
    int score = 0;
    
    // Extract article names from URLs (after last /)
    const char *url_name = strrchr(url, '/');
    const char *target_name = strrchr(target, '/');
    
    if (!url_name || !target_name) {
        return 0;
    }
    
    url_name++;  // Skip the /
    target_name++;
    
    // Convert to lowercase for comparison
    char url_lower[512], target_lower[512];
    snprintf(url_lower, sizeof(url_lower), "%s", url_name);
    snprintf(target_lower, sizeof(target_lower), "%s", target_name);
    
    for (int i = 0; url_lower[i]; i++) {
        if (url_lower[i] >= 'A' && url_lower[i] <= 'Z') {
            url_lower[i] = url_lower[i] + 32;
        }
    }
    for (int i = 0; target_lower[i]; i++) {
        if (target_lower[i] >= 'A' && target_lower[i] <= 'Z') {
            target_lower[i] = target_lower[i] + 32;
        }
    }
    
    // Strategy 1: Check for exact match (shouldn't happen but just in case)
    if (strcmp(url_lower, target_lower) == 0) {
        return 1000;
    }
    
    // Strategy 2: Count common words (split by underscore)
    char *url_copy = strdup(url_lower);
    char *target_copy = strdup(target_lower);
    
    char *url_token = strtok(url_copy, "_");
    while (url_token != NULL) {
        // Longer words are more significant
        int word_len = strlen(url_token);
        if (word_len > 3 && strstr(target_copy, url_token) != NULL) {
            score += word_len * 2;  // Weight by word length
        }
        url_token = strtok(NULL, "_");
    }
    
    free(url_copy);
    free(target_copy);
    
    // Strategy 3: Boost geographic/institutional pages if target is geographic/institutional
    // For example: if target is "Rutgers_University-Camden", boost other university/NJ pages
    if (strstr(target_lower, "university") && strstr(url_lower, "university")) {
        score += 20;
    }
    if (strstr(target_lower, "college") && strstr(url_lower, "college")) {
        score += 20;
    }
    if (strstr(target_lower, "new_jersey") || strstr(target_lower, "camden")) {
        if (strstr(url_lower, "new_jersey") || strstr(url_lower, "jersey") || 
            strstr(url_lower, "camden") || strstr(url_lower, "philadelphia")) {
            score += 30;  // Geographic proximity
        }
    }
    
    // Strategy 4: Penalize very long URLs (often too specific)
    int url_len = strlen(url_name);
    if (url_len > 50) {
        score -= 5;
    }
    
    return score;
}
