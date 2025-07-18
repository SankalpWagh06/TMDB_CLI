# Makefile for TMDB CLI Tool
CXX = g++
CXXFLAGS = -std=c++17 -lcurl

all: tmdb

tmdb: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o tmdb

clean:
	rm -f tmdb
