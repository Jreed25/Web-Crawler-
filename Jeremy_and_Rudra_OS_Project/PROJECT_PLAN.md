# Multithreaded Wikipedia Crawler Implementation Plan

## Project Overview
Build a multithreaded web crawler in C that finds paths between Wikipedia articles using libcurl for HTTP requests, gumbo-parser for HTML parsing, and pthreads for concurrent URL fetching.

## Project Structure Setup

Create project directory at `/Users/rudraasakariya/Desktop/os-project/` with:
- `crawler.c` - Main implementation file
- `Makefile` - Build configuration
- `.gitignore` - Ignore compiled binaries
- `README.md` - Basic usage instructions

Initialize git repository to track all changes.

## Core Implementation Components

### 1. Data Structures (Top of crawler.c)

**URLQueueNode** - Enhanced queue node with:
- `char *url` - The URL string
- `int depth` - Current depth level
- `struct URLQueueNode *parent` - Parent node for path reconstruction
- `struct URLQueueNode *next` - Next in queue

**URLQueue** - Thread-safe queue with:
- Head and tail pointers
- `pthread_mutex_t lock` - Queue access mutex
- `pthread_cond_t cond` - Condition variable for thread coordination
- `int active_threads` - Track working threads
- `int found` - Flag when target is found
- `URLQueueNode *target_node` - Store target node for backtracking

**VisitedSet** - Hash table to track visited URLs:
- Simple array-based hash table (size ~10000)
- Chaining for collision resolution
- Mutex for thread-safe access

### 2. URL Queue Operations

- `initQueue()` - Initialize queue and synchronization primitives
- `enqueue()` - Add URL with depth and parent tracking
- `dequeue()` - Remove URL (blocking wait if empty, return NULL if done)
- `isVisited()` - Check if URL already visited
- `markVisited()` - Add URL to visited set

### 3. HTTP and HTML Parsing

**fetch_and_parse()** function:
- Use libcurl to fetch Wikipedia page content
- Handle HTTP errors gracefully
- Parse HTML with gumbo-parser
- Extract only Wikipedia article links (`/wiki/` paths)
- Filter out special pages (File:, Special:, Wikipedia:, Help:, etc.)
- Return list of found links

### 4. Thread Worker Function

**crawl_worker()** - Main thread function:
- Loop: dequeue URL from queue
- Check if target URL found (if so, signal and exit)
- Fetch and parse the page
- For each extracted link:
  - Check depth limit
  - Check if visited
  - Check if matches target URL
  - Enqueue new URLs with incremented depth and parent pointer
- Handle errors and continue
- Exit when queue empty and all threads idle

### 5. Path Reconstruction

**print_path()** function:
- Start from target node
- Walk back through parent pointers
- Store path in array
- Print in forward order (source to target)

### 6. Main Function Logic

- Parse command line arguments: `<url-1> <url-2> <depth>`
- Handle `-h` help flag
- Validate Wikipedia URLs
- Initialize queue and visited set
- Enqueue starting URL (depth 0, no parent)
- Create worker threads (4 threads)
- Wait for threads to complete
- Check if path found, print result
- Cleanup resources

### 7. Synchronization Strategy

- Queue mutex for enqueue/dequeue operations
- Condition variable to wake threads when URLs added
- Visited set mutex for checking/adding URLs
- Global flag when target found (stop all threads)
- Track active thread count to know when crawling complete

### 8. Error Handling and Logging

- Log to stderr for errors
- Log to stdout for progress (URLs visited)
- Handle: network failures, invalid URLs, parsing errors
- Use simple printf statements with clear messages

## Makefile Contents

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
LIBS = -lcurl -lgumbo

all: crawler

crawler: crawler.c
	$(CC) $(CFLAGS) -o crawler crawler.c $(LIBS)

clean:
	rm -f crawler

test:
	./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Rutgers_University-Camden 6
```

## Code Style Guidelines

- Add comments before each function explaining purpose
- Comment complex logic blocks
- Use descriptive variable names
- Keep functions reasonably sized (< 100 lines)
- Simple, readable C code (student-level complexity)
- Consistent indentation (4 spaces)

## Testing Steps

1. Compile: `make`
2. Test help: `./crawler -h`
3. Test short path: `./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Bell_Labs 3`
4. Test longer path: Use the example from assignment
5. Test no path found: Use depth 2 with distant articles

## Dependencies Installation

User will need to install:
- libcurl development library
- gumbo-parser development library

Instructions for macOS (Homebrew):
```bash
brew install curl gumbo-parser
```

## Implementation Checklist

- [ ] Create project directory and initialize git repository
- [ ] Implement queue node, URL queue, and visited set structures
- [ ] Write queue init, enqueue, dequeue, and visited tracking functions
- [ ] Implement HTTP fetching with libcurl and HTML parsing with gumbo
- [ ] Create crawl_worker function with main thread logic
- [ ] Implement path backtracking and printing
- [ ] Write main function with argument parsing and thread management
- [ ] Create Makefile for building the project
- [ ] Add README and comments throughout code
- [ ] Test with various Wikipedia article pairs and depths

