# Installation Guide

This guide will help you install the required dependencies and build the Wikipedia crawler.

## Step 1: Install Dependencies

The crawler requires two libraries:
- **libcurl** - For making HTTP requests
- **gumbo-parser** - For parsing HTML

### On macOS (using Homebrew)

```bash
brew install curl gumbo-parser
```

### On Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev libgumbo-dev
```

### On Linux (Fedora/RHEL)

```bash
sudo dnf install libcurl-devel gumbo-parser-devel
```

## Step 2: Build the Crawler

Once the dependencies are installed, build the crawler:

```bash
make
```

This will create the `crawler` executable.

## Step 3: Test the Crawler

To verify it works, run:

```bash
./crawler -h
```

You should see the help message.

## Step 4: Run Your First Crawl

Try finding a path between two Wikipedia articles:

```bash
./crawler https://en.wikipedia.org/wiki/Linux https://en.wikipedia.org/wiki/Bell_Labs 3
```

## Troubleshooting

### Error: "gumbo.h file not found"

This means gumbo-parser is not installed. Make sure you've run the installation command above.

### Error: "curl/curl.h file not found"

This means libcurl is not installed. Make sure you've run the installation command above.

### Compilation warnings

Some warnings are normal and can be ignored if the program compiles successfully.

## Clean Build

If you need to rebuild from scratch:

```bash
make clean
make
```

