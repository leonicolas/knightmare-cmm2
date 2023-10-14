const KB_SPACE=32
const KB_ESC=27

const TILESET_IMG="tiles/km_tileset.png"
const MAPS_DIR="maps/"

const SCROLL_SPEED=135                    ' Scroll speed in ms

const TILES_BUFFER=5                      ' Tileset buffer number
const TILE_SIZE=8                         ' Tile size in pixels
const TILES_COLS=32                       ' Tileset columns number
const TILES_ROWS=30                       ' Tileset rows numbers

const MAP_COLS=32                         ' Map columns number
const MAP_ROWS=238                        ' Map rows number
const MAP_COLS_0=MAP_COLS-1               ' Zero based map columns number
const MAP_ROWS_0=MAP_ROWS-1               ' Zero based map rows number
const MAP_SIZE=MAP_COLS*MAP_ROWS          ' Total number of tiles in a map

const SCREEN_OFFSET=32                    ' Left screen offset in pixels
const SCREEN_BUFFER=2                     ' Screen buffer number
const SCREEN_ROWS=22                      ' Screen rows number
const SCREEN_WIDTH=MAP_COLS*TILE_SIZE     ' Screen width in pixels
const SCREEN_HEIGHT=SCREEN_ROWS*TILE_SIZE ' Screen height in pixels
