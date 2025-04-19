#!/bin/bash

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo "Please run as root"
    exit 1
fi

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install required packages
echo "Installing required packages..."
if command_exists apt-get; then
    apt-get update
    apt-get install -y iptables-persistent fail2ban ufw
elif command_exists yum; then
    yum install -y iptables-services fail2ban
else
    echo "Unsupported package manager"
    exit 1
fi

# Configure iptables
echo "Configuring iptables..."
cat > /etc/iptables/rules.v4 << 'EOF'
*filter
:INPUT DROP [0:0]
:FORWARD DROP [0:0]
:OUTPUT ACCEPT [0:0]

# Allow localhost
-A INPUT -i lo -j ACCEPT
-A OUTPUT -o lo -j ACCEPT

# Allow established connections
-A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# Allow SSH
-A INPUT -p tcp --dport 22 -j ACCEPT

# Allow Tor ports
-A INPUT -p tcp --dport 9001 -j ACCEPT  # ORPort
-A INPUT -p tcp --dport 9030 -j ACCEPT  # DirPort
-A INPUT -p tcp --dport 80 -j ACCEPT    # HTTP
-A INPUT -p tcp --dport 443 -j ACCEPT   # HTTPS

# Allow ICMP (ping)
-A INPUT -p icmp -j ACCEPT

# Log dropped packets
-A INPUT -j LOG --log-prefix "IPTABLES-DROP: " --log-level 4

COMMIT
EOF

# Configure fail2ban
echo "Configuring fail2ban..."
cat > /etc/fail2ban/jail.local << 'EOF'
[sshd]
enabled = true
port = ssh
filter = sshd
logpath = /var/log/auth.log
maxretry = 3
bantime = 3600

[tor-ssh]
enabled = true
port = ssh
filter = sshd
logpath = /var/log/auth.log
maxretry = 3
bantime = 3600
EOF

# Configure UFW if available
if command_exists ufw; then
    echo "Configuring UFW..."
    ufw default deny incoming
    ufw default allow outgoing
    ufw allow ssh
    ufw allow 9001/tcp  # ORPort
    ufw allow 9030/tcp  # DirPort
    ufw allow 80/tcp    # HTTP
    ufw allow 443/tcp   # HTTPS
    ufw --force enable
fi

# Enable and start services
echo "Starting services..."
if command_exists systemctl; then
    systemctl enable iptables
    systemctl start iptables
    systemctl enable fail2ban
    systemctl start fail2ban
    if command_exists ufw; then
        systemctl enable ufw
        systemctl start ufw
    fi
else
    service iptables restart
    service fail2ban restart
    if command_exists ufw; then
        service ufw restart
    fi
fi

# Set up automatic updates
echo "Setting up automatic updates..."
if command_exists apt-get; then
    apt-get install -y unattended-upgrades
    dpkg-reconfigure -plow unattended-upgrades
elif command_exists yum; then
    yum install -y yum-cron
    systemctl enable yum-cron
    systemctl start yum-cron
fi

# Configure sysctl for security
echo "Configuring sysctl..."
cat > /etc/sysctl.d/99-security.conf << 'EOF'
# Disable IP forwarding
net.ipv4.ip_forward = 0

# Enable SYN flood protection
net.ipv4.tcp_syncookies = 1

# Disable ICMP redirects
net.ipv4.conf.all.accept_redirects = 0
net.ipv4.conf.default.accept_redirects = 0

# Enable IP spoofing protection
net.ipv4.conf.all.rp_filter = 1
net.ipv4.conf.default.rp_filter = 1

# Disable source routing
net.ipv4.conf.all.accept_source_route = 0
net.ipv4.conf.default.accept_source_route = 0

# Enable TCP SYN cookies
net.ipv4.tcp_syncookies = 1

# Enable bad error message protection
net.ipv4.icmp_ignore_bogus_error_responses = 1
EOF

# Apply sysctl settings
sysctl -p /etc/sysctl.d/99-security.conf

echo "Security setup complete!"
echo "Please review the configuration and adjust as needed for your specific requirements." 