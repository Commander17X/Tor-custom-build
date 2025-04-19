# Tor Custom Build: Geoptimaliseerd voor Hosting en .SN Netwerken

Hoi ontwikkelaar! Deze README legt een beetje uit hoe dit Tor project in elkaar zit en hoe je ermee aan de slag kunt op je Windows machine.

## Wat is dit?

Dit is een **aangepaste versie (fork)** van de Tor broncode. Terwijl de originele Tor focust op algemene anonimiteit, is deze fork specifiek aangepast en geoptimaliseerd met de volgende doelen:

*   **Verbeterde Hostability:** Stabieler en efficiënter maken van het hosten van services (met name Hidden Services) op het netwerk.
*   **Hogere Snelheid:** Optimalisaties voor snellere verbindingen en reactietijden binnen het netwerk.
*   **Integratie met .SN Netwerken:** Specifieke aanpassingen voor betere werking en interactie binnen aangepaste `.sn` netwerkomgevingen.
*   **Verhoogde Veiligheid:** Extra maatregelen en aangescherpte logica om de veiligheid te verbeteren.

De basisfunctionaliteit van Tor (anonimiteit via gelaagde encryptie en relays) blijft behouden, maar met deze specifieke verbeteringen.

## Mappenstructuur (Waar vind ik wat?)

De code is opgedeeld in verschillende mappen in de `src` map:

*   `src/core`: De echte kern van Tor, zoals netwerkverbindingen en circuits bouwen.
*   `src/feature`: Specifieke features zoals hidden services (`hs`), directory authorities, etc.
*   `src/lib`: Algemene hulpfuncties en bibliotheken die overal gebruikt worden.
*   `src/app`: Code specifiek voor de Tor applicatie zelf.
*   `src/ext`: Externe libraries waar Tor van afhankelijk is.
*   `src/trunnel`: Code voor het parsen van Tor's cellen (de data pakketjes).
*   `src/tools`: Handige tools voor ontwikkeling of beheer.
*   `src/test`: Unit tests en integratie tests.
*   `src/win32`: Specifieke code voor Windows.

## Code Bouwen (Compileren)

Om de code te kunnen draaien of testen, moet je het eerst compileren. Op Windows gebruiken we hiervoor meestal:

1.  **MSYS2 met MinGW-w64**: Dit geeft je een Linux-achtige omgeving en de `gcc` compiler. Zorg dat je dit geïnstalleerd hebt.
2.  **Build Tools**: Je hebt ook dingen als `make`, `automake`, `autoconf` nodig. Installeer die via de MSYS2 package manager (`pacman`).
3.  **Afhankelijkheden**: Tor heeft externe libraries nodig (zoals OpenSSL, Libevent, zlib). Installeer deze ook via `pacman` (de `mingw-w64-x86_64-...` versies).

**Belangrijk:** Tijdens het bouwproces wordt een bestand `orconfig.h` gemaakt. Dit bestand bevat configuratie-instellingen die specifiek zijn voor jouw systeem en build. Veel andere bestanden hebben dit nodig (`#include "orconfig.h"`). **Zonder een succesvolle build zal je linter (en de compiler) veel fouten geven omdat dit bestand mist!**

De typische stappen om te builden (uitgevoerd in de MSYS2 MinGW 64-bit shell) zijn:

```bash
./autogen.sh  # Maakt de configure script aan (alleen de eerste keer nodig)
./configure   # Checkt je systeem en stelt de build in
make          # Compileert de code
```

Kijk in de officiële Tor documentatie voor de exacte commando's en benodigde packages.

## Ontwikkelen met VS Code (Linting & IntelliSense)

Om het makkelijker te maken om de code te lezen en te schrijven in VS Code, hebben we een configuratiebestand: `.vscode/c_cpp_properties.json`.

*   **Wat doet het?** Dit bestand vertelt de C/C++ extensie van VS Code waar het de `#include` bestanden (zoals `orconfig.h` en andere Tor headers) kan vinden.
*   **Waarom?** Zonder dit weet VS Code niet waar alle functies en types gedefinieerd zijn, en krijg je rode golfjes (linting errors) en werkt auto-aanvullen (IntelliSense) niet goed.
*   **Instellingen:**
    *   `includePath`: Hier staan alle mappen waar VS Code moet zoeken naar header files.
    *   `compilerPath`: Verwijst naar de `gcc.exe` compiler in je MSYS2 installatie. Controleer of dit pad klopt voor jouw systeem.
    *   `intelliSenseMode`: Staat ingesteld op `windows-gcc-x64` omdat we met GCC via MSYS2 werken.

**Onthoud:** Zelfs met de juiste VS Code configuratie, moet je nog steeds de code **eerst succesvol builden** zodat `orconfig.h` bestaat, anders blijven de include-fouten bestaan. Herstart VS Code soms na het builden of na het aanpassen van `c_cpp_properties.json`.

## Meer Info

*   Check de officiële Tor documentatie voor de meest up-to-date build instructies en diepere uitleg.
*   Verken de `src` map om te zien hoe de verschillende onderdelen samenwerken.

Veel succes met ontwikkelen!

Tor protects your privacy on the internet by hiding the connection between
your Internet address and the services you use. We believe Tor is reasonably
secure, but please ensure you read the instructions and configure it properly.

## Build

To build Tor from source:

```
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

 