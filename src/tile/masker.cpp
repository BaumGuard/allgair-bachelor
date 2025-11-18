#include "masker.h"

#include "../status_codes.h"

int maskTile ( GridTile& masked, GridTile& tile1, GridTile& tile2 ) {
    if ( tile1.getTileWidth() != tile2.getTileWidth() ) {
        return TILE_SIZES_UNEQUAL;
    }

    int width = tile1.getTileWidth();
    masked = GridTile( width );

    float
        tile1_value,
        tile2_value,
        masked_value;

    for ( int y = 0; y < width; y++ ) {
        for ( int x = 0; x < width; x++ ) {
            tile1.getValue( x, y, tile1_value );
            tile2.getValue( x, y, tile2_value );

            masked_value = tile1_value - tile2_value;

            masked.setValue( x, y, masked_value );
        }
    }

    return TILES_MASKED;
} /* maskTile() */
