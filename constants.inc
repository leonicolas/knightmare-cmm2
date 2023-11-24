const false=0
const true=1

const KB_ESC=27
const KB_SPACE=32
const KB_UP=128
const KB_DOWN=129
const KB_LEFT=130
const KB_RIGHT=131

const MAP_TILESET_IMG="tiles/maps.png"
const OBJ_TILESET_IMG="tiles/objects.png"
const BOSSES_TILESET_IMG="tiles/bosses.png"
const MAPS_DIR="maps/"

' Buffers
const SCREEN_BUFFER=2                     ' Screen buffer number
const MAP_TILES_BUFFER=3                  ' Map tileset buffer number
const OBJ_TILES_BUFFER=4                  ' Objects tileset buffer number
const BOSSES_TILES_BUFFER=4               ' Bosses tileset buffer number

' Speed control
const SCROLL_SPEED_MS=135                 ' Scroll speed in ms
const GAME_TICK_MS=10                     ' Game tick in ms
const SHOT_COOLDOWN_MS=150
const PLAYER_ANIMATION_MS=300             ' Default player animation frame time in ms
const OBJECTS_ANIMATION_MS=50             ' Objects animation frame time in ms
const ANIM_TICK_DIVIDER=6                 ' Animation frame time tick divider. Used for slowing some animations
const HALF_ANIM_TICK_DIVIDER=ANIM_TICK_DIVIDER/2

' Map data
const MAP_COLS=32                         ' Map columns number
const MAP_ROWS=238                        ' Map rows number
const MAP_COLS_0=MAP_COLS-1               ' Zero based map columns number
const MAP_ROWS_0=MAP_ROWS-1               ' Zero based map rows number
const MAP_SIZE=MAP_COLS*MAP_ROWS          ' Total number of tiles in a map

const TILE_SIZE=8                         ' Tile size in pixels
const TILE_SIZEx2=TILE_SIZE*2             ' Double tile size
const TILE_SIZEx4=TILE_SIZE*4             ' Quadruple tile size
const TILES_COLS=32                       ' Tileset columns number
const TILES_ROWS=30                       ' Tileset rows numbers

const SCREEN_OFFSET=32                    ' Left screen offset in pixels
const SCREEN_ROWS=22                      ' Screen rows number
const SCREEN_WIDTH=MAP_COLS*TILE_SIZE     ' Screen width in pixels
const SCREEN_HEIGHT=SCREEN_ROWS*TILE_SIZE ' Screen height in pixels
const SCREEN_CENTER=SCREEN_WIDTH/2        ' Screen center

const SYMBOLS_X=0
const SYMBOLS_Y=48
const NUMBERS_X=24
const NUMBERS_Y=48
const LETTERS_X=0
const LETTERS_Y=56

const PLAYER_SKIN_Y=0
const PLAYER_SKIN1_X_L=0
const PLAYER_SKIN1_X_R=TILE_SIZE*2
const NEW_LIFE_POINTS=1000

const BLOCK_HITS=5
const BLOCK_POINTS=500

' Effects
const SHOT_SFX="sfx/shot.wav"
const ENEMY_DEATH_SFX="sfx/enemy_death.wav"
const PLAYER_DEATH_SFX="sfx/player_death.wav"
const NEW_LIFE_SFX="sfx/new_life.wav"
const POINTS_SFX="sfx/points.wav"
const KILL_ALL_ENEMIES_SFX="sfx/kill_all_enemies.wav"
const BLOCK_HIT_SFX="sfx/block_hit.wav"
const BLOCK_OPEN_SFX="sfx/block_open.wav"

' Music table
dim MUSIC$(9)=("music/01_intro-psg.mod", "music/02_majou-densetsu-psg.mod", "", "", "", "", "", "", "", "")

dim TRANSPARENT_BLOCK(1)=(80,64)

' Points
dim OBJ_POINTS(38)=(0,10,50,50,100,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)

