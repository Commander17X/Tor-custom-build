#!/bin/bash

# Stop any running Tor instances
sudo systemctl stop tor
sudo pkill -f tor

# Create necessary directories
sudo mkdir -p /etc/tor
sudo mkdir -p /var/lib/tor
sudo mkdir -p /var/log/tor

# Install the compiled Tor
sudo make install

# Set up configuration
sudo cp src/config/torrc.sample /etc/tor/torrc

# Set proper permissions
sudo chown -R debian-tor:debian-tor /var/lib/tor
sudo chown -R debian-tor:debian-tor /var/log/tor
sudo chmod 700 /var/lib/tor
sudo chmod 700 /var/log/tor

# Start Tor service
sudo systemctl daemon-reload
sudo systemctl start tor

# Verify installation
echo "Checking Tor version:"
tor --version

echo "Checking Tor status:"
sudo systemctl status tor

echo "Checking if Tor is running:"
curl --socks5 localhost:9050 --socks5-hostname localhost:9050 -s https://check.torproject.org/ | cat | grep -m 1 Congratulations | xargs 