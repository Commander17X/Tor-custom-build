# SN/Onion Search Engine

This directory contains the code for a specialized search engine designed to crawl and index websites hosted exclusively on the Tor network as `.onion` hidden services and the custom `.sn` network variant (powered by the parent SN Tor fork).

## Features

*   Crawls `.onion` and `.sn` hidden services via a Tor SOCKS proxy.
*   Indexes website content (title, description, body text).
*   Provides a web UI (built with React/Vite) for searching.
*   Basic relevance ranking based on term frequency.
*   Filtering results by type (`.onion` or `.sn`).

## Technology Stack

*   **Backend:** Node.js, TypeScript, Express
*   **Frontend:** React, Vite, Tailwind CSS
*   **Crawling:** Axios (HTTP client), SocksProxyAgent (Tor proxy), Cheerio (HTML parsing)
*   **Storage/Queue:** Redis (used for URL Frontier, visited set, inverted index, page data cache)
*   **Process Management (Recommended):** PM2 or similar

## Prerequisites

*   **Node.js:** Version 18 or higher (LTS recommended). Use `nvm` to manage versions if needed.
*   **npm or yarn:** Node.js package manager.
*   **Redis Server:** A running Redis instance (defaults to `redis://127.0.0.1:6379`).
*   **SN Tor Instance:** A running instance of the custom **SN Network Tor build** from the parent directory, configured with an active `SocksPort` (defaults to `socks5h://127.0.0.1:9050`).
*   **Git:** For cloning and managing the code.

## Installation

