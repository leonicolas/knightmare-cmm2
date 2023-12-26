const false=0
const true=1

const KB_ESC=27
const KB_SPACE=32
const KB_UP=128
const KB_DOWN=129
const KB_LEFT=130
const KB_RIGHT=131

#define "MAP_TILESET_IMG", "tiles/maps.png"
#define "OBJ_TILESET_IMG", "tiles/objects.png"
#define "BOSSES_TILESET_IMG", "tiles/bosses.png"
#define "MAPS_DIR", "maps/"

' Effects
#define "SHOT_SFX", "sfx/shot.wav"
#define "BOOMERANG_SFX", "sfx/shot_boomerang.wav"
#define "FIRE_ARROW_SFX", "sfx/shot_fire_arrow.wav"
#define "FLAME_SFX", "sfx/shot_flame.wav"
#define "SWORD_SFX", "sfx/shot_sword.wav"
#define "ENEMY_DEATH_SFX", "sfx/enemy_death.wav"
#define "PLAYER_DEATH_SFX", "sfx/player_death.wav"
#define "NEW_LIFE_SFX", "sfx/new_life.wav"
#define "POINTS_SFX", "sfx/points.wav"
#define "KILL_ALL_ENEMIES_SFX", "sfx/kill_all_enemies.wav"
#define "BLOCK_HIT_SFX", "sfx/block_hit.wav"
#define "BLOCK_OPEN_SFX", "sfx/block_open.wav"
#define "FREEZE_TICK_SFX", "sfx/freeze_tick.wav"
#define "CHRYSTAL_SFX", "sfx/chrystal.wav"
#define "POWER_UP_HIT_SFX", "sfx/power_up_hit.wav"
#define "POWER_UP_ENDING", "sfx/power_up_ending.wav"

' Buffers
const SCREEN_BUFFER=2                     ' Screen buffer number
const MAP_TILES_BUFFER=3                  ' Map tileset buffer number
const OBJ_TILES_BUFFER=4                  ' Objects tileset buffer number
const BOSSES_TILES_BUFFER=5               ' Bosses tileset buffer number
const SPRITES_BUFFER=6

' Speed control
const SCROLL_SPEED_MS=128                 ' Scroll speed in ms
const GAME_TICK_MS=1000/120               ' Game tick in ms for max 120 FPS
const SHOT_COOLDOWN_MS=150                ' Player shot cooldown in ms
const ENEMIES_SHOTS_MS=2000               ' Enemies shots in ms
const ANIM_TICK_MS=50                     ' Animation frame time in ms

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
const TRANSPARENT_BLOCK_X=224
const TRANSPARENT_BLOCK_Y=104

const PLAYER_SKIN_Y=0
const PLAYER_SKIN1_X_L=0
const PLAYER_SKIN1_X_R=TILE_SIZE*2
const PLAYER_INIT_SPEED=60
const PLAYER_SPEED_INC=20   ' Speed increment
const PLAYER_MAX_SPEED=PLAYER_INIT_SPEED+PLAYER_SPEED_INC*3
const NEW_LIFE_POINTS=100000
const SHIELD_MAX_HITS=40

const MAX_ENEMIES_SHOOT_Y=SCREEN_HEIGHT*0.6
const ENEMIES_SHOOT_CHANCE=0.7

const BLOCK_HITS=5
const BLOCK_POINTS=500
const FREEZE_TIME=10
const POWER_UP_TIME=45


' Music table
dim MUSIC$(9)=("music/01_intro-psg.mod", "music/02_majou-densetsu-psg.mod", "", "", "", "", "", "", "", "")

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

