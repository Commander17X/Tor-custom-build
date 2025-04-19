#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define WEBSITE_DIR "/var/www/sn"
#define CONFIG_DIR "/etc/sn/websites"
#define SERVICE_DIR "/etc/systemd/system"
#define LOG_DIR "/var/log/sn/websites"
#define NGINX_CONF_DIR "/etc/nginx/conf.d"
#define FIREWALL_RULES "/etc/sn/firewall.rules"
#define TOR_HIDDEN_SERVICE_DIR "/var/lib/tor/hidden_service"
#define TORRC_PATH "/etc/tor/torrc"

// Function to check if port is available
int is_port_available(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return 0;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    int result = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    close(sock);
    return result == 0;
}

// Function to find random available port
int find_available_port() {
    srand(time(NULL));
    int port;
    do {
        port = 1024 + (rand() % (65535 - 1024));
    } while (!is_port_available(port));
    return port;
}

// Function to create necessary directories
void create_directories() {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), 
             "mkdir -p %s %s %s %s %s && chmod 755 %s %s %s %s %s",
             WEBSITE_DIR, CONFIG_DIR, SERVICE_DIR, LOG_DIR, NGINX_CONF_DIR,
             WEBSITE_DIR, CONFIG_DIR, SERVICE_DIR, LOG_DIR, NGINX_CONF_DIR);
    system(cmd);
}

// Function to create Tor hidden service configuration
int create_tor_hidden_service(const char *name, int port) {
    char tor_service_dir[256];
    snprintf(tor_service_dir, sizeof(tor_service_dir), "%s/%s", TOR_HIDDEN_SERVICE_DIR, name);
    
    // Create hidden service directory
    if (mkdir(tor_service_dir, 0700) == -1) return -1;
    
    // Set correct permissions
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "chown -R debian-tor:debian-tor %s", tor_service_dir);
    if (system(cmd) != 0) return -1;
    
    // Add hidden service configuration to torrc
    FILE *torrc = fopen(TORRC_PATH, "a");
    if (!torrc) return -1;
    
    fprintf(torrc, "\n# Hidden service for %s\n", name);
    fprintf(torrc, "HiddenServiceDir %s\n", tor_service_dir);
    fprintf(torrc, "HiddenServicePort 80 127.0.0.1:%d\n", port);
    
    fclose(torrc);
    
    // Reload Tor configuration
    if (system("systemctl reload tor") != 0) return -1;
    
    // Wait for .onion address to be generated
    sleep(5);
    
    // Read .onion address
    char hostname_path[256];
    snprintf(hostname_path, sizeof(hostname_path), "%s/hostname", tor_service_dir);
    
    FILE *hostname = fopen(hostname_path, "r");
    if (!hostname) return -1;
    
    char onion_address[256];
    if (!fgets(onion_address, sizeof(onion_address), hostname)) {
        fclose(hostname);
        return -1;
    }
    
    fclose(hostname);
    
    // Remove newline and .onion suffix
    onion_address[strcspn(onion_address, "\n")] = 0;
    char *dot_onion = strstr(onion_address, ".onion");
    if (dot_onion) *dot_onion = '\0';
    
    // Update website configuration with .onion address
    char config_path[256];
    snprintf(config_path, sizeof(config_path), "%s/%s.conf", CONFIG_DIR, name);
    
    FILE *config = fopen(config_path, "a");
    if (!config) return -1;
    
    fprintf(config, "ONION_ADDRESS=%s\n", onion_address);
    fclose(config);
    
    return 0;
}

// Function to create website configuration
int create_website_config(const char *name, const char *domain, int port) {
    char config_path[256];
    snprintf(config_path, sizeof(config_path), "%s/%s.conf", CONFIG_DIR, name);
    
    FILE *config = fopen(config_path, "w");
    if (!config) return -1;
    
    fprintf(config, "WEBSITE_NAME=%s\n", name);
    fprintf(config, "DOMAIN=%s\n", domain);
    fprintf(config, "PORT=%d\n", port);
    fprintf(config, "DOCUMENT_ROOT=%s/%s\n", WEBSITE_DIR, name);
    fprintf(config, "LOG_FILE=%s/%s.log\n", LOG_DIR, name);
    fprintf(config, "ACCESS_LOG=%s/%s.access.log\n", LOG_DIR, name);
    fprintf(config, "ERROR_LOG=%s/%s.error.log\n", LOG_DIR, name);
    
    fclose(config);
    return 0;
}

