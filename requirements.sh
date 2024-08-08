#!/bin/bash

# packages
sudo apt-get install make
sudo apt-get install g++
sudo apt-get install redis
sudo apt-get install libhiredis-dev

# commands
sudo usermod -aG $USER postgres