' Fire
OBJ_TILE%(20,0)=112: OBJ_TILE%(20,1)=104: OBJ_TILE%(20,2)=16: OBJ_TILE%(20,3)=16
dim FIRE_ANIM(15)=(1,0,1,0,1,0,2,1,2,1,2,1,3,2,3,2)
' Pink Fire
OBJ_TILE%(21,0)=176: OBJ_TILE%(21,1)=104: OBJ_TILE%(21,2)=16: OBJ_TILE%(21,3)=16
' Enemies shots
' Dot bullet
OBJ_TILE%(22,0)=22: OBJ_TILE%(22,1)=38: OBJ_TILE%(22,2)=4: OBJ_TILE%(22,3)=4
' Arrow
OBJ_TILE%(23,0)=133: OBJ_TILE%(23,1)=34: OBJ_TILE%(23,2)=5: OBJ_TILE%(23,3)=14
OBJ_TILE%(24,0)=145: OBJ_TILE%(24,1)=38: OBJ_TILE%(24,2)=13: OBJ_TILE%(24,3)=5
OBJ_TILE%(25,0)=163: OBJ_TILE%(25,1)=35: OBJ_TILE%(25,2)=11: OBJ_TILE%(25,3)=11
' Weapon crystals
OBJ_TILE%(29,0)=0: OBJ_TILE%(29,1)=64: OBJ_TILE%(29,2)=16: OBJ_TILE%(29,3)=16
' Power Up crystals
OBJ_TILE%(30,0)=0: OBJ_TILE%(30,1)=80: OBJ_TILE%(30,2)=16: OBJ_TILE%(30,3)=16
dim WEAPON_ANIM(5,3)=(0,3,5,7,9,11, 1,4,6,8,10,12, 0,3,5,7,9,11, 2,1,1,1,1,1)
' Blocks
OBJ_TILE%(31,0)=0: OBJ_TILE%(31,1)=104: OBJ_TILE%(31,2)=16: OBJ_TILE%(31,3)=16
dim PUP_ANIM(4,3)=(0,3,6,9,12, 1,4,7,10,13, 0,3,6,9,12, 2,5,8,11,14)
' Shield
OBJ_TILE%(32,0)=112: OBJ_TILE%(32,1)=48: OBJ_TILE%(32,2)=16: OBJ_TILE%(32,3)=7
' Shadow
OBJ_TILE%(39,0)=0: OBJ_TILE%(39,1)=33: OBJ_TILE%(39,2)=16: OBJ_TILE%(39,3)=6
' Player Shots
' Arrow
OBJ_TILE%(40,0)=209: OBJ_TILE%(40,1)=50: OBJ_TILE%(40,2)=5: OBJ_TILE%(40,3)=13
' Twin Arrows
OBJ_TILE%(41,0)=210: OBJ_TILE%(41,1)=66: OBJ_TILE%(41,2)=12: OBJ_TILE%(41,3)=13
' Triple flames
OBJ_TILE%(42,0)=218: OBJ_TILE%(42,1)=54: OBJ_TILE%(42,2)=4: OBJ_TILE%(42,3)=10
' Boomerang
OBJ_TILE%(43,0)=208: OBJ_TILE%(43,1)=35: OBJ_TILE%(43,2)=6: OBJ_TILE%(43,3)=10
OBJ_TILE%(44,0)=218: OBJ_TILE%(44,1)=37: OBJ_TILE%(44,2)=10: OBJ_TILE%(44,3)=5
' Sword
OBJ_TILE%(45,0)=225: OBJ_TILE%(45,1)=48: OBJ_TILE%(45,2)=6: OBJ_TILE%(45,3)=16
' Double Sword
OBJ_TILE%(46,0)=226: OBJ_TILE%(46,1)=64: OBJ_TILE%(46,2)=13: OBJ_TILE%(46,3)=16
' Fire arrow
OBJ_TILE%(47,0)=232: OBJ_TILE%(47,1)=49: OBJ_TILE%(47,2)=8: OBJ_TILE%(47,3)=14

' Object data: 0: speed X | 1: speed Y | 2: GPR 1 | 3: GPR 2 | 4: GPR 3
' TODO: Move to data block
dim OBJ_DATA(38,4)
' Blob
OBJ_DATA(1,1)=20     ' Y speed
OBJ_DATA(1,2)=1      ' Life
const BLOB_SHOT_SPEED=70
' Bat
OBJ_DATA(2,0)=75     ' X speed (angle increment)
OBJ_DATA(2,1)=65     ' Y speed
OBJ_DATA(2,2)=1      ' Life
OBJ_DATA(2,3)=90     ' Angle
const BAT_SPAWN_SPEED_MS=500
const BAT_SHOT_SPEED=70
' Bat wave
OBJ_DATA(3,0)=120    ' X speed (angle increment)
OBJ_DATA(3,1)=33     ' Y speed
OBJ_DATA(3,2)=1      ' Life
const BAT_WAVE_SPAWN_SPEED_MS=200
' Knight
OBJ_DATA(4,0)=40     ' X speed - applied when it changes to horizontal movement
OBJ_DATA(4,1)=40     ' Y speed
OBJ_DATA(4,2)=3      ' Life
const KNIGHT_CHANGE_DIRECT_DIST_PX=55
const KNIGHT_MAX_HORIZONTAL_DIST_PX=48
const KNIGHT_SHOT_SPEED=70
' Weapon chrystal
OBJ_DATA(29,0)=50   ' X speed (angle increment)
OBJ_DATA(29,1)=20   ' Y speed
OBJ_DATA(29,4)=-1   ' Angle for the wobbling movement. -1 means disabled
' Power-up chrystal
OBJ_DATA(30,0)=50   ' X speed (angle increment)
OBJ_DATA(30,1)=20   ' Y speed
OBJ_DATA(30,4)=-1   ' Angle for the wobbling movement. -1 means disabled
