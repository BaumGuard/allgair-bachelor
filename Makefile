CC = g++
CFLAGS = -c -Wall -g --std=c++23
LDFLAGS = -lcurl -ltiff

TARGET = main

SRCDIR = src
BUILDDIR = build

all: $(TARGET)

$(TARGET): $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o $(BUILDDIR)/gmlfile.o $(BUILDDIR)/vector_tile.o $(BUILDDIR)/grid_tile.o $(BUILDDIR)/surface.o $(BUILDDIR)/download.o $(BUILDDIR)/exception.o $(BUILDDIR)/main.o $(SRCDIR)/status_codes.h
	$(CC) $(LDFLAGS) $(BUILDDIR)/main.o $(SRCDIR)/status_codes.h $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o $(BUILDDIR)/gmlfile.o $(BUILDDIR)/vector_tile.o $(BUILDDIR)/grid_tile.o $(BUILDDIR)/surface.o $(BUILDDIR)/download.o $(BUILDDIR)/exception.o -o $(TARGET)

$(BUILDDIR)/vector.o: $(SRCDIR)/geometry/vector.h $(SRCDIR)/geometry/vector.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/geometry/vector.cpp -o $(BUILDDIR)/vector.o

$(BUILDDIR)/line.o: $(SRCDIR)/geometry/line.h $(SRCDIR)/geometry/line.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/geometry/line.cpp -o $(BUILDDIR)/line.o

$(BUILDDIR)/plane.o: $(SRCDIR)/geometry/plane.h $(SRCDIR)/geometry/plane.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/geometry/plane.cpp -o $(BUILDDIR)/plane.o

$(BUILDDIR)/polygon.o: $(SRCDIR)/geometry/polygon.h $(SRCDIR)/geometry/polygon.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/geometry/polygon.cpp -o $(BUILDDIR)/polygon.o

$(BUILDDIR)/utils.o: $(SRCDIR)/utils.h $(SRCDIR)/utils.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/utils.cpp -o $(BUILDDIR)/utils.o

$(BUILDDIR)/gmlfile.o: $(SRCDIR)/raw_data/gmlfile.h $(SRCDIR)/raw_data/gmlfile.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/raw_data/gmlfile.cpp -o $(BUILDDIR)/gmlfile.o

$(BUILDDIR)/vector_tile.o: $(SRCDIR)/tile/vector_tile.h $(SRCDIR)/tile/vector_tile.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/tile/vector_tile.cpp -o $(BUILDDIR)/vector_tile.o

$(BUILDDIR)/grid_tile.o: $(SRCDIR)/tile/grid_tile.h $(SRCDIR)/tile/grid_tile.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/tile/grid_tile.cpp -o $(BUILDDIR)/grid_tile.o

$(BUILDDIR)/surface.o: $(SRCDIR)/raw_data/surface.h $(SRCDIR)/raw_data/surface.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/raw_data/surface.cpp -o $(BUILDDIR)/surface.o

$(BUILDDIR)/download.o: $(SRCDIR)/web/download.h $(SRCDIR)/web/download.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/web/download.cpp -o $(BUILDDIR)/download.o

$(BUILDDIR)/exception.o: $(SRCDIR)/exceptions/exception.h $(SRCDIR)/exceptions/exception.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/exceptions/exception.cpp -o $(BUILDDIR)/exception.o

$(BUILDDIR)/main.o: $(SRCDIR)/main.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/main.cpp -o $(BUILDDIR)/main.o

test: $(BUILDDIR)/*.o tests/*.cpp tests/*.h
	$(CC) -c -g tests/test.cpp -o $(BUILDDIR)/test.o
	$(CC) -std=c++17 -g -lgtest $(BUILDDIR)/test.o $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o -o test

clean:
	rm -rf $(BUILDDIR)/*.o
	rm -rf *.o
	rm -f test main
