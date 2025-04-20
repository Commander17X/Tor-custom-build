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
    
    fprintf(nginx, "    # Basic Security Headers\n");
    fprintf(nginx, "    add_header X-Frame-Options \"SAMEORIGIN\";\n");
    fprintf(nginx, "    add_header X-Content-Type-Options \"nosniff\";\n");
    fprintf(nginx, "    add_header Referrer-Policy \"strict-origin-when-cross-origin\";\n");
    // Basic CSP - might need adjustment per site
    fprintf(nginx, "    add_header Content-Security-Policy \"default-src 'self'; script-src 'self'; style-src 'self'; img-src 'self' data:; font-src 'self';\";\n\n"); 

    fprintf(nginx, "    # DDoS Protection\n");
    fprintf(nginx, "    limit_req_zone $binary_remote_addr zone=one:10m rate=1r/s;\n");
    fprintf(nginx, "    limit_conn_zone $binary_remote_addr zone=addr:10m;\n\n");

    fprintf(nginx, "    # Gzip Compression\n");
    fprintf(nginx, "    gzip on;\n");
    fprintf(nginx, "    gzip_vary on;\n");
    fprintf(nginx, "    gzip_proxied any;\n");
    fprintf(nginx, "    gzip_comp_level 6;\n");
    fprintf(nginx, "    gzip_types text/plain text/css application/json application/javascript application/x-javascript text/xml application/xml application/xml+rss text/javascript image/svg+xml;\n\n");

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
    fprintf(nginx, "    location ~* \\.(jpg|jpeg|png|gif|ico|css|js|svg|woff|woff2|ttf|eot)$ {\n");
    fprintf(nginx, "        expires 30d;\n");
    fprintf(nginx, "        add_header Cache-Control \"public, immutable\";\n");
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

