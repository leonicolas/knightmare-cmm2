#define "MAP_TILESET_IMG", "tiles/maps.png"
#define "OBJ_TILESET_IMG", "tiles/objects.png"
#define "BOSS_TILESET_IMG", "tiles/bosses.png"
#define "MAPS_DIR", "maps/"

const false=0
const true=1
const left=-1
const right=1

const KB_ESC=27
const KB_SPACE=32
const KB_UP=128
const KB_DOWN=129
const KB_LEFT=130
const KB_RIGHT=131

' Buffers
const SCREEN_BUFFER=2                     ' Screen buffer number
const MAP_TILES_BUFFER=3                  ' Map tileset buffer number
const OBJ_TILES_BUFFER=4                  ' Objects tileset buffer number
const BOSS_TILES_BUFFER=5                 ' Bosses tileset buffer number
const SPRITES_BUFFER=6

' Speed control
const GAME_TICK_MS=1000/120               ' Game tick in ms for max 120 FPS
const SHOT_COOLDOWN_MS=150                ' Player shot cooldown in ms

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

const SCREEN_OFFSET=32                    ' Left screen offset in pixels
const SCREEN_ROWS=22                      ' Screen rows number
const SCREEN_COLS=MAP_COLS                ' Screen cols number
const SCREEN_WIDTH=MAP_COLS*TILE_SIZE     ' Screen width in pixels
const SCREEN_HEIGHT=SCREEN_ROWS*TILE_SIZE ' Screen height in pixels
const SCREEN_CENTER_W=SCREEN_WIDTH/2      ' Screen center
const SCREEN_CENTER_H=SCREEN_HEIGHT/2     ' Screen center
const EDGE_LEFT=TILE_SIZEx4               ' Left Edge of the screen
const EDGE_RIGHT=SCREEN_WIDTH-TILE_SIZEx4 ' Right Edge of the screen

const SYMBOLS_X=0
const SYMBOLS_Y=48
const NUMBERS_X=24
const NUMBERS_Y=48
const LETTERS_X=0
const LETTERS_Y=56
const TRANSPARENT_BLOCK_X=96
const TRANSPARENT_BLOCK_Y=144

dim PLAYER_DEATH_ANIM(25)=(0,12,0,12,0,12,0,12,0,12,0,12,0,12,0,12,14,16,18,20,22,22,22,22,22,22)
const PLAYER_SKIN_Y=0
const PLAYER_SKIN1_X_L=0
const PLAYER_SKIN1_X_R=TILE_SIZE*2
const PLAYER_INIT_SPEED=60
const PLAYER_INIT_COL=SCREEN_COLS/2-1
const PLAYER_INIT_ROW=SCREEN_ROWS-1
const PLAYER_SPEED_INC=20   ' Speed increment
const PLAYER_MAX_SPEED=PLAYER_INIT_SPEED+PLAYER_SPEED_INC*3
const NEW_LIFE_POINTS=100000
const SHIELD_MAX_HITS=30
dim BOSS_LIFE(8)=(0,20,48,30,48,40,40,40,0)
const BOSS_SPEED=60

const MAX_ENEMIES_SHOOT_Y=SCREEN_HEIGHT*0.6
'Chance of enemy shoot per stage - Easy, Normal, Hard
dim SHOOT_CHANCE(2,8)=(0,0,0,  0.3,0.5,0.7,  0.3,0.5,0.7,  0.4,0.6,0.8,  0.4,0.6,0.8,  0.5,0.7,0.9,  0.5,0.7,0.9,  0.6,0.8,1,  0.6,0.8,1)

const BLOCK_HITS=5

const BLOCK_POINTS=500
const FREEZE_TIME=10
const POWER_UP_TIME=45

' Map Tiles Offset
dim TILES_OFFSET(7)=(0,TILE_SIZE*3,TILE_SIZE*5,TILE_SIZE*9,TILE_SIZE*12,TILE_SIZE*15,TILE_SIZE*15,TILE_SIZE*20)

' Points
dim OBJ_POINTS(38)=(0,10,50,50,100,200,50,100,200,200,100,100,300,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)

