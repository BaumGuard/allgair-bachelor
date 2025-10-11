CC = g++
CFLAGS = -c -Wall -g --std=c++23

TARGET = main

BUILDDIR = build

all: $(TARGET)

$(TARGET): $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o $(BUILDDIR)/gmlfile.o $(BUILDDIR)/surface.o $(BUILDDIR)/main.o
	$(CC) $(BUILDDIR)/main.o $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o  $(BUILDDIR)/utils.o $(BUILDDIR)/gmlfile.o $(BUILDDIR)/surface.o -o $(TARGET)

$(BUILDDIR)/vector.o: geometry/vector.h geometry/vector.cpp
	$(CC) $(CFLAGS) geometry/vector.cpp -o $(BUILDDIR)/vector.o

$(BUILDDIR)/line.o: geometry/line.h geometry/line.cpp geometry/status_codes.h
	$(CC) $(CFLAGS) geometry/line.cpp -o $(BUILDDIR)/line.o

$(BUILDDIR)/plane.o: geometry/plane.h geometry/plane.cpp geometry/status_codes.h
	$(CC) $(CFLAGS) geometry/plane.cpp -o $(BUILDDIR)/plane.o

$(BUILDDIR)/polygon.o: geometry/polygon.h geometry/polygon.cpp geometry/status_codes.h
	$(CC) $(CFLAGS) geometry/polygon.cpp -o $(BUILDDIR)/polygon.o

$(BUILDDIR)/utils.o: geometry/utils.h geometry/utils.cpp
	$(CC) $(CFLAGS) geometry/utils.cpp -o $(BUILDDIR)/utils.o

$(BUILDDIR)/gmlfile.o: gml/gmlfile.h gml/gmlfile.cpp
	$(CC) $(CFLAGS) gml/gmlfile.cpp -o $(BUILDDIR)/gmlfile.o

$(BUILDDIR)/surface.o: gml/surface.h gml/surface.cpp
	$(CC) $(CFLAGS) gml/surface.cpp -o $(BUILDDIR)/surface.o

$(BUILDDIR)/main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o $(BUILDDIR)/main.o

test:
	$(CC) -c -g tests/test.cpp -o $(BUILDDIR)/test.o
	$(CC) -std=c++17 -g -lgtest $(BUILDDIR)/test.o $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o -o test

clean:
	rm -rf $(BUILDDIR)/*.o
	rm -rf *.o
	rm -f test main
