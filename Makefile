CC = g++
CFLAGS = -c -Wall -g --std=c++23
LDFLAGS = -lcurl -ltiff

TARGET = main

SRCDIR = src
BUILDDIR = build

all: $(TARGET)

$(TARGET): $(BUILDDIR)/main.o $(SRCDIR)/status_codes.h $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o $(BUILDDIR)/gmlfile.o $(BUILDDIR)/geotiff.o $(BUILDDIR)/vector_tile.o $(BUILDDIR)/grid_tile.o $(BUILDDIR)/surface.o $(BUILDDIR)/download.o $(BUILDDIR)/field.o $(BUILDDIR)/config.o $(BUILDDIR)/fresnel_zone.o $(BUILDDIR)/precalc.o $(BUILDDIR)/raytracing.o $(BUILDDIR)/load_tile.o
	$(CC) $(LDFLAGS) $(BUILDDIR)/*.o $(SRCDIR)/status_codes.h -o $(TARGET)

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

$(BUILDDIR)/geotiff.o: $(SRCDIR)/raw_data/geotiff.h $(SRCDIR)/raw_data/geotiff.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/raw_data/geotiff.cpp -o $(BUILDDIR)/geotiff.o

$(BUILDDIR)/vector_tile.o: $(SRCDIR)/tile/vector_tile.h $(SRCDIR)/tile/vector_tile.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/tile/vector_tile.cpp -o $(BUILDDIR)/vector_tile.o

$(BUILDDIR)/grid_tile.o: $(SRCDIR)/tile/grid_tile.h $(SRCDIR)/tile/grid_tile.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/tile/grid_tile.cpp -o $(BUILDDIR)/grid_tile.o

$(BUILDDIR)/surface.o: $(SRCDIR)/raw_data/surface.h $(SRCDIR)/raw_data/surface.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/raw_data/surface.cpp -o $(BUILDDIR)/surface.o

$(BUILDDIR)/download.o: $(SRCDIR)/web/download.h $(SRCDIR)/web/download.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/web/download.cpp -o $(BUILDDIR)/download.o

$(BUILDDIR)/field.o: $(SRCDIR)/tile/field.h $(SRCDIR)/tile/field.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/tile/field.cpp -o $(BUILDDIR)/field.o

$(BUILDDIR)/config.o: $(SRCDIR)/config/config.h $(SRCDIR)/config/config.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/config/config.cpp -o $(BUILDDIR)/config.o

$(BUILDDIR)/fresnel_zone.o: $(SRCDIR)/precalc/fresnel_zone.h $(SRCDIR)/precalc/fresnel_zone.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/precalc/fresnel_zone.cpp -o $(BUILDDIR)/fresnel_zone.o

$(BUILDDIR)/load_tile.o: $(SRCDIR)/tile/load_tile.h $(SRCDIR)/tile/load_tile.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/tile/load_tile.cpp -o $(BUILDDIR)/load_tile.o

$(BUILDDIR)/precalc.o: $(SRCDIR)/precalc/precalc.h $(SRCDIR)/precalc/precalc.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/precalc/precalc.cpp -o $(BUILDDIR)/precalc.o

$(BUILDDIR)/raytracing.o: $(SRCDIR)/raytracing/raytracing.h $(SRCDIR)/raytracing/raytracing.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/raytracing/raytracing.cpp -o $(BUILDDIR)/raytracing.o

$(BUILDDIR)/main.o: $(SRCDIR)/main.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/main.cpp -o $(BUILDDIR)/main.o

test: $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o tests/*.cpp tests/*.h
	$(CC) -c -g tests/test.cpp -o $(BUILDDIR)/tests/test.o
	$(CC) -std=c++17 -g -lgtest $(BUILDDIR)/tests/test.o $(BUILDDIR)/vector.o $(BUILDDIR)/line.o $(BUILDDIR)/plane.o $(BUILDDIR)/polygon.o $(BUILDDIR)/utils.o -o test

clean:
	rm -rf $(BUILDDIR)/*.o $(BUILDDIR)/tests/*.o
	rm -f test main
