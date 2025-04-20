# SN/Onion Search Engine

[English](#english-technical-details) | [Nederlands](#nederlands-technische-details)

---

## English Technical Details <a name="english-technical-details"></a>

### 1. Overview

This application implements a specialized search engine designed exclusively for the Tor network. It discovers, crawls, indexes, and provides search capabilities for websites hosted as `.onion` hidden services and the custom `.sn` domain variant facilitated by the parent SN Tor custom build. It operates independently of the clearnet.

The core challenge lies in interacting with the Tor network for all operations, handling the inherent latency and volatility of hidden services, and managing the unique `.sn` domain resolution provided by the companion Tor fork.

### 2. Architecture & Technology

The system comprises several interconnected components:

*   **Backend API Server (`src/server.ts`):** Built with Node.js, TypeScript, and Express. It handles API requests for searching (`/api/search`) and potentially manual crawl initiation (`/api/crawl`). It also orchestrates the crawler process.
*   **Crawler (`src/crawler/index.ts`):** Responsible for fetching web pages. Uses `axios` for HTTP requests, critically routed through `socks-proxy-agent` configured to use the **SN Tor instance's SOCKS port**. This is essential for accessing `.onion` and `.sn` addresses. It uses `cheerio` for server-side HTML parsing to extract text content and links.
*   **URL Frontier (`src/urlFrontier/index.ts`):** Manages the queue of URLs pending crawling (`url:queue` set in Redis) and tracks already visited URLs (`url:visited` set in Redis) to prevent duplicate work. Uses Redis sets (`SADD`, `SPOP`, `SISMEMBER`, `SCARD`).
*   **Indexer (`src/indexer/index.ts`):** Processes the data extracted by the crawler. Stores page metadata (title, description, URL, content hash, etc.) likely in a Redis hash (`index:{url}`). It also builds a basic inverted index by extracting terms from the content and storing mappings in Redis sets (`term:{term}` -> set of URLs).
*   **Query Processor (`src/queryProcessor/index.ts`):** Handles search requests from the API. It tokenizes the query, looks up terms in the Redis inverted index (`term:{term}` sets), aggregates potential URL matches, retrieves full document data from the index (`index:{url}` hash), performs basic relevance scoring (e.g., based on term overlap), applies filters (like type: onion/sn), and returns the results.
*   **Frontend (`src/` components, `App.tsx`, `main.tsx`):** A Single Page Application (SPA) built with React and Vite, styled with Tailwind CSS. It provides the user interface for submitting search queries and displaying results. It interacts with the Backend API Server via `/api/search` requests.
*   **Storage (`src/utils/redis.ts`):** Redis is heavily utilized as the primary data store for the URL frontier, visited set, page index, and inverted term index.

### 3. Core Dependencies & Setup

*   **System:** Node.js (v18+ recommended, check via `node -v`), npm/yarn, Git.
*   **Services:**
    *   **Redis:** A running Redis server instance is mandatory. The application attempts to connect to `redis://127.0.0.1:6379` by default.
    *   **SN Tor Build:** A running instance of the custom Tor build from the parent project is absolutely essential. Its SOCKS proxy (e.g., `socks5h://127.0.0.1:9050`) must be active and reachable by the crawler.
*   **Installation:**
    1.  Clone the parent repository (`git clone <repo_url>`).
    2.  Navigate to this directory (`cd Search-engine`).
    3.  Install Node.js dependencies (`npm install` or `yarn install`).

### 4. Configuration

Configuration is primarily managed via environment variables, typically loaded using a `.env` file at the root of the `Search-engine` directory (requires installing `dotenv` and modifying `src/server.ts` to load it) or by exporting variables in the shell.

*   `REDIS_URL`: Connection string for Redis (Default: `redis://127.0.0.1:6379`).
*   `TOR_SOCKS_HOST`: Host/IP of the SN Tor SOCKS proxy (Default: `127.0.0.1`).
*   `TOR_SOCKS_PORT`: Port of the SN Tor SOCKS proxy (Default: `9050`).
*   `PORT`: Port for the backend Express server (Default: `3000`).

### 5. Building & Running

*   **Development:**
    *   Command: `npm run dev:all`
    *   Action: Starts the Vite frontend dev server (with HMR, e.g., `http://localhost:3050`) and the backend server using `tsx` (Node.js/TypeScript runner with watch mode, typically on port 3000). Requires Redis and SN Tor to be running.
    *   Access: Open the browser to the Vite server URL (e.g., `http://localhost:3050`).
*   **Production Build:**
    *   Command: `npm run build`
    *   Action: Compiles TypeScript to JavaScript (`tsc`), builds the frontend static assets using Vite, and compiles the backend TypeScript using `tsc --project tsconfig.node.json`. Output is placed in the `dist/` directory.
*   **Production Execution:**
    *   Requires: Successful `npm run build`, running Redis, running SN Tor.
    *   **Backend:** Start the compiled server persistently using a process manager:
        ```bash
        # Ensure you are in the Search-engine directory
        pm2 start dist/src/server.js --name search-engine-backend 
        ```
    *   **Frontend:** The static files (`dist/index.html`, `dist/assets/*`) must be served by a dedicated web server (like Nginx or Caddy). This server also needs to be configured to reverse proxy requests for `/api/*` to the backend running on its specified port (e.g., `http://localhost:3000`).

### 6. Deployment as Tor Hidden Service (via `sn_website_manager`)

This outlines deploying the production build as a hidden service.

1.  **Build:** Execute `npm run build`.
2.  **Start Backend:** Launch `dist/src/server.js` using `pm2` (as described above). Ensure Redis is running.
3.  **Create Service:** Use the parent project's tool (`sudo /path/to/sn_website_manager create <sitename>`).
4.  **Deploy Frontend:** Copy contents of `Search-engine/dist/` (specifically `index.html` and `assets/`) to the webroot created by the manager (e.g., `/var/www/sn/<sitename>/`).
5.  **Configure Nginx:** **Crucially, manually edit** the Nginx configuration file generated by `sn_website_manager`.
    *   Set the `root` directive to the webroot (e.g., `/var/www/sn/<sitename>`).
    *   In the `/` location block, use `try_files $uri $uri/ /index.html;` to support client-side routing.
    *   Add a `location /api/ { ... }` block to `proxy_pass` requests to the backend service (e.g., `http://localhost:3000`).
6.  **Verify & Reload:** Test Nginx config (`sudo nginx -t`) and reload (`sudo systemctl reload nginx`).
7.  **Access:** Use the `.onion` or `.sn` address provided.

### 7. Code Structure (`src/`)

*   `components/`: React frontend components.
*   `crawler/`: Tor/SOCKS web page fetching and parsing.
*   `indexer/`: Data processing and Redis index updates.
*   `queryProcessor/`: Search query handling.
*   `server.ts`: Backend Express application.
*   `types/`: Shared TypeScript types.
*   `urlFrontier/`: URL queue and visited set management.
*   `utils/`: Utilities (e.g., Redis client setup).
*   `App.tsx`, `main.tsx`, `index.css`: Frontend application shell, entry point, base styles.

### 8. Important Considerations

*   **Tor Network Dependency:** The crawler *must* use the custom SN Tor build's SOCKS proxy to resolve `.sn` addresses.
*   **Performance:** Crawling over Tor is inherently slow and error-prone. Robust error handling, timeouts, and retries are necessary.
*   **Resource Usage:** Redis memory usage will grow with the index size. Crawling consumes bandwidth.
*   **Content Moderation:** Indexing hidden services carries significant risk. Implementing effective content filtering and adhering to legal requirements is critical and complex.
*   **Security:** The server hosting the application, Redis, and Tor instance must be appropriately secured.

### 9. Future Enhancements

*   Advanced ranking (TF-IDF, BM25+).
*   Content filtering mechanisms.
*   Distributed/parallel crawling.
*   Respecting `robots.txt`.
*   Improved UI/UX.
*   Caching layers.

---

## Nederlands Technische Details <a name="nederlands-technische-details"></a>

### 1. Overzicht

Deze applicatie implementeert een gespecialiseerde zoekmachine die exclusief is ontworpen voor het Tor-netwerk. Het ontdekt, crawlt, indexeert en biedt zoekmogelijkheden voor websites die worden gehost als `.onion` hidden services en de aangepaste `.sn` domeinvariant die wordt gefaciliteerd door de bovenliggende SN Tor custom build. Het werkt onafhankelijk van het clearnet.

De kernuitdaging ligt in de interactie met het Tor-netwerk voor alle operaties, het omgaan met de inherente latentie en volatiliteit van hidden services, en het beheren van de unieke `.sn` domeinresolutie die wordt geboden door de bijbehorende Tor-fork.

### 2. Architectuur & Technologie

Het systeem bestaat uit verschillende onderling verbonden componenten:

*   **Backend API Server (`src/server.ts`):** Gebouwd met Node.js, TypeScript en Express. Het verwerkt API-verzoeken voor zoeken (`/api/search`) en mogelijk handmatige crawl-initiatie (`/api/crawl`). Het orkestreert ook het crawlerproces.
*   **Crawler (`src/crawler/index.ts`):** Verantwoordelijk voor het ophalen van webpagina's. Gebruikt `axios` voor HTTP-verzoeken, cruciaal gerouteerd via `socks-proxy-agent` geconfigureerd om de **SOCKS-poort van de SN Tor-instantie** te gebruiken. Dit is essentieel voor toegang tot `.onion`- en `.sn`-adressen. Het gebruikt `cheerio` voor server-side HTML-parsing om tekstinhoud en links te extraheren.
*   **URL Frontier (`src/urlFrontier/index.ts`):** Beheert de wachtrij met URL's die wachten om gecrawld te worden (`url:queue` set in Redis) en houdt reeds bezochte URL's bij (`url:visited` set in Redis) om dubbel werk te voorkomen. Gebruikt Redis sets (`SADD`, `SPOP`, `SISMEMBER`, `SCARD`).
*   **Indexer (`src/indexer/index.ts`):** Verwerkt de gegevens die door de crawler zijn geëxtraheerd. Slaat paginametadata op (titel, beschrijving, URL, content-hash, enz.) waarschijnlijk in een Redis hash (`index:{url}`). Het bouwt ook een basale geïnverteerde index door termen uit de inhoud te extraheren en koppelingen op te slaan in Redis sets (`term:{term}` -> set van URL's).
*   **Query Processor (`src/queryProcessor/index.ts`):** Verwerkt zoekopdrachten van de API. Het tokeniseert de query, zoekt termen op in de Redis geïnverteerde index (`term:{term}` sets), aggregeert potentiële URL-matches, haalt volledige documentgegevens op uit de index (`index:{url}` hash), voert basale relevantiescoring uit (bijv. op basis van term-overlap), past filters toe (zoals type: onion/sn), en retourneert de resultaten.
*   **Frontend (`src/` componenten, `App.tsx`, `main.tsx`):** Een Single Page Application (SPA) gebouwd met React en Vite, gestyled met Tailwind CSS. Het biedt de gebruikersinterface voor het invoeren van zoekopdrachten en het weergeven van resultaten. Het communiceert met de Backend API Server via `/api/search`-verzoeken.
*   **Opslag (`src/utils/redis.ts`):** Redis wordt intensief gebruikt als de primaire gegevensopslag voor de URL-frontier, bezochte set, pagina-index en geïnverteerde term-index.

### 3. Kernafhankelijkheden & Setup

*   **Systeem:** Node.js (v18+ aanbevolen, controleer via `node -v`), npm/yarn, Git.
*   **Services:**
    *   **Redis:** Een draaiende Redis server-instantie is verplicht. De applicatie probeert standaard verbinding te maken met `redis://127.0.0.1:6379`.
    *   **SN Tor Build:** Een draaiende instantie van de custom Tor build uit het bovenliggende project is absoluut essentieel. De SOCKS-proxy (bijv. `socks5h://127.0.0.1:9050`) moet actief zijn en bereikbaar voor de crawler.
*   **Installatie:**
    1.  Kloon de bovenliggende repository (`git clone <repo_url>`).
    2.  Navigeer naar deze map (`cd Search-engine`).
    3.  Installeer Node.js afhankelijkheden (`npm install` of `yarn install`).

### 4. Configuratie

Configuratie wordt voornamelijk beheerd via omgevingsvariabelen, typisch geladen met een `.env`-bestand in de root van de `Search-engine` map (vereist installatie van `dotenv` en aanpassing van `src/server.ts` om het te laden) of door variabelen te exporteren in de shell.

*   `REDIS_URL`: Verbindings-URL voor Redis (Standaard: `redis://127.0.0.1:6379`).
*   `TOR_SOCKS_HOST`: Host/IP van de SN Tor SOCKS-proxy (Standaard: `127.0.0.1`).
*   `TOR_SOCKS_PORT`: Poort van de SN Tor SOCKS-proxy (Standaard: `9050`).
*   `PORT`: Poort voor de backend Express-server (Standaard: `3000`).

### 5. Bouwen & Uitvoeren

*   **Ontwikkelmodus:**
    *   Commando: `npm run dev:all`
    *   Actie: Start de Vite frontend dev-server (met HMR, bijv. `http://localhost:3050`) en de backend-server met `tsx` (Node.js/TypeScript runner met watch-modus, typisch op poort 3000). Vereist dat Redis en SN Tor draaien.
    *   Toegang: Open de browser naar de Vite server-URL (bijv. `http://localhost:3050`).
*   **Productie Build:**
    *   Commando: `npm run build`
    *   Actie: Compileert TypeScript naar JavaScript (`tsc`), bouwt de frontend statische assets met Vite, en compileert de backend TypeScript met `tsc --project tsconfig.node.json`. Output wordt geplaatst in de `dist/` map.
*   **Productie Uitvoering:**
    *   Vereist: Succesvolle `npm run build`, draaiende Redis, draaiende SN Tor.
    *   **Backend:** Start de gecompileerde server persistent met een process manager:
        ```bash
        # Zorg ervoor dat je in de Search-engine map bent
        pm2 start dist/src/server.js --name search-engine-backend
        ```
    *   **Frontend:** De statische bestanden (`dist/index.html`, `dist/assets/*`) moeten worden geserveerd door een dedicated webserver (zoals Nginx of Caddy). Deze server moet ook geconfigureerd zijn om verzoeken voor `/api/*` via een reverse proxy door te sturen naar de backend die draait op de opgegeven poort (bijv. `http://localhost:3000`).

### 6. Deployment als Tor Hidden Service (via `sn_website_manager`)

Dit beschrijft het deployen van de productie-build als een hidden service.

1.  **Build:** Voer `npm run build` uit.
2.  **Start Backend:** Lanceer `dist/src/server.js` met `pm2` (zoals hierboven beschreven). Zorg ervoor dat Redis draait.
3.  **Creëer Service:** Gebruik de tool van het bovenliggende project (`sudo /path/to/sn_website_manager create <sitename>`).
4.  **Deploy Frontend:** Kopieer de inhoud van `Search-engine/dist/` (specifiek `index.html` en `assets/`) naar de webroot die door de manager is aangemaakt (bijv. `/var/www/sn/<sitename>/`).
5.  **Configureer Nginx:** **Cruciaal, bewerk handmatig** het Nginx-configuratiebestand dat door `sn_website_manager` is gegenereerd.
    *   Stel de `root` directive in op de webroot (bijv. `/var/www/sn/<sitename>`).
    *   Gebruik in het `/` location block `try_files $uri $uri/ /index.html;` om client-side routing te ondersteunen.
    *   Voeg een `location /api/ { ... }` block toe om verzoeken via `proxy_pass` door te sturen naar de backend service (bijv. `http://localhost:3000`).
6.  **Verifieer & Herlaad:** Test Nginx config (`sudo nginx -t`) en herlaad (`sudo systemctl reload nginx`).
7.  **Toegang:** Gebruik het `.onion`- of `.sn`-adres.

### 7. Codestructuur (`src/`)

*   `components/`: React frontend componenten.
*   `crawler/`: Tor/SOCKS webpagina ophalen en parsen.
*   `indexer/`: Gegevensverwerking en Redis index updates.
*   `queryProcessor/`: Afhandeling van zoekopdrachten.
*   `server.ts`: Backend Express applicatie.
*   `types/`: Gedeelde TypeScript types.
*   `urlFrontier/`: URL-wachtrij en beheer van bezochte sets.
*   `utils/`: Hulpprogramma's (bijv. Redis client setup).
*   `App.tsx`, `main.tsx`, `index.css`: Frontend applicatie shell, entry point, basisstijlen.

### 8. Belangrijke Overwegingen

*   **Tor Netwerk Afhankelijkheid:** De crawler *moet* de SOCKS-proxy van de custom SN Tor build gebruiken om `.sn`-adressen op te lossen.
*   **Prestaties:** Crawlen over Tor is inherent traag en foutgevoelig. Robuuste foutafhandeling, timeouts en retries zijn noodzakelijk.
*   **Resourcegebruik:** Redis geheugengebruik groeit met de indexgrootte. Crawlen verbruikt bandbreedte.
*   **Content Moderatie:** Het indexeren van hidden services brengt aanzienlijke risico's met zich mee. Effectieve content filtering implementeren en voldoen aan wettelijke eisen is cruciaal en complex.
*   **Beveiliging:** De server die de applicatie, Redis en de Tor-instantie host, moet adequaat beveiligd zijn.

### 9. Toekomstige Verbeteringen

*   Geavanceerdere ranking-algoritmen (TF-IDF, BM25+).
*   Content filtering mechanismen.
*   Gedistribueerd/parallel crawlen.
*   Respecteren van `robots.txt`.
*   Verbeterde UI/UX.
*   Caching lagen. 