# Multithreaded Wikipedia Crawler

A multithreaded web crawler in C that finds paths between two Wikipedia articles by following links, similar to the Wikipedia Game.

## Description

This crawler uses multiple threads to efficiently explore Wikipedia pages and find a path from one article to another. It demonstrates concepts like multithreading, synchronization, and concurrent data structures.

## Dependencies

The project requires the following libraries:

- **libcurl** - For making HTTP requests
- **gumbo-parser** - For parsing HTML content

### Installing Dependencies on macOS

```bash
brew install curl gumbo-parser
```

### Installing Dependencies on Linux (Ubuntu/Debian)

```bash
sudo apt-get install libcurl4-openssl-dev libgumbo-dev
```

## Building

To compile the crawler, simply run:

```bash
make
```

## Usage

```bash
./crawler <url-1> <url-2> <depth>
```

- `<url-1>`: Starting Wikipedia article URL
- `<url-2>`: Target Wikipedia article URL
- `<depth>`: Maximum depth to search

### Examples

Show help:
```bash
./crawler -h
```

Find path between Linux and Rutgers-Camden:
```bash
./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Rutgers_University-Camden 6
```

## Features

- **Multithreading**: Uses 4 worker threads to fetch pages concurrently
- **Thread-safe queue**: Manages URLs to be crawled across threads
- **Depth control**: Limits how deep the crawler explores
- **Path tracking**: Remembers the path taken to reach each URL
- **Duplicate detection**: Avoids visiting the same page twice

## Project Structure

- `crawler.c` - Main source code
- `Makefile` - Build configuration
- `README.md` - This file

## Author

Created as a final project for Operating Systems course.