1.  **Clone the main repository (if you haven't already):**
    ```bash
    git clone https://github.com/Commander17X/Tor-custom-build.git # Replace with your repo URL if different
    cd Tor-custom-build
    ```
2.  **Navigate to the Search Engine directory:**
    ```bash
    cd Search-engine
    ```
3.  **Install dependencies:**
    ```bash
    npm install
    # or: yarn install
    ```

## Configuration

The application requires connection details for Redis and the SN Tor SOCKS proxy. Configuration is typically handled via environment variables. You can set these directly in your shell or use a `.env` file (you might need to install `dotenv` package and add code to load it in `server.ts`).

**Required Environment Variables:**

*   `REDIS_URL`: The connection URL for your Redis server. (Defaults internally to `redis://127.0.0.1:6379` if not set, but explicit setting is recommended).
    *   Example: `export REDIS_URL="redis://127.0.0.1:6379"`
*   `TOR_SOCKS_HOST`: The hostname or IP address of the Tor SOCKS proxy. (Defaults internally to `127.0.0.1`).
    *   Example: `export TOR_SOCKS_HOST="127.0.0.1"`
*   `TOR_SOCKS_PORT`: The port number of the Tor SOCKS proxy. (Defaults internally to `9050`).
    *   Example: `export TOR_SOCKS_PORT="9050"`
*   `PORT`: The port for the backend Express server to listen on. (Defaults to `3000`).
    *   Example: `export PORT="3000"`

**Optional Environment Variables:**

*   (Add any other variables your specific implementation might need, e.g., for connecting to a different search indexer if Redis isn't used for that).

**Example using `.env` file:**

1.  Install `dotenv`: `npm install dotenv`
2.  Create a file named `.env` in the `Search-engine` directory:
    ```dotenv
    REDIS_URL=redis://127.0.0.1:6379
    TOR_SOCKS_HOST=127.0.0.1
    TOR_SOCKS_PORT=9050
    PORT=3000
    # INDEXER_URL=http://localhost:9200 # Example if using Elasticsearch
    ```
3.  Modify `src/server.ts` (near the top) to load the variables:
    ```typescript
    import dotenv from 'dotenv';
    dotenv.config();

    import express from 'express';
    // ... rest of the imports
    ```

## Building

To create a production build (compiles TypeScript to JavaScript, bundles frontend):

```bash
npm run build
```
This generates the runnable code in the `dist/` directory (`dist/src` for backend, `dist/assets` and `dist/index.html` for frontend).

## Running

### Development Mode

For development, this starts the Vite frontend dev server (typically on port 5173 or 3050) with hot-reloading and the backend Node.js server (on port 3000) using `tsx` for automatic restarts on changes.

```bash
npm run dev:all
```

*   Access the frontend at the URL provided by Vite (e.g., `http://localhost:3050`).
*   Ensure Redis and SN Tor are running.

### Production Mode

**Prerequisites:**
*   Ensure Redis and SN Tor are running.
*   Run `npm run build` first.

1.  **Start the Backend:** Use a process manager like `pm2` for reliability. Make sure you are in the `Search-engine` directory.
    ```bash
    pm2 start dist/src/server.js --name search-engine-backend
    ```
    *   Check status: `pm2 status`
    *   View logs: `pm2 logs search-engine-backend`

2.  **Serve the Frontend:** You need a separate web server (like Nginx) to serve the static frontend files (`dist/index.html`, `dist/assets/*`) and proxy API requests (`/api/*`) to the backend running on port 3000.

    *   **Standalone:** Configure Nginx (or Caddy, Apache) manually.
    *   **Via `sn_website_manager`:** Refer to the **Deployment via `sn_website_manager`** section below.

## Deployment via `sn_website_manager`

This allows hosting the search engine as a Tor Hidden Service (`.onion` and `.sn`).

1.  **Build:** Run `npm run build` in the `Search-engine` directory.
2.  **Start Backend:** Use `pm2 start dist/src/server.js --name search-engine-backend` to run the backend persistently. Ensure Redis is also running.
3.  **Create Site:** Use the parent project's tool: `sudo /path/to/sn_website_manager create snsearch` (replace `snsearch` with your desired site name). Note the `.onion` address generated.
4.  **Copy Frontend Files:** Copy the built frontend assets to the web root created by the manager:
    ```bash
    # Copy index.html and the assets directory
    sudo cp ~/Search-engine/dist/index.html /var/www/sn/snsearch/
    sudo cp -r ~/Search-engine/dist/assets /var/www/sn/snsearch/
    # Adjust paths and site name as needed
    sudo chown -R www-data:www-data /var/www/sn/snsearch # Optional: ensure permissions
    ```
5.  **Configure Nginx Reverse Proxy:** **Manually edit** the Nginx config created by the manager (e.g., `/etc/nginx/conf.d/snsearch.conf`). Configure it to:
    *   Serve static files from `/var/www/sn/snsearch`.
    *   Use `try_files $uri $uri/ /index.html;` for the `/` location to support client-side routing.
    *   Add a `location /api/ { ... }` block to `proxy_pass` requests to `http://localhost:3000` (where your `pm2`-managed backend is running). See the main project README or standard Nginx reverse proxy examples.
6.  **Test & Reload Nginx:**
    ```bash
    sudo nginx -t
    sudo systemctl reload nginx
    ```
7.  **Access:** Visit the `.onion` address via Tor Browser or the `.sn` address via a client using the SN Tor build.

## Code Structure (`src/`)

*   `components/`: React UI components for the frontend.
*   `crawler/`: Logic for fetching pages via Tor/SOCKS proxy and parsing content.
*   `indexer/`: Logic for processing crawled data and updating the search index (Redis).
*   `queryProcessor/`: Logic for handling search queries against the index.
*   `server.ts`: Backend Express server setup, API endpoints.
*   `types/`: Shared TypeScript type definitions.
*   `urlFrontier/`: Manages the queue of URLs to crawl and the set of visited URLs (Redis).
*   `utils/`: Utility functions, Redis client setup.
*   `App.tsx`, `main.tsx`, `index.css`: Frontend entry points and main styles.

## Warnings & Disclaimers

*   **Complexity:** Building and maintaining a search engine, especially for the volatile Tor network, is challenging.
*   **Resource Intensive:** Crawling over Tor is slow. Storage and indexing require resources.
*   **Content:** The Tor network hosts diverse content. Implement filtering/moderation carefully and be aware of legal/ethical risks. Indexing illegal content is strongly discouraged.
*   **Security:** Harden the server running the crawler and backend.

## TODO / Future Enhancements

*   More sophisticated ranking algorithms.
*   Robust content filtering/moderation.
*   Improved error handling and retry logic for crawler.
*   Rate limiting for crawling.
*   Respecting `robots.txt`.
*   User interface improvements.
*   Scalability enhancements (e.g., distributed crawling). 