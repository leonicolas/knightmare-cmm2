option explicit
option base 0
option angle degrees
option default float

#include "constants.inc"

' Global variables
dim g_sound_on%=0
dim g_debug%=1
dim g_map(MAP_SIZE) as integer ' Map data
dim g_row%                     ' Current top map row. Zero is the bottom row.
dim g_game_tick                ' The game tick
dim g_tile_px%                 ' The rendered tile row pixel
dim g_kb1%,g_kb2%,g_kb3%
dim g_i%                       ' Auxiliary global integer variables
dim g_temp1,g_temp2,g_x,g_y    ' Auxiliary global float variables

' Timers
dim g_scroll_timer             ' Scroll timer
dim g_player_timer             ' Player timer
dim g_objects_timer            ' Objects timer
dim g_shot_timer               ' Shot timer

' Player state: x, y, animation counter, weapon, speed
dim g_player(4)
dim g_player_shot_ms=100
dim g_player_animation_ms=300

' Supports 12 shots at the same time (id, x, y, speed x, speed y).
' The three first slots are for the player shots, the other ones for enemies
' 0 - No shot
' 1 - Player Arrow
' 2 - Player Double arrow
' 3 - Player Fire
' 4 - Player Boomerang
' 5 - Player Sword
' 6 - Player Double arrow (level 2)
' 7 - Player Fire (level 2)
' 8 - Player Boomerang (level 2)
' 9 - Player Sword (level 2)
dim g_shots(12,4)
' Supports 10 objects at the same time (id, x, y, speed x, speed y, aux).
dim g_objects(10,5)
dim g_objects_tick%=0
const OBJ_INI_SPRITE_ID=bound(g_shots()) + 1 ' Initial sprite Id for object

init()
load_map(1)
initialize_screen_buffer()

run_stage(1)
page write 0: end

sub run_stage(stage%)
    page write 0: cls
    page write SCREEN_BUFFER

    'local dbg_max_time=0,dbg_start_time;
    if g_sound_on% then play modfile MUSIC$(stage%), 16000

    timer=0
    init_player()

    do
        ' Game tick
        if timer - g_game_tick < GAME_TICK_MS then continue do

        ' Process keyboard
        g_kb1%=KeyDown(1): g_kb2%=KeyDown(2): g_kb3%=KeyDown(3)
        if g_kb1% or g_kb2% or g_kb3% then process_kb()

        ' Scrolls the map
        if g_row% >= 0 and timer - g_scroll_timer >= SCROLL_SPEED_MS then
            scroll_map()
            g_scroll_timer=timer
        end if

        ' Process player animation
        if timer - g_player_timer >= g_player_animation_ms then
            g_player(2)=not g_player(2)
            sprite read #1, choice(g_player(2),PLAYER_SKIN1_X_R,PLAYER_SKIN1_X_L), PLAYER_SKIN_Y, TILE_SIZEx2, TILE_SIZEx2, TILES_BUFFER
            g_player_timer=timer
        end if

        ' Process objects animation
        if timer - g_objects_timer >= OBJECTS_ANIMATION_MS then
            animate_objects()
            g_objects_timer=timer
        end if

        ' Process game logic, the sprites and map rendering
        ' Map rendering
        page write 0
        blit 0,TILE_SIZE, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT+TILE_SIZE, SCREEN_BUFFER
        page write SCREEN_BUFFER

        ' Process sprites
        move_shots()
        move_objects()
        ' Move player
        sprite next #1,g_player(0),g_player(1)
        sprite move

        g_game_tick=timer
    loop
end sub

'
' Animate the game objects (enemies, power-ups)
sub animate_objects()
    local i%, obj_id%, offset%=TILE_SIZEx2
    inc g_objects_tick%
    for i%=0 to bound(g_objects())
        obj_id%=g_objects(i%,0)
        if not obj_id% then continue for
        if g_objects_tick% mod 2 then offset%=0
        sprite read OBJ_INI_SPRITE_ID + i%, OBJ_TILE_X%(obj_id%)+offset%, OBJ_TILE_Y%, TILE_SIZEx2, TILE_SIZEx2, TILES_BUFFER
    next
end sub

'
' Move screen shots
sub move_shots()
    local i%
    for i%=0 to bound(g_shots())
        ' Checks weapon id
        if not g_shots(i%,0) then continue for
        ' Moves shot
        inc g_shots(i%,1),g_shots(i%,3)
        inc g_shots(i%,2),g_shots(i%,4)
        if g_shots(i%,2) < 0 then
            g_shots(i%,0)=0
            sprite hide safe i%+2
        else
            sprite next i%+2,g_shots(i%,1),g_shots(i%,2)
        end if
    next
end sub

