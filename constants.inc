const KB_ESC=27
const KB_SPACE=32
const KB_UP=128
const KB_DOWN=129
const KB_LEFT=130
const KB_RIGHT=131

const TILESET_IMG="tiles/km_tileset.png"
const MAPS_DIR="maps/"

' const PLAYER_ANIM_SPEED1
const SCROLL_SPEED_MS=135                 ' Scroll speed in ms
const GAME_TICK_MS=10                     ' Game tick in ms

const MAP_COLS=32                         ' Map columns number
const MAP_ROWS=238                        ' Map rows number
const MAP_COLS_0=MAP_COLS-1               ' Zero based map columns number
const MAP_ROWS_0=MAP_ROWS-1               ' Zero based map rows number
const MAP_SIZE=MAP_COLS*MAP_ROWS          ' Total number of tiles in a map

const TILES_BUFFER=5                      ' Tileset buffer number
const TILE_SIZE=8                         ' Tile size in pixels
const TILE_SIZEx2=TILE_SIZE*2             ' Double tile size
const TILE_SIZEx4=TILE_SIZE*4             ' Quadruple tile size
const TILES_COLS=32                       ' Tileset columns number
const TILES_ROWS=30                       ' Tileset rows numbers

const SCREEN_BUFFER=2                     ' Screen buffer number
const SCREEN_OFFSET=32                    ' Left screen offset in pixels
const SCREEN_ROWS=22                      ' Screen rows number
const SCREEN_WIDTH=MAP_COLS*TILE_SIZE     ' Screen width in pixels
const SCREEN_HEIGHT=SCREEN_ROWS*TILE_SIZE ' Screen height in pixels
const SCREEN_CENTER=SCREEN_WIDTH/2        ' Screen center

const PLAYER_SKIN_Y=TILE_SIZE*14
const PLAYER_SKIN1_X_L=0
const PLAYER_SKIN1_X_R=TILE_SIZE*2

const WEAPON_Y=TILE_SIZE*19
const WEAPON_ARROW_X=0

const OBJECTS_ANIMATION_MS=140

const SHOT_EFFECT="sfx/shot.wav"

' Mapping tables
dim MUSIC$(9)=("music/01_intro-psg.mod", "music/02_majou-densetsu-psg.mod", "", "", "", "", "", "", "", "")
' Objects tiles position. First slot is not used
dim OBJ_TILE_Y%=128
dim OBJ_TILE_X%(13)=(0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
' Object speed X and Y and auxiliary data. First slot is not used
dim OBJ_DATA(13,2)
' Bubble
OBJ_DATA(1,1)=0.15   ' Y speed
' Bat
OBJ_DATA(2,0)=1.2    ' X speed (angle increment)
OBJ_DATA(2,1)=0.2    ' Y speed
' Bat wave
OBJ_DATA(3,0)=1.2    ' X speed (angle increment)
OBJ_DATA(3,1)=0.33   ' Y speed
