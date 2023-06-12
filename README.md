
# This repository has been deprecated. Use instead: https://github.com/martukas/nuclei/

Note: this repository has been imported in Apr 2021 from https://sourceforge.net/projects/nuclei/ in order to adapt it to Qt5.

Nuclei is a software tool for the displaying of nuclear decay schemes and estimated energy spectra, the calculation of angular γ emission anisotropies, and the automated search for appropriate decay cascade properties. It uses the Evaluated Nuclear Structure Data Files (ENSDF).

A description of Nuclei's functionality and results obtained using its search method was published in Nuclear Instruments and Methods in Physics Research, Section A:

M. Nagl, et al., NIM A 726 (2013), 17-30
[doi:10.1016/j.nima.2013.05.045]

-------------------------
© 2012-2013, Matthias Nagl, II. Physikalisches Institut, Georg-August-Universität Göttingen
http://physik2.uni-goettingen.de/research/2_hofs
-------------------------

# Features

- Exports decay schemes as pdf or svg file
- Allows to select decay cascades for TDPAC spectroscopy
- Computes angular gamma emission anisotropy values (e.g. for TDPAC)
- Shows estimated energy spectra for easier spectrometer calibration
- Powerful decay cascade search functionality

# Prerequisites (Ubuntu 19 - Qt5)

Quazip:
sudo apt install libquazip-dev libgl1-mesa-dev

Libakk:

- cd /opt
- sudo git clone https://github.com/ferdymercury/libakk
- sudo chown $USER -R libakk
- cd libakk
- git checkout dev
- Open with QtCreator and compile
- On QtCreator build directory path, create symlink called libakk pointing to build-...

QtFtp:

- cd /opt
- sudo git clone https://github.com/qt/qtftp
- sudo chown $USER -R qtftp
- cd qtftp
- <QTDIR>/bin/syncqt.pl -version X.Y.Z
- Open with QtCreator and compile
- On QtCreator build directory path, create symlink called qtftp pointing to build-...

Qwt:
- cd /opt
- sudo git clone https://github.com/opencor/qwt
- sudo chown $USER -R qwt
- Open with QtCreator and compile
- cd to build directory generated by QtCreator
- make
- sudo make install

Qxt:
- cd /opt
- sudo git clone https://github.com/ferdymercury/libqxt
- sudo chown $USER -R libqxt
- cd libqxt
- git checkout dev
- ./configure -qmake-bin <QT_DIR>/bin/qmake
- make
- sudo make install
- If some error is found, you might need for nuclei to compile to do this:
- cd /usr/local/Qxt/include
- sudo ln -s QxtCore/qxtglobal.h qxtglobal.h
- sudo ln -s QxtCore/qxtnamespace.h qxtnamespace.h

# Compilation (Ubuntu 19 - Qt5)

- cd /opt
- git clone https://github.com/ferdymercury/nuclei
- sudo chown $USER -R nuclei
- cd nuclei
- git checkout dev
- Open with QtCreator and compile
- Click on Run

# ENSDF database
- For the moment the automatic download of the ENSDF database via FTP is broken
- Instead, download it from https://www.nndc.bnl.gov/ensarchivals/
- Extract it into the same folder e.g. on /opt/
- When running, select load from local copy
