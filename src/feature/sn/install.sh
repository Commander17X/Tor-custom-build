#!/bin/bash

# Exit on error
set -e

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo "Please run as root"
    exit 1
fi

# Create necessary directories for website manager (adjust paths if needed)
mkdir -p /var/www/sn
mkdir -p /etc/sn/websites
mkdir -p /var/log/sn/websites
# Nginx config dir is usually created by nginx package

# Install dependencies
apt-get update
apt-get install -y tor nginx

# Configure Tor for hidden services
# Ensure Tor user can access this directory
mkdir -p /var/lib/tor/hidden_service
chown -R debian-tor:debian-tor /var/lib/tor/hidden_service
chmod 700 /var/lib/tor/hidden_service

# Ensure necessary users/groups exist (debian-tor is standard for tor package)
# Ensure website manager tool is placed in path (e.g., /usr/local/bin/)
# echo "Ensure sn_website_manager is compiled and placed in /usr/local/bin/"

# Enable and start services
systemctl daemon-reload
systemctl enable tor
systemctl enable nginx

# Start services
systemctl start tor
systemctl start nginx

echo "SN Network base setup complete!"
echo "Remember to install the sn_website_manager tool."
echo "Ensure Tor and Nginx are running correctly." 