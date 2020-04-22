## Problem
Since all Masternode IP address is visible in all wallets, it's possible that someone try to attack one or many. 

Depending on your VPS specifications, it could restart and you'd lose some coins, till it turn on again.
***

## Solution
Using few softwares, it's possible to mitigate some of this attacks, but it's always recommended to rent a CloudFlare or purchase your hosting DDoS protection (Around $10).
***

## Required
FirewallD 

Fail2Ban

***
## Installing FirewallD

### Fedora

`sudo dnf install -y firewalld # Probably already installed`

### CentOS or RHEL...

`sudo yum install -y firewalld # Probably already installed`

### Debian or Ubuntu

`sudo apt install -y firewalld`


## Setting IPTABLES

Mask iptables

```
sudo systemctl disable iptables.service
sudo systemctl mask iptables.service
```

Setting firewall
Is firewalld running? If not, turn it on.
```
sudo firewall-cmd --state
sudo systemctl start firewalld.service
```

Enable firewalld to start upon boot.
```
sudo systemctl enable firewalld.service
```

Determine what the default zone is.
```
sudo firewall-cmd --get-default-zone
sudo firewall-cmd --get-active-zone
```

Take a look at the configuration as it stands now.
```
sudo firewall-cmd --list-all
```

Add and remove base services.
```
sudo firewall-cmd --permanent --add-service ssh
sudo firewall-cmd --permanent --remove-service dhcpv6-client
// sudo firewall-cmd --permanent --add-service cockpit - not used in fedora
sudo firewall-cmd --permanent --remove-service cockpit
```

Open Masternode port
```
sudo firewall-cmd --permanent --add-port=51315/tcp
```
Limit traffic to 10 request per minute
```
sudo firewall-cmd --permanent --add-rich-rule='rule service name=ssh limit value=10/m accept'
```
Final configuration
```
sudo firewall-cmd --reload
sudo firewall-cmd --state
sudo firewall-cmd --list-all
```

We are done here.

***
## Installing Fail2Ban

Utility:
Fail2Ban verify for bad requests, or hackers trying to attack your vps.

### Fedora

`
sudo dnf install -y fail2ban fail2ban-systemd
`

### CentOS or RHEL
```
sudo yum install epel-release # if not already installed
sudo yum install -y fail2ban fail2ban-systemd
```

### Debian or Ubuntu

`sudo apt install -y fail2ban`

If you are not using FirewallD, and instead are using IPTables for your firewall, uninstall fail2ban-firewalld (for the Red Hat-based systems only).

`sudo dnf remove -y fail2ban-firewalld # Fedora`

`sudo yum remove -y fail2ban-firewalld # CentOS or RHEL`

## Configuring Fail2Ban

Edit `/etc/fail2ban/jail.d/local.conf` or `/etc/fail2ban/jail.local`
Copy, Paste and save:
```
[DEFAULT]
# Ban hosts for one hour:
bantime = 3600

# Flip the comments here if you use iptables instead of firewalld
#banaction = iptables-multiport
port = ssh
banaction = iptables-multiport

# Enable logging to the systemd journal
backend = systemd

[sshd]
enabled = true
```

### Enable Fail2Ban and reboot
```
sudo systemctl enable fail2ban
sudo systemctl restart fail2ban
```

### Analyzing
```
# Watch the IP addresses slowly pile up by occassionally looking in the SSH jail...
sudo fail2ban-client status sshd
```
```
# Also watch...
sudo journalctl -u fail2ban.service -f
```
```
# ...and...
sudo tail -F /var/log/fail2ban.log 
```