' Objects tiles: 0: tile x | 1: tile y | 2: tile width | 3: tile height | 4: obj data index
dim OBJ_TILE%(50,4)
' Enemies
' Blob
OBJ_TILE%(1,0)=1: OBJ_TILE%(1,1)=17: OBJ_TILE%(1,2)=14: OBJ_TILE%(1,3)=14: OBJ_TILE%(1,4)=1
' Bat
OBJ_TILE%(2,0)=32: OBJ_TILE%(2,1)=16: OBJ_TILE%(2,2)=16: OBJ_TILE%(2,3)=11: OBJ_TILE%(2,4)=2
' Bat Wave
OBJ_TILE%(3,0)=32: OBJ_TILE%(3,1)=16: OBJ_TILE%(3,2)=16: OBJ_TILE%(3,3)=11: OBJ_TILE%(3,4)=3
' Knight
OBJ_TILE%(4,0)=64: OBJ_TILE%(4,1)=16: OBJ_TILE%(4,2)=16: OBJ_TILE%(4,3)=16: OBJ_TILE%(4,4)=4
' Cloud
OBJ_TILE%(5,0)=113: OBJ_TILE%(5,1)=17: OBJ_TILE%(5,2)=14: OBJ_TILE%(5,3)=14: OBJ_TILE%(5,4)=5
' Blue Demon
OBJ_TILE%(6,0)=144: OBJ_TILE%(6,1)=16: OBJ_TILE%(6,2)=16: OBJ_TILE%(6,3)=16: OBJ_TILE%(6,4)=6
' Skeleton
OBJ_TILE%(7,0)=176: OBJ_TILE%(7,1)=16: OBJ_TILE%(7,2)=16: OBJ_TILE%(7,3)=16: OBJ_TILE%(7,4)=7
' Demon
OBJ_TILE%(8,0)=240: OBJ_TILE%(8,1)=17: OBJ_TILE%(8,2)=16: OBJ_TILE%(8,3)=15: OBJ_TILE%(8,4)=8
' Death ghost
OBJ_TILE%(9,0)=16: OBJ_TILE%(9,1)=136: OBJ_TILE%(9,2)=15: OBJ_TILE%(9,3)=16: OBJ_TILE%(9,4)=9
' Zombie
OBJ_TILE%(10,0)=64: OBJ_TILE%(10,1)=136: OBJ_TILE%(10,2)=16: OBJ_TILE%(10,3)=16: OBJ_TILE%(10,4)=10
' Ghost
OBJ_TILE%(11,0)=16: OBJ_TILE%(11,1)=120: OBJ_TILE%(11,2)=16: OBJ_TILE%(11,3)=16: OBJ_TILE%(11,4)=11
' Yellow thing
OBJ_TILE%(12,0)=209: OBJ_TILE%(12,1)=2: OBJ_TILE%(12,2)=13: OBJ_TILE%(12,3)=13: OBJ_TILE%(12,4)=12
' Red thing
OBJ_TILE%(13,0)=224: OBJ_TILE%(13,1)=1: OBJ_TILE%(13,2)=15: OBJ_TILE%(13,3)=14: OBJ_TILE%(13,4)=13
' Bat Reverse
OBJ_TILE%(14,0)=32: OBJ_TILE%(14,1)=16: OBJ_TILE%(14,2)=16: OBJ_TILE%(14,3)=11: OBJ_TILE%(14,4)=14
' Sorcerer
OBJ_TILE%(15,0)=0: OBJ_TILE%(15,1)=120: OBJ_TILE%(15,2)=16: OBJ_TILE%(15,3)=16: OBJ_TILE%(15,4)=15
' Red Death ghost
OBJ_TILE%(16,0)=32: OBJ_TILE%(16,1)=136: OBJ_TILE%(16,2)=15: OBJ_TILE%(16,3)=16: OBJ_TILE%(16,4)=16

' Weapon crystals
OBJ_TILE%(20,0)=0: OBJ_TILE%(20,1)=64: OBJ_TILE%(20,2)=16: OBJ_TILE%(20,3)=16: OBJ_TILE%(20,4)=20
' Power Up crystals
OBJ_TILE%(21,0)=0: OBJ_TILE%(21,1)=80: OBJ_TILE%(21,2)=16: OBJ_TILE%(21,3)=16: OBJ_TILE%(21,4)=21
dim WEAPON_ANIM(5,3)=(0,3,5,7,9,11, 1,4,6,8,10,12, 0,3,5,7,9,11, 2,1,1,1,1,1)
dim PUP_ANIM(4,3)=(0,3,6,9,12, 1,4,7,10,13, 0,3,6,9,12, 2,5,8,11,14)
' Blocks
OBJ_TILE%(22,0)=0: OBJ_TILE%(22,1)=104: OBJ_TILE%(22,2)=16: OBJ_TILE%(22,3)=16


