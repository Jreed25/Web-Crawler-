# Testing Guide

This document explains how to test your Wikipedia crawler to ensure it's working correctly.

## Before Testing

Make sure you've:
1. Installed dependencies (see INSTALL.md)
2. Built the crawler with `make`

## Test 1: Help Message

Test that the help message displays correctly:

```bash
./crawler -h
```

**Expected Output:**
```
USAGE: crawler <url-1> <url-2> <depth>

Arguments:
  <url-1>   Starting Wikipedia article URL
  <url-2>   Target Wikipedia article URL
  <depth>   Maximum depth to search

Example:
  crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Rutgers_University-Camden 6
```

## Test 2: Short Path (Easy)

Test with two closely related articles:

```bash
./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Unix 2
```

**What to expect:**
- Should find a path quickly (within seconds)
- Linux likely links directly to Unix or through one intermediate article

## Test 3: Medium Path

Test with moderately related articles:

```bash
./crawler https://en.wikipedia.org/wiki/Python_(programming_language) https://en.wikipedia.org/wiki/Computer_science 3
```

**What to expect:**
- Should find a path within 3 hops
- Takes longer than Test 2

## Test 4: Assignment Example

Test with the exact example from your assignment:

```bash
./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Rutgers_University-Camden 6
```

**Expected Output:**
```
Finding path from https://en.wikipedia.org/wiki/Linux to https://en.wikipedia.org/wiki/Rutgers_University-Camden.

Crawling: https://en.wikipedia.org/wiki/Linux (depth 0)
[... more crawling output ...]

https://en.wikipedia.org/wiki/Linux
https://en.wikipedia.org/wiki/Bell_Labs
https://en.wikipedia.org/wiki/Murray_Hill,_New_Jersey
https://en.wikipedia.org/wiki/New_Jersey
https://en.wikipedia.org/wiki/Rutgers_University-Camden
```

**Note:** The exact path may vary as Wikipedia content changes.

## Test 5: No Path Found

Test with a depth that's too small:

```bash
./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Rutgers_University-Camden 2
```

**Expected Output:**
```
Finding path from https://en.wikipedia.org/wiki/Linux to https://en.wikipedia.org/wiki/Rutgers_University-Camden.

Crawling: https://en.wikipedia.org/wiki/Linux (depth 0)
[... crawling output ...]

No path found from https://en.wikipedia.org/wiki/Linux to https://en.wikipedia.org/wiki/Rutgers_University-Camden.
```

## Test 6: Invalid Arguments

Test error handling:

```bash
./crawler
```

**Expected:** Error message about invalid arguments

```bash
./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Unix 0
```

**Expected:** Error about depth being positive

## Understanding the Output

While crawling, you'll see messages like:
```
Crawling: https://en.wikipedia.org/wiki/Linux (depth 0)
Crawling: https://en.wikipedia.org/wiki/Unix (depth 1)
```

This shows which pages are being visited and at what depth level.

### What the Program Does

1. **Starts** at the first URL (depth 0)
2. **Fetches** the HTML content of that page
3. **Extracts** all Wikipedia article links from the page
4. **Adds** those links to a queue to be processed (at depth 1)
5. **Repeats** for each URL in the queue
6. **Stops** when:
   - The target URL is found (success!)
   - Maximum depth is reached (no path found)
   - No more URLs to process (no path found)

### Multithreading in Action

The crawler uses 4 threads, so you'll see multiple URLs being crawled simultaneously. The output might be interleaved from different threads.

## Performance Notes

- **Depth 1-2:** Very fast (seconds)
- **Depth 3-4:** Moderate (10-30 seconds)
- **Depth 5-6:** Can take 1-5 minutes depending on the articles
- **Higher depths:** Can take much longer due to exponential growth

## Common Issues

### Takes Too Long

If a crawl is taking too long, you can:
- Press Ctrl+C to stop it
- Try a smaller depth value
- Use more closely related articles

### Network Errors

If you see many "Error fetching..." messages:
- Check your internet connection
- Wikipedia might be rate-limiting you
- Try again in a few minutes

### No Path Found

This can happen when:
- The depth is too small
- The articles are in very different domains
- One article has few outgoing links

## Grading Checklist

Make sure your implementation demonstrates:

- [x] **Multithreading:** 4 worker threads fetching pages concurrently
- [x] **Thread-safe queue:** URLs managed safely across threads
- [x] **Depth control:** Respects the maximum depth parameter
- [x] **Path tracking:** Stores parent pointers for path reconstruction
- [x] **Error handling:** Handles network failures gracefully
- [x] **Synchronization:** Uses mutexes and condition variables
- [x] **HTML parsing:** Extracts Wikipedia links correctly
- [x] **Visited tracking:** Avoids revisiting the same page
- [x] **Proper output:** Displays the path when found
- [x] **Help message:** -h flag shows usage