' Objects tiles: tile x, y, width and height
' TODO: Move to data block
dim OBJ_TILE%(50,3)
' Enemies
' Blob
OBJ_TILE%(1,0)=1: OBJ_TILE%(1,1)=17: OBJ_TILE%(1,2)=14: OBJ_TILE%(1,3)=14
' Bat
OBJ_TILE%(2,0)=32: OBJ_TILE%(2,1)=16: OBJ_TILE%(2,2)=16: OBJ_TILE%(2,3)=11
' Bat Wave
OBJ_TILE%(3,0)=32: OBJ_TILE%(3,1)=16: OBJ_TILE%(3,2)=16: OBJ_TILE%(3,3)=11
' Knight
OBJ_TILE%(4,0)=64: OBJ_TILE%(4,1)=16: OBJ_TILE%(4,2)=16: OBJ_TILE%(4,3)=16

' VFX
' Shadow
OBJ_TILE%(39,0)=0: OBJ_TILE%(39,1)=33: OBJ_TILE%(39,2)=16: OBJ_TILE%(39,3)=6
' Shots
OBJ_TILE%(40,0)=1: OBJ_TILE%(40,1)=66: OBJ_TILE%(40,2)=5: OBJ_TILE%(40,3)=13
' Fire
OBJ_TILE%(20,0)=112: OBJ_TILE%(20,1)=104: OBJ_TILE%(20,2)=TILE_SIZEx2: OBJ_TILE%(20,3)=TILE_SIZEx2
dim FIRE_ANIM(15)=(1,0,1,0,1,0,2,1,2,1,2,1,3,2,3,2)
' Blocks
' Question mark
OBJ_TILE%(31,0)=0: OBJ_TILE%(31,1)=104: OBJ_TILE%(31,2)=TILE_SIZEx2: OBJ_TILE%(31,3)=TILE_SIZEx2
' Rook
OBJ_TILE%(32,0)=16: OBJ_TILE%(32,1)=104: OBJ_TILE%(32,2)=TILE_SIZEx2: OBJ_TILE%(32,3)=TILE_SIZEx2
' Knight
OBJ_TILE%(33,0)=32: OBJ_TILE%(33,1)=104: OBJ_TILE%(33,2)=TILE_SIZEx2: OBJ_TILE%(33,3)=TILE_SIZEx2
' Queen
OBJ_TILE%(34,0)=48: OBJ_TILE%(34,1)=104: OBJ_TILE%(34,2)=TILE_SIZEx2: OBJ_TILE%(34,3)=TILE_SIZEx2
' King
OBJ_TILE%(35,0)=64: OBJ_TILE%(35,1)=104: OBJ_TILE%(35,2)=TILE_SIZEx2: OBJ_TILE%(35,3)=TILE_SIZEx2
' Barrier
OBJ_TILE%(36,0)=80: OBJ_TILE%(36,1)=104: OBJ_TILE%(36,2)=TILE_SIZEx2: OBJ_TILE%(36,3)=TILE_SIZEx2
' Empty
OBJ_TILE%(37,0)=96: OBJ_TILE%(37,1)=104: OBJ_TILE%(37,2)=TILE_SIZEx2: OBJ_TILE%(37,3)=TILE_SIZEx2

' Object data: speed X, speed Y, auxiliary data 1 and 2
' TODO: Move to data block
dim OBJ_DATA(38,4)
' Blob
OBJ_DATA(1,1)=0.15   ' Y speed
OBJ_DATA(1,2)=1      ' Life
' Bat
OBJ_DATA(2,0)=0.7    ' X speed (angle increment)
OBJ_DATA(2,1)=0.6    ' Y speed
OBJ_DATA(2,2)=1      ' Life
OBJ_DATA(2,3)=90     ' Angle
const BAT_SPAWN_SPEED_MS=500
' Bat wave
OBJ_DATA(3,0)=1.2    ' X speed (angle increment)
OBJ_DATA(3,1)=0.33   ' Y speed
OBJ_DATA(3,2)=1      ' Life
const BAT_WAVE_SPAWN_SPEED_MS=200
' Knight
OBJ_DATA(4,0)=0.4    ' X speed - applied when it changes to horizontal movement
OBJ_DATA(4,1)=0.4    ' Y speed
OBJ_DATA(4,2)=3      ' Life
const KNIGHT_CHANGE_DIRECT_DIST_PX=55
const KNIGHT_MAX_HORIZONTAL_DIST_PX=48
