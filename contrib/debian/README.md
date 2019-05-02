
Debian
====================
This directory contains files used to package martexd/martex-qt
for Debian-based Linux systems. If you compile martexd/martex-qt yourself, there are some useful files here.

## martex: URI support ##


martex-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install martex-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your martex-qt binary to `/usr/bin`
and the `../../share/pixmaps/martex128.png` to `/usr/share/pixmaps`

martex-qt.protocol (KDE)

