#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#define SCRIPT_PATH "../scripts/setup_secure_server.sh"
#define LOG_FILE "/var/log/secure_server.log"
#define BACKUP_DIR "/etc/secure_server_backup"

// Function to log messages with timestamp
void log_message(FILE *log_file, const char *message) {
    time_t now;
    time(&now);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline
    fprintf(log_file, "[%s] %s\n", timestamp, message);
    fflush(log_file);
}

// Function to create backup of important files
int create_backup() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s && cp -r /etc/iptables %s/ && cp -r /etc/fail2ban %s/ && cp -r /etc/ufw %s/ 2>/dev/null", 
             BACKUP_DIR, BACKUP_DIR, BACKUP_DIR, BACKUP_DIR);
    return system(cmd);
}

// Function to restore from backup
int restore_backup() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "cp -r %s/iptables/* /etc/iptables/ 2>/dev/null && "
                               "cp -r %s/fail2ban/* /etc/fail2ban/ 2>/dev/null && "
                               "cp -r %s/ufw/* /etc/ufw/ 2>/dev/null", 
             BACKUP_DIR, BACKUP_DIR, BACKUP_DIR);
    return system(cmd);
}

int main(int argc, char *argv[]) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        fprintf(stderr, "Error: Cannot open log file %s\n", LOG_FILE);
        return 1;
    }

    // Check if running as root
    if (geteuid() != 0) {
        log_message(log_file, "Error: This program must be run as root");
        fclose(log_file);
        return 1;
    }

    log_message(log_file, "Starting server security setup...");

    // Create backup
    log_message(log_file, "Creating backup of current configuration...");
    if (create_backup() != 0) {
        log_message(log_file, "Warning: Failed to create complete backup");
    }

    // Check if script exists
    if (access(SCRIPT_PATH, F_OK) == -1) {
        log_message(log_file, "Error: Setup script not found");
        fclose(log_file);
        return 1;
    }

    // Check if script is executable
    if (access(SCRIPT_PATH, X_OK) == -1) {
        log_message(log_file, "Error: Setup script is not executable");
        fclose(log_file);
        return 1;
    }

    // Execute the script and capture output
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "%s 2>&1 | tee -a %s", SCRIPT_PATH, LOG_FILE);
    int result = system(cmd);
    
    if (result != 0) {
        log_message(log_file, "Error: Setup script failed - initiating rollback");
        
        // Perform rollback
        if (restore_backup() != 0) {
            log_message(log_file, "Error: Rollback failed - manual intervention required");
        } else {
            log_message(log_file, "Rollback completed successfully");
        }
        
        fclose(log_file);
        return 1;
    }

    log_message(log_file, "Server security setup completed successfully!");
    fclose(log_file);
    return 0;
} 