// Function to create nginx virtual host configuration
int create_nginx_config(const char *name, const char *onion_address, int port) {
    char nginx_path[256];
    snprintf(nginx_path, sizeof(nginx_path), "%s/%s.conf", NGINX_CONF_DIR, name);
    
    FILE *nginx = fopen(nginx_path, "w");
    if (!nginx) return -1;
    
    // Create .sn domain that includes the onion address
    char sn_domain[256];
    snprintf(sn_domain, sizeof(sn_domain), "%s.%s.sn", name, onion_address);
    
    fprintf(nginx, "server {\n");
    fprintf(nginx, "    listen 80;\n");
    fprintf(nginx, "    server_name %s;\n\n", sn_domain);
    fprintf(nginx, "    # DDoS Protection\n");
    fprintf(nginx, "    limit_req_zone $binary_remote_addr zone=one:10m rate=1r/s;\n");
    fprintf(nginx, "    limit_conn_zone $binary_remote_addr zone=addr:10m;\n\n");
    fprintf(nginx, "    location / {\n");
    fprintf(nginx, "        limit_req zone=one burst=5;\n");
    fprintf(nginx, "        limit_conn addr 10;\n");
    fprintf(nginx, "        proxy_pass http://127.0.0.1:%d;\n", port);
    fprintf(nginx, "        proxy_set_header Host $host;\n");
    fprintf(nginx, "        proxy_set_header X-Real-IP $remote_addr;\n");
    fprintf(nginx, "        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;\n");
    fprintf(nginx, "        proxy_set_header X-Forwarded-Proto $scheme;\n");
    fprintf(nginx, "    }\n\n");
    fprintf(nginx, "    # Static file caching\n");
    fprintf(nginx, "    location ~* \\.(jpg|jpeg|png|gif|ico|css|js)$ {\n");
    fprintf(nginx, "        expires 30d;\n");
    fprintf(nginx, "        add_header Cache-Control \"public, no-transform\";\n");
    fprintf(nginx, "    }\n\n");
    fprintf(nginx, "    # Logging\n");
    fprintf(nginx, "    access_log %s/%s.access.log;\n", LOG_DIR, name);
    fprintf(nginx, "    error_log %s/%s.error.log;\n", LOG_DIR, name);
    fprintf(nginx, "}\n");
    
    fclose(nginx);
    return 0;
}

// Function to update firewall rules
int update_firewall_rules(const char *name, int port) {
    FILE *firewall = fopen(FIREWALL_RULES, "a");
    if (!firewall) return -1;
    
    fprintf(firewall, "# Rules for website %s\n", name);
    fprintf(firewall, "-A INPUT -p tcp --dport %d -m state --state NEW -m limit --limit 50/second --limit-burst 100 -j ACCEPT\n", port);
    fprintf(firewall, "-A INPUT -p tcp --dport %d -m state --state NEW -j DROP\n", port);
    
    fclose(firewall);
    
    // Apply new rules
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "iptables-restore < %s", FIREWALL_RULES);
    return system(cmd);
}

// Function to create website directory
int create_website_directory(const char *name) {
    char website_path[256];
    snprintf(website_path, sizeof(website_path), "%s/%s", WEBSITE_DIR, name);
    
    if (mkdir(website_path, 0755) == -1) return -1;
    
    // Create basic index.html with DDoS protection notice
    char index_path[256];
    snprintf(index_path, sizeof(index_path), "%s/index.html", website_path);
    
    FILE *index = fopen(index_path, "w");
    if (!index) return -1;
    
    fprintf(index, "<!DOCTYPE html>\n");
    fprintf(index, "<html>\n<head>\n<title>%s - SN Network</title>\n", name);
    fprintf(index, "<style>\n");
    fprintf(index, "body { font-family: Arial, sans-serif; margin: 40px; }\n");
    fprintf(index, ".security-badge { background: #4CAF50; color: white; padding: 5px 10px; border-radius: 3px; }\n");
    fprintf(index, "</style>\n</head>\n");
    fprintf(index, "<body>\n<h1>Welcome to %s</h1>\n", name);
    fprintf(index, "<p>This is your new website on the SN Network.</p>\n");
    fprintf(index, "<p><span class=\"security-badge\">DDoS Protected</span></p>\n");
    fprintf(index, "</body>\n</html>");
    
    fclose(index);
    return 0;
}

// Function to start a website
int start_website(const char *name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "systemctl start sn-website-%s && systemctl reload nginx", name);
    return system(cmd);
}

// Function to stop a website
int stop_website(const char *name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "systemctl stop sn-website-%s && systemctl reload nginx", name);
    return system(cmd);
}

// Function to enable website on boot
int enable_website(const char *name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "systemctl enable sn-website-%s", name);
    return system(cmd);
}

// Function to disable website on boot
int disable_website(const char *name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "systemctl disable sn-website-%s", name);
    return system(cmd);
}

// Function to list all websites
void list_websites() {
    DIR *dir = opendir(CONFIG_DIR);
    if (!dir) {
        printf("No websites configured.\n");
        return;
    }
    
    struct dirent *entry;
    printf("Configured websites:\n");
    printf("-------------------\n");
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".conf")) {
            char *name = strtok(entry->d_name, ".");
            char config_path[256];
            snprintf(config_path, sizeof(config_path), "%s/%s", CONFIG_DIR, entry->d_name);
            
            FILE *config = fopen(config_path, "r");
            if (config) {
                char line[256];
                char domain[256] = "N/A";
                char port[256] = "N/A";
                
                while (fgets(line, sizeof(line), config)) {
                    if (strstr(line, "DOMAIN=")) sscanf(line, "DOMAIN=%s", domain);
                    if (strstr(line, "PORT=")) sscanf(line, "PORT=%s", port);
                }
                
                printf("- %s (Domain: %s, Port: %s)\n", name, domain, port);
                fclose(config);
            }
        }
    }
    
    closedir(dir);
}

