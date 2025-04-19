# Tor Custom Build / Tor Aangepaste Versie

[English](#english) | [Nederlands](#nederlands)

## English <a name="english"></a>

### What is this?

This is a **custom version (fork)** of the Tor source code. While the original Tor focuses on general anonymity, this fork is specifically adapted and optimized for:

* **Improved Hostability:** More stable and efficient hosting of services (especially Hidden Services) on the network.
* **Higher Speed:** Optimizations for faster connections and response times within the network.
* **.SN Network Integration:** Specific adaptations for better operation and interaction within custom `.sn` network environments.
* **Enhanced Security:** Additional measures and refined logic to improve security.

The basic functionality of Tor (anonymity via layered encryption and relays) is preserved, but with these specific improvements.

### New Features: Website Manager

A powerful tool for hosting websites on the .sn network with Tor integration:

```bash
# Create a new website
sudo sn_website_manager create mijnsite

# The website will be available at:
# - http://mijnsite.<onion-address>.sn
# - http://<onion-address>.onion
```

Features:
- Virtual hosting with .sn domains
- Automatic Tor hidden service setup
- DDoS protection
- Automatic port configuration
- Nginx reverse proxy
- Comprehensive logging

See [Website Manager Documentation](#website-manager) for details.

### Directory Structure

The code is organized in different directories within the `src` folder:

* `src/core`: The core of Tor, handling network connections and circuit building.
* `src/feature`: Specific features like hidden services (`hs`), directory authorities, etc.
* `src/lib`: General utility functions and libraries used throughout.
* `src/app`: Code specific to the Tor application itself.
* `src/ext`: External libraries that Tor depends on.
* `src/trunnel`: Code for parsing Tor's cells (data packets).
* `src/tools`: Useful tools for development or management.
* `src/test`: Unit tests and integration tests.
* `src/win32`: Windows-specific code.

### Building the Code

To run or test the code, you need to compile it first. On Windows, we typically use:

1. **MSYS2 with MinGW-w64**: This provides a Linux-like environment and the `gcc` compiler.
2. **Build Tools**: You'll need tools like `make`, `automake`, `autoconf`. Install these via the MSYS2 package manager (`pacman`).
3. **Dependencies**: Tor requires external libraries (like OpenSSL, Libevent, zlib). Install these via `pacman` (the `mingw-w64-x86_64-...` versions).

**Important:** During the build process, an `orconfig.h` file is generated. This file contains configuration settings specific to your system and build. Many other files need this (`#include "orconfig.h"`). **Without a successful build, your linter (and compiler) will show many errors because this file is missing!**

Typical build steps (executed in the MSYS2 MinGW 64-bit shell):

```bash
./autogen.sh  # Creates the configure script (only needed first time)
./configure   # Checks your system and sets up the build
make          # Compiles the code
```

### Development with VS Code

To make it easier to read and write code in VS Code, we have a configuration file: `.vscode/c_cpp_properties.json`.

* **What it does:** This file tells the C/C++ extension where to find `#include` files (like `orconfig.h` and other Tor headers).
* **Settings:**
    * `includePath`: Lists all directories where VS Code should look for header files.
    * `compilerPath`: Points to the `gcc.exe` compiler in your MSYS2 installation.
    * `intelliSenseMode`: Set to `windows-gcc-x64` since we're using GCC via MSYS2.

**Remember:** Even with the correct VS Code configuration, you still need to **successfully build** the code first so that `orconfig.h` exists, otherwise include errors will persist.

### Recent Updates

* Added secure server setup script with improved logging and backup functionality
* Enhanced directory authority bandwidth management
* Improved hidden service performance and security
* Added new testing framework for network components

### More Information

* Check the official Tor documentation for the most up-to-date build instructions and deeper explanations.
* Explore the `src` directory to see how different components work together.

## Nederlands <a name="nederlands"></a>

### Wat is dit?

Dit is een **aangepaste versie (fork)** van de Tor broncode. Terwijl de originele Tor focust op algemene anonimiteit, is deze fork specifiek aangepast en geoptimaliseerd voor:

```bash
./configure
make
make install
```

To build Tor from a just-cloned git repository:

```
./autogen.sh
./configure
make
make install
```

## Releases

The tarballs, checksums and signatures can be found here: https://dist.torproject.org

- Checksum: `<tarball-name>.sha256sum`
- Signatures: `<tarball-name>.sha256sum.asc`

### Schedule

You can find our release schedule here:

- https://gitlab.torproject.org/tpo/core/team/-/wikis/NetworkTeam/CoreTorReleases

### Keys that CAN sign a release

The following keys are the maintainers of this repository. One or many of
these keys can sign the releases, do NOT expect them all:

- Alexander Færøy:
    [514102454D0A87DB0767A1EBBE6A0531C18A9179](https://keys.openpgp.org/vks/v1/by-fingerprint/1C1BC007A9F607AA8152C040BEA7B180B1491921)
- David Goulet:
    [B74417EDDF22AC9F9E90F49142E86A2A11F48D36](https://keys.openpgp.org/vks/v1/by-fingerprint/B74417EDDF22AC9F9E90F49142E86A2A11F48D36)
- Nick Mathewson:
    [2133BC600AB133E1D826D173FE43009C4607B1FB](https://keys.openpgp.org/vks/v1/by-fingerprint/2133BC600AB133E1D826D173FE43009C4607B1FB)

## Development

See our hacking documentation in [doc/HACKING/](./doc/HACKING).

## Resources

Home page:

- https://www.torproject.org/

Download new versions:

- https://www.torproject.org/download/tor

How to verify Tor source:

- https://support.torproject.org/little-t-tor/

Documentation and Frequently Asked Questions:

- https://support.torproject.org/

How to run a Tor relay:

- https://community.torproject.org/relay/

## Specifieke Setup Voor Deze Workspace (Windows + VS Code)

In deze specifieke ontwikkelomgeving hebben we een paar dingen gedaan om het werken met de Tor-codebase op Windows met VS Code makkelijker te maken:

1.  **VS Code Configuratie (.vscode/c_cpp_properties.json):**
    *   We hebben dit bestand aangemaakt en geconfigureerd.
    *   Het bevat de juiste `includePath`s zodat VS Code de Tor header-bestanden kan vinden (cruciaal na het builden!).
    *   Het `compilerPath` is ingesteld voor een standaard MSYS2/MinGW-w64 installatie (`C:/msys64/mingw64/bin/gcc.exe`). **Controleer of dit pad klopt voor jouw installatie!**
    *   Dit helpt enorm bij het oplossen van include-fouten in de editor (linting) en zorgt voor betere code-suggesties (IntelliSense), **nadat** je het project succesvol hebt gebouwd.

2.  **Focus op `orconfig.h`:**
    *   We benadrukken dat het **essentieel** is om het project eerst succesvol te builden (`./configure && make` in MSYS2 MinGW shell).
    *   De build genereert het bestand `src/core/or/orconfig.h` (of een vergelijkbaar pad afhankelijk van de build output). Zonder dit bestand zullen de C/C++ extensie en de compiler klagen over missende headers.

3.  **Windows Build Uitleg:**
    *   De build-instructies hierboven zijn gericht op het gebruik van MSYS2/MinGW-w64, de gebruikelijke manier om Tor op Windows te compileren voor ontwikkeling.

**Kortom:** Zorg dat MSYS2/MinGW-w64 correct is geïnstalleerd met alle dependencies, voer de build-stappen uit (`./configure`, `make`), en controleer/herstart VS Code. Dan zou je een veel soepelere ontwikkelervaring moeten hebben!

## Kernaanpassingen in Deze Fork

Deze fork onderscheidt zich van de standaard Tor door de volgende kernaanpassingen:

*   **Focus op Hostability:**
    *   Logica rondom Hidden Service (HS) publicatie, stabiliteit en descriptor management is geoptimaliseerd voor betrouwbaarder en efficiënter hosten.
    *   Resource management voor langlopende services is verbeterd.

*   **Snelheidsoptimalisaties:**
    *   Algoritmes voor circuitbouw en node-selectie zijn aangepast voor potentieel snellere verbindingen.
    *   Communicatie overhead tussen instances is verminderd waar mogelijk.

*   **Integratie met .SN Netwerk:**
    *   Specifieke logica toegevoegd of aangepast om naadloos te werken met de naamresolutie, protocollen of topologie van een `.sn` netwerk.
    *   (Voeg hier eventueel specifiekere details toe over de .sn integratie indien bekend).

*   **Veiligheidsverbeteringen:**
    *   Bepaalde standaardinstellingen zijn mogelijk strenger geconfigureerd.
    *   Extra validaties of controles zijn toegevoegd in kritieke codepaden.
    *   (Voeg hier eventueel specifiekere details toe over de security hardening indien bekend).

Deze aanpassingen zijn bedoeld om een robuustere, snellere en veiligere Tor-variant te bieden, specifiek afgestemd op hosting-scenario's en `.sn` netwerkomgevingen.

## Functionele Verbeteringen in Deze Versie

Naast de setup voor de ontwikkelomgeving, bevat deze versie van de Tor-code ook specifieke aanpassingen gericht op het verbeteren van de kernfunctionaliteit:

*   **Slimmere Hidden Service Stabiliteit en Keuzes:**
    *   Er zijn wijzigingen doorgevoerd om de algehele stabiliteit van Hidden Services (HS) te verhogen.
    *   Het selectieproces voor bijvoorbeeld introductie- en rendezvouspunten is mogelijk aangepast voor betere prestaties en betrouwbaarheid.

*   **Snellere Reactiesnelheid Tussen Tor Instances:**
    *   Optimalisaties zijn geïmplementeerd om de communicatie en interactie tussen verschillende Tor-nodes (clients, relays, etc.) te versnellen.
    *   Dit kan leiden tot snellere circuitopbouw en een meer responsief netwerk.

*   **Integratie van Besproken Functionaliteiten:**
    *   Diverse andere features en verbeteringen die eerder zijn besproken, zijn geïntegreerd in deze codebase.

Deze aanpassingen zijn bedoeld om deze specifieke Tor-implementatie robuuster en efficiënter te maken dan de standaardversie.

# SN Network Website Manager

Een geavanceerde website manager voor het .sn netwerk, geïntegreerd met Tor voor maximale privacy en veiligheid.

## Functies

- **Virtual Hosting**: Host meerdere websites op het .sn netwerk
- **Automatische Configuratie**: 
  - Willekeurige poorten voor elke website
  - Automatische Nginx configuratie
  - Tor hidden service integratie
- **DDoS Bescherming**:
  - Rate limiting
  - Verbindingslimieten
  - Firewall regels
  - Caching van statische bestanden
- **Privacy & Veiligheid**:
  - Tor hidden services
  - Versleutelde communicatie
  - Geïsoleerde omgevingen
  - Uitgebreide logging

## Vereisten

- Linux systeem (Ubuntu/Debian aanbevolen)
- Nginx
- Tor
- iptables
- Root toegang

## Installatie

1. Compileer de website manager:
```bash
cd src/tools
make clean
make
sudo cp sn_website_manager /usr/local/bin/
```

2. Installeer benodigde dependencies:
```bash
sudo apt-get update
sudo apt-get install nginx tor iptables
```

3. Configureer Tor voor hidden services:
```bash
sudo mkdir -p /var/lib/tor/hidden_service
sudo chown debian-tor:debian-tor /var/lib/tor/hidden_service
```

## Gebruik

### Een nieuwe website maken

```bash
sudo sn_website_manager create mijnsite
```

Dit zal:
1. Een beschikbare poort vinden
2. Een Tor hidden service configureren
3. Nginx configuratie aanmaken
4. DDoS bescherming instellen
5. De website starten

### Website beheren

```bash
# Website starten
sudo sn_website_manager start mijnsite

# Website stoppen
sudo sn_website_manager stop mijnsite

# Website automatisch starten bij boot
sudo sn_website_manager enable mijnsite

# Website uitschakelen bij boot
sudo sn_website_manager disable mijnsite

# Website status bekijken
sudo sn_website_manager status mijnsite

# Alle websites bekijken
sudo sn_website_manager list
```

## Architectuur

### Tor Integratie

Elke website wordt gehost als een Tor hidden service:
- Automatische .onion adres generatie
- Versleutelde communicatie
- Anonieme toegang
- Geïsoleerde netwerk stack

### Netwerk Structuur

```
[Tor Network]
    |
    v
[Tor Hidden Service]
    |
    v
[Nginx Reverse Proxy]
    |
    v
[Website Manager]
    |
    v
[Individuele Websites]
```

### Beveiliging

1. **Netwerk Laag**:
   - Tor hidden services
   - Firewall regels
   - Rate limiting
   - Verbindingslimieten

2. **Applicatie Laag**:
   - Geïsoleerde processen
   - Beperkte gebruikersrechten
   - Uitgebreide logging
   - Automatische updates

3. **Data Laag**:
   - Versleutelde opslag
   - Backups
   - Geïsoleerde bestandssystemen

## Bestanden & Directories

- `/var/www/sn/` - Website bestanden
- `/etc/sn/websites/` - Website configuraties
- `/etc/nginx/conf.d/` - Nginx configuraties
- `/var/log/sn/websites/` - Log bestanden
- `/etc/sn/firewall.rules` - Firewall regels
- `/var/lib/tor/hidden_service/` - Tor hidden services

## Troubleshooting

### Veelvoorkomende problemen

1. **Poort conflicten**:
   ```bash
   sudo netstat -tulpn | grep LISTEN
   ```

2. **Nginx fouten**:
   ```bash
   sudo nginx -t
   sudo tail -f /var/log/nginx/error.log
   ```

3. **Tor problemen**:
   ```bash
   sudo systemctl status tor
   sudo tail -f /var/log/tor/log
   ```

### Logging

- Website logs: `/var/log/sn/websites/[website].log`
- Nginx logs: `/var/log/nginx/`
- Tor logs: `/var/log/tor/log`
- Systeem logs: `/var/log/syslog`

## Ontwikkeling

### Code Structuur

```
src/
  tools/
    sn_website_manager.c  # Hoofd programma
    Makefile             # Compilatie configuratie
```

### Bijdragen

1. Fork de repository
2. Maak een feature branch
3. Commit je wijzigingen
4. Push naar de branch
5. Maak een pull request

## Licentie

GNU General Public License v3.0

## Contact

Voor vragen of problemen:
- Issue tracker: [GitHub Issues](https://github.com/your-repo/issues)
- Email: support@sn.network

## Website Manager <a name="website-manager"></a>

### Overview

The SN Network Website Manager is a powerful tool for hosting websites on the .sn network with full Tor integration. It provides:

- Virtual hosting with .sn domains
- Automatic Tor hidden service setup
- DDoS protection
- Automatic port configuration
- Nginx reverse proxy
- Comprehensive logging

### Installation

1. Compile the website manager:
```bash
cd src/tools
make clean
make
sudo cp sn_website_manager /usr/local/bin/
```

2. Install dependencies:
```bash
sudo apt-get update
sudo apt-get install nginx tor iptables
```

3. Configure Tor for hidden services:
```bash
sudo mkdir -p /var/lib/tor/hidden_service
sudo chown debian-tor:debian-tor /var/lib/tor/hidden_service
```

### Usage

#### Creating a Website

```bash
sudo sn_website_manager create mijnsite
```

This will:
1. Find an available port
2. Create a Tor hidden service
3. Generate a .sn domain (mijnsite.<onion-address>.sn)
4. Configure Nginx
5. Set up DDoS protection
6. Start the website

The website will be available at:
- `http://mijnsite.<onion-address>.sn` (via .sn network)
- `http://<onion-address>.onion` (via Tor)

#### Managing Websites

```bash
# Start a website
sudo sn_website_manager start mijnsite

# Stop a website
sudo sn_website_manager stop mijnsite

# Enable on boot
sudo sn_website_manager enable mijnsite

# Disable on boot
sudo sn_website_manager disable mijnsite

# Check status
sudo sn_website_manager status mijnsite

# List all websites
sudo sn_website_manager list
```

### Architecture

```
[Tor Network]
    |
    v
[Tor Hidden Service]
    |
    v
[Nginx Reverse Proxy]
    |
    v
[Website Manager]
    |
    v
[Individuele Websites]
```

### Security Features

1. **Network Layer**:
   - Tor hidden services
   - Firewall rules
   - Rate limiting
   - Connection limits

2. **Application Layer**:
   - Isolated processes
   - Limited user permissions
   - Comprehensive logging
   - Automatic updates

3. **Data Layer**:
   - Encrypted storage
   - Backups
   - Isolated filesystems

### Files & Directories

- `/var/www/sn/` - Website files
- `/etc/sn/websites/` - Website configurations
- `/etc/nginx/conf.d/` - Nginx configurations
- `/var/log/sn/websites/` - Log files
- `/etc/sn/firewall.rules` - Firewall rules
- `/var/lib/tor/hidden_service/` - Tor hidden services

### Troubleshooting

#### Common Issues

1. **Port Conflicts**:
   ```bash
   sudo netstat -tulpn | grep LISTEN
   ```

2. **Nginx Errors**:
   ```bash
   sudo nginx -t
   sudo tail -f /var/log/nginx/error.log
   ```

3. **Tor Issues**:
   ```bash
   sudo systemctl status tor
   sudo tail -f /var/log/tor/log
   ```

#### Logging

- Website logs: `/var/log/sn/websites/[website].log`
- Nginx logs: `/var/log/nginx/`
- Tor logs: `/var/log/tor/log`
- System logs: `/var/log/syslog`

### Development

#### Code Structure

```
src/
  tools/
    sn_website_manager.c  # Main program
    Makefile             # Compilation configuration
```

#### Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a pull request

### License

GNU General Public License v3.0

### Contact

For questions or issues:
- Issue tracker: [GitHub Issues](https://github.com/your-repo/issues)
- Email: support@sn.network 