' Enemies shots
' Dot bullet
OBJ_TILE%(23,0)=22: OBJ_TILE%(23,1)=38: OBJ_TILE%(23,2)=4: OBJ_TILE%(23,3)=4
' Energy ray
OBJ_TILE%(24,0)=35: OBJ_TILE%(24,1)=35: OBJ_TILE%(24,2)=10: OBJ_TILE%(24,3)=10
' Bone
OBJ_TILE%(25,0)=50: OBJ_TILE%(25,1)=37: OBJ_TILE%(25,2)=12: OBJ_TILE%(25,3)=6
OBJ_TILE%(26,0)=66: OBJ_TILE%(26,1)=35: OBJ_TILE%(26,2)=11: OBJ_TILE%(26,3)=11
OBJ_TILE%(27,0)=85: OBJ_TILE%(27,1)=34: OBJ_TILE%(27,2)=6: OBJ_TILE%(27,3)=12
' White explosion
OBJ_TILE%(28,0)=99: OBJ_TILE%(28,1)=35: OBJ_TILE%(28,2)=9: OBJ_TILE%(28,3)=9
' Arrow
OBJ_TILE%(29,0)=133: OBJ_TILE%(29,1)=34: OBJ_TILE%(29,2)=5: OBJ_TILE%(29,3)=14
OBJ_TILE%(30,0)=145: OBJ_TILE%(30,1)=38: OBJ_TILE%(30,2)=13: OBJ_TILE%(30,3)=5
OBJ_TILE%(31,0)=163: OBJ_TILE%(31,1)=35: OBJ_TILE%(31,2)=11: OBJ_TILE%(31,3)=11
' Axe
OBJ_TILE%(32,0)=177: OBJ_TILE%(32,1)=34: OBJ_TILE%(32,2)=13: OBJ_TILE%(32,3)=13
' Scythe
OBJ_TILE%(33,0)=48: OBJ_TILE%(33,1)=120: OBJ_TILE%(33,2)=16: OBJ_TILE%(33,3)=16
' Fire
OBJ_TILE%(34,0)=112: OBJ_TILE%(34,1)=104: OBJ_TILE%(34,2)=16: OBJ_TILE%(34,3)=16
dim FIRE_ANIM(15)=(1,0,1,0,1,0,2,1,2,1,2,1,3,2,3,2)
' Big Fire
OBJ_TILE%(35,0)=112: OBJ_TILE%(35,1)=120: OBJ_TILE%(35,2)=16: OBJ_TILE%(35,3)=16
' Shield
OBJ_TILE%(36,0)=136: OBJ_TILE%(36,1)=48: OBJ_TILE%(36,2)=16: OBJ_TILE%(36,3)=7


' Boss Shadow
OBJ_TILE%(37,0)=200: OBJ_TILE%(37,1)=98: OBJ_TILE%(37,2)=40: OBJ_TILE%(37,3)=6
' Big Shadow
OBJ_TILE%(38,0)=184: OBJ_TILE%(38,1)=49: OBJ_TILE%(38,2)=24: OBJ_TILE%(38,3)=6
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
OBJ_TILE%(43,0)=209: OBJ_TILE%(43,1)=35: OBJ_TILE%(43,2)=5: OBJ_TILE%(43,3)=10
OBJ_TILE%(44,0)=218: OBJ_TILE%(44,1)=37: OBJ_TILE%(44,2)=10: OBJ_TILE%(44,3)=5
' Sword
OBJ_TILE%(45,0)=225: OBJ_TILE%(45,1)=48: OBJ_TILE%(45,2)=6: OBJ_TILE%(45,3)=16
' Double Sword
OBJ_TILE%(46,0)=226: OBJ_TILE%(46,1)=64: OBJ_TILE%(46,2)=13: OBJ_TILE%(46,3)=16
' Fire arrow
OBJ_TILE%(47,0)=232: OBJ_TILE%(47,1)=49: OBJ_TILE%(47,2)=8: OBJ_TILE%(47,3)=14
' Bridge
OBJ_TILE%(48,0)=184: OBJ_TILE%(48,1)=136: OBJ_TILE%(48,2)=32: OBJ_TILE%(48,3)=8
' Terrain
OBJ_TILE%(49,0)=240: OBJ_TILE%(49,1)=192: OBJ_TILE%(49,2)=16: OBJ_TILE%(49,3)=8