// Function to show website status
int show_website_status(const char *name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "systemctl status sn-website-%s && nginx -t", name);
    return system(cmd);
}

// Function to show help
void show_help() {
    printf("SN Network Website Manager\n");
    printf("-------------------------\n");
    printf("Usage: sn_website_manager [command] [options]\n\n");
    printf("Commands:\n");
    printf("  create <name> <domain>    Create a new website (auto port)\n");
    printf("  start <name>              Start a website\n");
    printf("  stop <name>               Stop a website\n");
    printf("  enable <name>             Enable website on boot\n");
    printf("  disable <name>            Disable website on boot\n");
    printf("  list                      List all websites\n");
    printf("  status <name>             Show website status\n");
    printf("  help                      Show this help message\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_help();
        return 1;
    }
    
    // Check if running as root
    if (geteuid() != 0) {
        printf("Error: This program must be run as root\n");
        return 1;
    }
    
    // Create necessary directories
    create_directories();
    
    if (strcmp(argv[1], "create") == 0) {
        if (argc != 3) {
            printf("Error: create command requires website name\n");
            return 1;
        }
        
        int port = find_available_port();
        
        // First create Tor hidden service to get onion address
        if (create_tor_hidden_service(argv[2], port) != 0) {
            printf("Error: Failed to create Tor hidden service\n");
            return 1;
        }
        
        // Read onion address from config
        char config_path[256];
        snprintf(config_path, sizeof(config_path), "%s/%s.conf", CONFIG_DIR, argv[2]);
        
        FILE *config = fopen(config_path, "r");
        if (!config) {
            printf("Error: Failed to read website configuration\n");
            return 1;
        }
        
        char line[256];
        char onion_address[256] = "N/A";
        
        while (fgets(line, sizeof(line), config)) {
            if (strstr(line, "ONION_ADDRESS=")) {
                sscanf(line, "ONION_ADDRESS=%s", onion_address);
                break;
            }
        }
        
        fclose(config);
        
        if (strcmp(onion_address, "N/A") == 0) {
            printf("Error: Failed to get onion address\n");
            return 1;
        }
        
        // Create website with onion address
        if (create_website_config(argv[2], onion_address, port) != 0 ||
            create_nginx_config(argv[2], onion_address, port) != 0 ||
            create_website_directory(argv[2]) != 0 ||
            update_firewall_rules(argv[2], port) != 0) {
            printf("Error: Failed to create website\n");
            return 1;
        }
        
        // Create .sn domain
        char sn_domain[256];
        snprintf(sn_domain, sizeof(sn_domain), "%s.%s.sn", argv[2], onion_address);
        
        printf("Website %s created successfully!\n", argv[2]);
        printf("Domain: %s\n", sn_domain);
        printf("Onion Address: %s.onion\n", onion_address);
        printf("Port: %d\n", port);
        printf("DDoS protection enabled\n");
        
    } else if (strcmp(argv[1], "start") == 0) {
        if (argc != 3) {
            printf("Error: start command requires website name\n");
            return 1;
        }
        
        if (start_website(argv[2]) != 0) {
            printf("Error: Failed to start website\n");
            return 1;
        }
        
        printf("Website %s started successfully!\n", argv[2]);
        
    } else if (strcmp(argv[1], "stop") == 0) {
        if (argc != 3) {
            printf("Error: stop command requires website name\n");
            return 1;
        }
        
        if (stop_website(argv[2]) != 0) {
            printf("Error: Failed to stop website\n");
            return 1;
        }
        
        printf("Website %s stopped successfully!\n", argv[2]);
        
    } else if (strcmp(argv[1], "enable") == 0) {
        if (argc != 3) {
            printf("Error: enable command requires website name\n");
            return 1;
        }
        
        if (enable_website(argv[2]) != 0) {
            printf("Error: Failed to enable website\n");
            return 1;
        }
        
        printf("Website %s enabled for boot successfully!\n", argv[2]);
        
    } else if (strcmp(argv[1], "disable") == 0) {
        if (argc != 3) {
            printf("Error: disable command requires website name\n");
            return 1;
        }
        
        if (disable_website(argv[2]) != 0) {
            printf("Error: Failed to disable website\n");
            return 1;
        }
        
        printf("Website %s disabled from boot successfully!\n", argv[2]);
        
    } else if (strcmp(argv[1], "list") == 0) {
        list_websites();
        
    } else if (strcmp(argv[1], "status") == 0) {
        if (argc != 3) {
            printf("Error: status command requires website name\n");
            return 1;
        }
        
        show_website_status(argv[2]);
        
    } else if (strcmp(argv[1], "help") == 0) {
        show_help();
        
    } else {
        printf("Error: Unknown command\n");
        show_help();
        return 1;
    }
    
    return 0;
} 