'
' Move screen objects
sub move_objects()
    local i%

    for i%=0 to bound(g_objects())
        ' Checks object id
        if not g_objects(i%,0) then continue for
        ' Moves object
        select case g_objects(i%,0)
            case 2,3 ' Bat and Bat wave
                g_objects(i%,1)=(SCREEN_CENTER-TILE_SIZE)+(SCREEN_CENTER-TILE_SIZEx4)*cos(g_objects(i%,5))
                inc g_objects(i%,5),g_objects(i%,3)
                inc g_objects(i%,2),g_objects(i%,4)
            case else
                inc g_objects(i%,1),g_objects(i%,3)
                inc g_objects(i%,2),g_objects(i%,4)
        end select

        if g_objects(i%,2) < 0 or g_objects(i%,2) > SCREEN_HEIGHT-TILE_SIZE then
            sprite hide OBJ_INI_SPRITE_ID + i%
            g_objects(i%,0)=0
        else
            sprite next OBJ_INI_SPRITE_ID + i%, g_objects(i%,1), g_objects(i%,2)
        end if
    next
end sub

'
' Create the shot sprite
sub fire()
    ' Cooldown
    debug_print("Shot!!! " + str$(timer) + ", " + str$(g_shot_timer));
    if timer - g_shot_timer < g_player_shot_ms then exit sub

    local i%
    for i%=0 to choice(g_player(3)>5,2,1)
        ' Ignore used slots
        if g_shots(i%,0) then continue for

        ' Create the shot state
        g_shots(i%,0)=g_player(3)             ' weapon
        g_shots(i%,1)=g_player(0)+TILE_SIZE/2 ' X
        g_shots(i%,2)=g_player(1)-TILE_SIZEx2 ' Y
        g_shots(i%,3)=0                       ' Speed X
        g_shots(i%,4)=-2.2                    ' Speed Y
        ' Create the shot sprite
        sprite read i%+2, WEAPON_ARROW_X, WEAPON_Y, TILE_SIZE, TILE_SIZEx2, TILES_BUFFER
        sprite show safe i%+2, g_shots(i%,1),g_shots(i%,2), 1
        ' Play SFX
        if g_sound_on% then play effect SHOT_EFFECT
        ' Increment timer
        g_shot_timer=timer
        exit for
    next
end sub

'
' Create a new object initialize its state and sprite
sub create_object(obj_id%, x%, y%)
    local i%
    for i%=0 to bound(g_objects())
        ' Check for empty slots
        if g_objects(i%,0) then continue for

        g_objects(i%,0)=obj_id%             ' Object Id
        g_objects(i%,1)=x%                  ' X
        g_objects(i%,2)=y%                  ' Y
        g_objects(i%,3)=OBJ_DATA(obj_id%,0) ' Speed X
        g_objects(i%,4)=OBJ_DATA(obj_id%,1) ' Speed Y
        g_objects(i%,5)=OBJ_DATA(obj_id%,2) ' Auxiliary

        select case obj_id%
            case 3 ' Bat wave
                if x% < SCREEN_CENTER then
                    g_objects(i%,5)=180
                end if
        end select

        sprite read OBJ_INI_SPRITE_ID + i%, OBJ_TILE_X%(obj_id%), OBJ_TILE_Y%, TILE_SIZEx2, TILE_SIZEx2, TILES_BUFFER
        sprite show safe OBJ_INI_SPRITE_ID + i%, g_objects(i%,1),g_objects(i%,2), 0
        exit for
    next
end sub

'
' Initialize player state and sprite
sub init_player()
    g_player(0)=SCREEN_WIDTH/2-TILE_SIZE  ' x
    g_player(1)=SCREEN_HEIGHT-TILE_SIZE*4 ' y
    g_player(2)=0                         ' animation counter
    g_player(3)=1                         ' weapon
    g_player(4)=0.6                       ' speed

    sprite read #1, PLAYER_SKIN1_X_L,PLAYER_SKIN_Y, TILE_SIZEx2,TILE_SIZEx2, TILES_BUFFER
    sprite show safe #1, g_player(0),g_player(1),1
end sub

'
' Check the player collision after map scroll
' Moves the player down in case of collision
sub check_scroll_collision()
    if map_colide(g_player()) then
        inc g_player(1),2
        'TODO: Implement player death by crush
    end if
end sub

'
' Scrolls the map
sub scroll_map()
    'sprite scroll 0,-1
    sprite scrollr 0,0,SCREEN_WIDTH,SCREEN_HEIGHT+TILE_SIZE,0,-1
    check_scroll_collision()
    inc g_tile_px%,1
    ' Draw the next map tile row
    if g_tile_px% = TILE_SIZE then
        g_tile_px%=0
        inc g_row%,-1
        ' Draw the next map row on the top of the screen buffer
        if g_row% >= 0 then
            draw_map_row_and_create_objects(g_row%)
        end if
    end if
end sub

