VERSION = 1.1-r1
PREFIX ?= /usr
CXXFLAGS = -std=c++17 -Wall -Wextra -Os -DVERSION=\"${VERSION}\"
CXX = g++