// Function to apply configurations by reloading services
int apply_configurations(const char *name) {
    // Reload Nginx first to pick up new/removed configs
    printf("Reloading Nginx configuration for %s...\n", name);
    if (system("systemctl reload nginx") != 0) {
        fprintf(stderr, "Error: Failed to reload Nginx.\n");
        // Don't necessarily fail, Tor might still need reload
    }
    // Reload Tor to pick up new/removed hidden services
    printf("Reloading Tor configuration for %s...\n", name);
    if (system("systemctl reload tor") != 0) {
        fprintf(stderr, "Error: Failed to reload Tor.\n");
        return -1; // Failing Tor reload is more critical
    }
    printf("Nginx and Tor configurations reloaded successfully.\n");
    return 0;
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

// Function to show website status (simplified)
int show_website_status(const char *name) {
    printf("Checking status for website %s configuration:\n", name);
    char nginx_check_cmd[256];
    snprintf(nginx_check_cmd, sizeof(nginx_check_cmd), "nginx -t");
    printf("\n--- Nginx Configuration Check ---\n");
    int nginx_status = system(nginx_check_cmd);
    if (nginx_status != 0) {
        fprintf(stderr, "Nginx configuration test failed for website %s!\n", name);
    } else {
         printf("Nginx configuration syntax OK.\n");
    }

    printf("\n--- Nginx Service Status ---\n");
    system("systemctl status nginx | cat"); // Pipe to cat for non-interactive view

    printf("\n--- Tor Service Status ---\n");
    system("systemctl status tor | cat"); // Pipe to cat

    // Check for HS directory existence as a basic check
    char tor_service_dir[256];
    snprintf(tor_service_dir, sizeof(tor_service_dir), "%s/%s", TOR_HIDDEN_SERVICE_DIR, name);
    struct stat st = {0};
    printf("\n--- Hidden Service Directory Check ---\n");
    if (stat(tor_service_dir, &st) == -1) {
        printf("Hidden Service directory %s does NOT exist.\n", tor_service_dir);
    } else {
        printf("Hidden Service directory %s exists.\n", tor_service_dir);
        // Optionally, check for hostname file existence here too
    }

    return nginx_status; // Return 0 if Nginx config is okay
}

// Function to remove website directory recursively
int remove_website_directory(const char *name) {
    char website_path[256];
    snprintf(website_path, sizeof(website_path), "%s/%s", WEBSITE_DIR, name);
    char cmd[512];
    // Use rm -rf carefully!
    snprintf(cmd, sizeof(cmd), "rm -rf %s", website_path);
    printf("Executing: %s\n", cmd); // Log command for safety
    return system(cmd);
}

// Function to remove firewall rules
// Note: This assumes rules were added with a specific comment format
int remove_firewall_rules(const char *name, int port) {
    char sed_cmd[512];
    // Escape potential special characters in name for sed
    // Simple escape for basic cases, might need more robust escaping
    char escaped_name[256];
    // For now, assume name is simple alphanumeric
    strncpy(escaped_name, name, sizeof(escaped_name) - 1);
    escaped_name[sizeof(escaped_name) - 1] = '\0';

    // Remove the comment line and the two rule lines associated with the port
    snprintf(sed_cmd, sizeof(sed_cmd),
             "sed -i '/# Rules for website %s/,+2d' %s",
             escaped_name, FIREWALL_RULES);
    printf("Executing: %s\n", sed_cmd); // Log command
    int result = system(sed_cmd);
    if (result != 0) {
        fprintf(stderr, "Warning: Failed to remove firewall rules using sed.\n");
        // Don't fail the whole delete operation, but warn
    }

    // Reload firewall rules after modification
    char restore_cmd[256];
    snprintf(restore_cmd, sizeof(restore_cmd), "iptables-restore < %s", FIREWALL_RULES);
    printf("Executing: %s\n", restore_cmd);
    result = system(restore_cmd);
    if (result != 0) {
        fprintf(stderr, "Warning: Failed to restore firewall rules.\n");
    }
    return 0; // Return success even if sed/restore had issues, but log warning
}

// Function to remove Tor hidden service config from torrc
// Note: This relies on the specific format added by create_tor_hidden_service
int remove_tor_config(const char *name) {
    char tor_service_dir_pattern[512];
    // Need to escape path separators for sed
    snprintf(tor_service_dir_pattern, sizeof(tor_service_dir_pattern),
             "%s\/%s", TOR_HIDDEN_SERVICE_DIR, name);
    // Escape forward slashes for sed pattern
    for (char *p = tor_service_dir_pattern; *p; ++p) {
        if (*p == '/') {
            memmove(p + 1, p, strlen(p) + 1);
            *p = '\\';
            p++; // Skip the escaped slash
        }
    }

    char sed_cmd[1024];
    // Remove the comment line, HiddenServiceDir, and HiddenServicePort lines
    snprintf(sed_cmd, sizeof(sed_cmd),
             "sed -i '/# Hidden service for %s/,+2d' %s",
             name, TORRC_PATH);

    printf("Executing: %s\n", sed_cmd); // Log command
    int result = system(sed_cmd);
    if (result != 0) {
        fprintf(stderr, "Warning: Failed to remove Tor config using sed.\n");
        return -1; // Fail if Tor config cannot be removed
    }

    // Reload Tor config
    printf("Executing: systemctl reload tor\n");
    result = system("systemctl reload tor");
    if (result != 0) {
        fprintf(stderr, "Warning: Failed to reload Tor configuration.\n");
        // Don't necessarily fail the whole delete, but it's a problem
    }
    return 0;
}

// Function to delete a website entirely
int delete_website(const char *name) {
    printf("Starting deletion process for website: %s\n", name);

    char config_path[256];
    snprintf(config_path, sizeof(config_path), "%s/%s.conf", CONFIG_DIR, name);

    // 1. Read port from config file before deleting it
    int port = -1;
    FILE *config = fopen(config_path, "r");
    if (config) {
        char line[256];
        while (fgets(line, sizeof(line), config)) {
            if (sscanf(line, "PORT=%d", &port) == 1) {
                break;
            }
        }
        fclose(config);
        printf("Found port %d for website %s\n", port, name);
    } else {
        fprintf(stderr, "Warning: Could not open config file %s to read port. Firewall rules might not be removed.\n", config_path);
        // Continue deletion attempt even if port isn't found
    }

    // 2. Remove Tor configuration and HS directory
    if (remove_tor_config(name) != 0) {
        fprintf(stderr, "Error: Failed to remove Tor configuration for %s. Aborting delete.\n", name);
        // Don't proceed if Tor config removal failed, could leave dangling HS
        return -1;
    }
    char tor_service_dir[256];
    snprintf(tor_service_dir, sizeof(tor_service_dir), "%s/%s", TOR_HIDDEN_SERVICE_DIR, name);
    char rm_tor_cmd[512];
    snprintf(rm_tor_cmd, sizeof(rm_tor_cmd), "rm -rf %s", tor_service_dir);
    printf("Executing: %s\n", rm_tor_cmd);
    if (system(rm_tor_cmd) != 0) {
        fprintf(stderr, "Warning: Failed to remove Tor hidden service directory %s\n", tor_service_dir);
    }

    // 3. Remove Nginx configuration
    char nginx_path[256];
    snprintf(nginx_path, sizeof(nginx_path), "%s/%s.conf", NGINX_CONF_DIR, name);
    printf("Removing Nginx config: %s\n", nginx_path);
    if (remove(nginx_path) != 0 && errno != ENOENT) { // Don't warn if already gone
        perror("Warning: Failed to remove Nginx config file");
    }
    printf("Executing: systemctl reload nginx\n");
    if (system("systemctl reload nginx") != 0) {
        fprintf(stderr, "Warning: Failed to reload Nginx configuration.\n");
    }

    // 4. Remove Firewall rules (if port was found)
    if (port != -1) {
        remove_firewall_rules(name, port);
    } else {
         printf("Skipping firewall rule removal as port was not found.\n");
    }

    // 5. Remove website directory
    if (remove_website_directory(name) != 0) {
         fprintf(stderr, "Warning: Failed to remove website directory %s/%s\n", WEBSITE_DIR, name);
    }

    // 6. Remove website config file
    printf("Removing website config: %s\n", config_path);
    if (remove(config_path) != 0 && errno != ENOENT) {
        perror("Warning: Failed to remove website config file");
    }

    printf("Deletion process for %s completed (check warnings above).\n", name);
    return 0;
}

// Function to show help (updated)
void show_help() {
    printf("SN Network Website Manager\n");
    printf("-------------------------\n");
    printf("Usage: sn_website_manager [command] [options]\n\n");
    printf("Manages Nginx/Tor configurations for websites, does NOT manage website processes.\n\n");
    printf("Commands:\n");
    printf("  create <name>    Create Nginx/Tor config for a new website (auto port)\n");
    printf("  delete <name>    Delete a website and its Nginx/Tor configuration\n");
    printf("  apply <name>     Reload Nginx and Tor to apply configurations for <name>\n");
    printf("  list            List all configured websites (based on config files)\n");
    printf("  status <name>   Check Nginx config syntax and service statuses\n");
    printf("  help            Show this help message\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_help();
        return 1;
    }
    
    // Check if running as root
    if (geteuid() != 0) { // Note: geteuid is POSIX/Linux specific
        printf("Error: This program must be run as root\n");
        return 1;
    }
    
    if (strcmp(argv[1], "create") == 0) {
        if (argc != 3) {
            printf("Error: create command requires website name\n");
            return 1;
        }
        // Ensure necessary base directories exist before creating website
        create_directories(); 
        
        int port = find_available_port();
        
        // First create Tor hidden service to get onion address
        // This also adds config to torrc and reloads tor
        if (create_tor_hidden_service(argv[2], port) != 0) {
            printf("Error: Failed to create Tor hidden service for %s\n", argv[2]);
            return 1;
        }
        
        // Read onion address back from config (written by create_tor_hidden_service)
        char config_path[256];
        snprintf(config_path, sizeof(config_path), "%s/%s.conf", CONFIG_DIR, argv[2]);
        char onion_address[256] = "N/A";
        FILE *config_read = fopen(config_path, "r"); // Open for reading first
        if (config_read) {
             char line[256];
             while (fgets(line, sizeof(line), config_read)) {
                 if (strstr(line, "ONION_ADDRESS=")) {
                     sscanf(line, "ONION_ADDRESS=%s", onion_address);
                     break;
                 }
             }
             fclose(config_read);
        }
        // Check if onion address was found AFTER trying to read
        if (strcmp(onion_address, "N/A") == 0) {
            // This case might happen if create_tor_hidden_service failed silently or file IO issue
            // Attempt to read directly from hostname file as fallback
            char hostname_path[256];
            snprintf(hostname_path, sizeof(hostname_path), "%s/%s/hostname", TOR_HIDDEN_SERVICE_DIR, argv[2]);
            FILE *hostname_file = fopen(hostname_path, "r");
            if (hostname_file) {
                if (fgets(onion_address, sizeof(onion_address), hostname_file)) {
                    onion_address[strcspn(onion_address, "\n")] = 0; // Remove newline
                    char *dot_onion = strstr(onion_address, ".onion");
                    if (dot_onion) *dot_onion = '\0'; // Remove .onion suffix
                    printf("Warning: Read onion address directly from hostname file: %s\n", onion_address);
                } else {
                     printf("Error: Failed to read onion address from hostname file %s.\n", hostname_path);
                     fclose(hostname_file);
                     return 1; 
                }
                 fclose(hostname_file);
            } else {
                 printf("Error: Failed to get onion address from both config and hostname file for %s\n", argv[2]);
                 return 1;
            }
        }
        
        // Create main website config (overwrites/creates the file)
        if (create_website_config(argv[2], onion_address, port) != 0) {
             printf("Error: Failed to create website config file for %s\n", argv[2]);
             return 1;
        }
        // Create Nginx config
        if (create_nginx_config(argv[2], onion_address, port) != 0) {
             printf("Error: Failed to create nginx config file for %s\n", argv[2]);
             return 1;
        }
        // Create web directory
        if (create_website_directory(argv[2]) != 0) {
              printf("Error: Failed to create website directory for %s\n", argv[2]);
             return 1;
        }
        // Add firewall rules
        if (update_firewall_rules(argv[2], port) != 0) {
             printf("Error: Failed to update firewall rules for %s\n", argv[2]);
             return 1;
        }
        
        // Reload Nginx after creating config
        printf("Reloading Nginx configuration...\n");
        if (system("systemctl reload nginx") != 0) {
            fprintf(stderr, "Warning: Failed to reload Nginx after creating config for %s.\n", argv[2]);
            // Continue, but website might not be immediately available
        }

        char sn_domain[256];
        snprintf(sn_domain, sizeof(sn_domain), "%s.%s.sn", argv[2], onion_address);
        printf("Website %s created successfully!\n", argv[2]);
        printf("Domain: %s\n", sn_domain);
        printf("Onion Address: %s.onion\n", onion_address);
        printf("Local Port: %d\n", port);
        printf("DDoS protection enabled\n");
        printf("Security headers added\n");
        printf("Remember to manage your website process (e.g., Node server) separately.\n");
        
    } else if (strcmp(argv[1], "delete") == 0) {
        if (argc != 3) {
            printf("Error: delete command requires website name\n");
            return 1;
        }
        if (delete_website(argv[2]) != 0) {
            printf("Error: Failed to complete website deletion for %s. Check logs/warnings.\n", argv[2]);
            return 1;
        }
        printf("Website %s deletion completed successfully.\n", argv[2]);

    } else if (strcmp(argv[1], "apply") == 0) {
        if (argc != 3) {
            printf("Error: apply command requires website name\n");
            return 1;
        }
        if (apply_configurations(argv[2]) != 0) {
            printf("Error: Failed to apply configurations for %s. Check logs/warnings.\n", argv[2]);
            return 1;
        }
        printf("Configurations for %s applied successfully (Nginx/Tor reloaded).\n", argv[2]);

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
        printf("Error: Unknown command '%s'\n", argv[1]);
        show_help();
        return 1;
    }
    
    return 0;
} 