'
' Process keyboard keys
sub process_kb()
    local x=g_player(0)
    local y=g_player(1)

    if g_kb1%=KB_LEFT or g_kb2%=KB_LEFT or g_kb3%=KB_LEFT then
        inc g_player(0), -g_player(4)
        if map_colide(g_player()) then g_player(0)=x
        if g_player(0) < 0 then g_player(0)=SCREEN_WIDTH - TILE_SIZE * 2
    else if g_kb1%=KB_RIGHT or g_kb2%=KB_RIGHT or g_kb3%=KB_RIGHT then
        inc g_player(0), g_player(4)
        if map_colide(g_player()) then g_player(0)=x
        if g_player(0) > SCREEN_WIDTH - TILE_SIZE * 2 then g_player(0)=0
    end if

    if g_kb1%=KB_UP or g_kb2%=KB_UP or g_kb3%=KB_UP then
        inc g_player(1), -g_player(4)
        if map_colide(g_player()) then g_player(1)=y
        if g_player(1) < TILE_SIZE * 4 then g_player(1)=TILE_SIZE * 4
    else if g_kb1%=KB_DOWN or g_kb2%=KB_DOWN or g_kb3%=KB_DOWN then
        inc g_player(1), g_player(4)
        if map_colide(g_player()) then g_player(1)=y
        if g_player(1) > SCREEN_HEIGHT - TILE_SIZE then g_player(1)=SCREEN_HEIGHT - TILE_SIZE
    end if

    if g_kb1%=KB_SPACE or g_kb2%=KB_SPACE or g_kb3%=KB_SPACE then
        fire()
    end if

    check_scroll_collision()
end sub

'
' Checks the player collision against solid tiles
function map_colide(player()) as integer
    local col%=player(0)\TILE_SIZE
    local row%=(player(1)-g_tile_px%)\TILE_SIZE+g_row%
    ' Check top left
    map_colide=g_map((row%+1)*MAP_COLS+col%) and &H100
    ' Check top right
    if not map_colide then map_colide=g_map((row%+1)*MAP_COLS+(col%+2))>>8 and 1
    ' Check bottom left
    if not map_colide then map_colide=g_map((row%+2)*MAP_COLS+col%)>>8 and 1
    ' Check bottom right
    if not map_colide then map_colide=g_map((row%+2)*MAP_COLS+(col%+2))>>8 and 1
    ' TODO: Check horizontal wrapping collision
end function

'
' Initialize the game
sub init()
    ' screen mode
    mode 7,12 ' 320x240
    ' init screen and tiles buffers
    page write SCREEN_BUFFER:cls
    page write TILES_BUFFER:cls
    load png TILESET_IMG
    ' clear screen
    page write 0: cls
    ' init game state variables
    g_row%=MAP_ROWS-SCREEN_ROWS-1
    g_tile_px%=0
end sub

'
' Draw map row to the top of the screen buffer
' and create objects sprites
sub draw_map_row_and_create_objects(row%)
    local tile%,col%,tx%,ty%,obj_id%,tile_data%

    page write SCREEN_BUFFER
    for col%=MAP_COLS_0 to 0 step -1
        ' Tile drawing
        tile_data%=g_map(row%*MAP_COLS+col%)
        tile%=(tile_data% AND &HFF) - 1
        tx%=(tile% mod TILES_COLS) * TILE_SIZE
        ty%=(tile% \ TILES_COLS) * TILE_SIZE
        blit tx%,ty%, col%*TILE_SIZE,0, TILE_SIZE,TILE_SIZE, TILES_BUFFER
        ' Create object
        obj_id%=(tile_data% AND &HE00) >> 9
        if obj_id% then create_object(obj_id%, col%*TILE_SIZE, 0)
    next
    page write 0
end sub

'
' Draw initial screen to the screen buffer
sub initialize_screen_buffer()
    local row%

    page write SCREEN_BUFFER
    for row%=MAP_ROWS_0 to MAP_ROWS_0 - (SCREEN_ROWS + 0) step -1
        draw_map_row_and_create_objects(row%)
        page scroll SCREEN_BUFFER,0,-TILE_SIZE
    next
    page scroll SCREEN_BUFFER,0,TILE_SIZE
end sub

'
' Load the map to the map global array g_map
sub load_map(num%)
    local i%=0
    local file_name$ = MAPS_DIR + "stage" + str$(num%) + ".map"
    open file_name$ for input as #1
    do while not eof(1)
        g_map(i%)=(asc(input$(1, #1)) << 8) OR asc(input$(1, #1))
        inc i%
    loop
    close #1
end sub

'
' Show the game intro
sub intro()
    local kb%
    do while kb% <> KB_SPACE
        kb%=KeyDown(1)
        if kb% = KB_ESC then mode 1:end
    loop
end sub

sub debug_print(msg$)
    if g_debug% then
        page write 0
        print @(0,184) msg$
        page write SCREEN_BUFFER
    end if
end sub