' Object data: 0: speed X | 1: speed Y | 2: GPR 1 | 3: GPR 2 | 4: GPR 3
' TODO: Move to data block
dim OBJ_DATA(21,4)
' Blob
OBJ_DATA(1,1)=20     ' Y speed
OBJ_DATA(1,2)=1      ' Life
' Bat
OBJ_DATA(2,0)=75     ' X speed (angle increment)
OBJ_DATA(2,1)=65     ' Y speed
OBJ_DATA(2,2)=1      ' Life
OBJ_DATA(2,3)=90     ' Angle
' Bat Reverse
OBJ_DATA(14,0)=50     ' X speed (angle increment)
OBJ_DATA(14,1)=-35    ' Y speed
OBJ_DATA(14,2)=1      ' Life
OBJ_DATA(14,3)=90     ' Angle
' Bat wave
OBJ_DATA(3,0)=120    ' X speed (angle increment)
OBJ_DATA(3,1)=33     ' Y speed
OBJ_DATA(3,2)=1      ' Life
' Knight
OBJ_DATA(4,0)=40     ' X speed - applied when it changes to horizontal movement
OBJ_DATA(4,1)=40     ' Y speed
OBJ_DATA(4,2)=3      ' Life
const KNIGHT_CHANGE_DIRECT_DIST_PX=55
const KNIGHT_MAX_HORIZONTAL_DIST_PX=48
' Cloud
OBJ_DATA(5,0)=25     ' X speed
OBJ_DATA(5,1)=360    ' Cycle angle before changing row
const CLOUD_ATTACK_SPEED=250
' Blue demon
OBJ_DATA(6,0)=70     ' X speed
OBJ_DATA(6,1)=70     ' Y speed
OBJ_DATA(6,2)=1      ' Life
const DEMON_CHANGE_DIRECT_DIST_PX=55
' Skeleton
OBJ_DATA(7,0)=50     ' X speed
OBJ_DATA(7,1)=15     ' Y speed
OBJ_DATA(7,2)=3      ' Life
' Black demon
OBJ_DATA(8,0)=160    ' X speed
OBJ_DATA(8,1)=160    ' Y speed
OBJ_DATA(8,2)=1      ' Life
const DEMON_ATTACK_DIST_PX=60
' Death ghost
OBJ_DATA(9,0)=90     ' X speed (angle increment)
OBJ_DATA(9,1)=80     ' Y speed
OBJ_DATA(9,2)=1      ' Life
' Zombie
OBJ_DATA(10,1)=25    ' Y speed
OBJ_DATA(10,2)=1     ' Life
OBJ_DATA(10,3)=60    ' Y speed after split
' Ghost
OBJ_DATA(11,0)=90    ' X speed (angle increment)
OBJ_DATA(11,1)=31    ' Y speed
OBJ_DATA(11,2)=1     ' Life
' Yellow thing
OBJ_DATA(12,0)=240   ' X speed
OBJ_DATA(12,1)=240   ' Y speed
OBJ_DATA(12,2)=1     ' Life
' Red thing
OBJ_DATA(13,0)=140   ' X speed
OBJ_DATA(13,1)=50    ' Y speed
OBJ_DATA(13,2)=1     ' Life
' Sorcerer
OBJ_DATA(15,1)=20    ' Y speed
OBJ_DATA(15,2)=1     ' Life
' Red Death ghost
OBJ_DATA(16,0)=40     ' X speed
OBJ_DATA(16,1)=20     ' Y speed
OBJ_DATA(16,2)=1      ' Life
' Weapon chrystal
OBJ_DATA(20,0)=50    ' X speed (angle increment)
OBJ_DATA(20,1)=20    ' Y speed
OBJ_DATA(20,4)=-1    ' Angle for the wobbling movement. -1 means disabled
' Power-up chrystal
OBJ_DATA(21,0)=50    ' X speed (angle increment)
OBJ_DATA(21,1)=20    ' Y speed
OBJ_DATA(21,4)=-1    ' Angle for the wobbling movement. -1 means disabled
