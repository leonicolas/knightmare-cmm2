option explicit
option base 0
option angle degrees
option default float

#include "constants.inc"

' Global variables
dim g_music_on%=1
dim g_debug%=0
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
dim g_shot_timer               ' Shot timer

' 0-X, 1-Y, 2-Animation Flag, 3-Weapon
dim g_player(3)
dim g_player_speed=0.6
dim g_player_shot_ms=100
dim g_player_animation_ms=300

' Supports 16 shots at the same time (id, x, y, speed x, speed y).
' The three first slots are for the player shots, the other ones for enemies
' 0 - No shot
' 1 - Player Arrow
' 2 - Player Double arrow
' 3 - Player Fire
' 4 - Player Boomerang
' 5 - Player Sord
' 6 - Player Double arrow (level 2)
' 7 - Player Fire (level 2)
' 8 - Player Boomerang (level 2)
' 9 - Player Sord (level 2)
dim g_shots(15,4)

init()
load_map(1)
initialize_screen_buffer()

page write 2
run_stage(1)
page write 0: end

sub run_stage(stage%)
    'local dbg_max_time=0,dbg_start_time;
    if g_music_on% then play modfile MUSIC$(stage%), 16000

    timer=0

    init_player()

    do
        'dbg_start_time=timer

        ' Shot timer
        if g_shot_timer > 0 and timer >= g_shot_timer then g_shot_timer=0

        ' Scrolls the map
        if g_scroll_timer >= 0 and timer >= g_scroll_timer then
            'sprite scroll 0,-1
            sprite scrollr 0,0,SCREEN_WIDTH,SCREEN_HEIGHT+TILE_SIZE,0,-1
            check_scroll_collision()
            inc g_tile_px%,1
            g_scroll_timer=timer+SCROLL_SPEED
            ' Draw the next map tile row
            if g_tile_px% = TILE_SIZE then
                draw_next_map_row()
            end if
        end if

        ' Process player animation
        if timer >= g_player_timer then
            g_player(2)=not g_player(2)
            sprite read #1, choice(g_player(2),PLAYER_SKIN1_X_R,PLAYER_SKIN1_X_L), PLAYER_SKIN_Y, TILE_SIZEx2, TILE_SIZEx2, TILES_BUFFER
            g_player_timer=timer+g_player_animation_ms
        end if

        ' Process game logic, the sprites and map rendering
        if timer >= g_game_tick then
            ' Process keyboard
            g_kb1%=KeyDown(1): g_kb2%=KeyDown(2): g_kb3%=KeyDown(3)
            if g_kb1% or g_kb2% or g_kb3% then process_kb()

            ' Map rendering
            page write 0
            blit 0,TILE_SIZE, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT+TILE_SIZE, SCREEN_BUFFER
            page write SCREEN_BUFFER

            ' Process sprites
            process_shots()
            sprite next #1,g_player(0),g_player(1)
            sprite move

            ' Debug
            if g_debug% then
                page write 0
                'print@(0,200) "r: "+str$(dbg_max_time)+"  "
                page write SCREEN_BUFFER
            end if

            g_game_tick=timer+GAME_TICK
        end if
        'dbg_max_tim'e=max(dbg_max_time,timer-dbg_start_time)
    loop
end sub

sub process_shots()
    local i
    for i=0 to bound(g_shots())
        ' Checks weapon id
        if not g_shots(i,0) then continue for
        ' Moves shot
        inc g_shots(i,1),g_shots(i,3)
        inc g_shots(i,2),g_shots(i,4)
        if g_shots(i,2) < 0 then
            g_shots(i,0)=0
            sprite hide safe i+2
        else
            sprite next i+2,g_shots(i,1),g_shots(i,2)
        end if
    next
end sub

sub init_player()
    g_player(0)=SCREEN_WIDTH/2-TILE_SIZE
    g_player(1)=SCREEN_HEIGHT-TILE_SIZE*4
    g_player(2)=0
    g_player(3)=1

    sprite read #1, PLAYER_SKIN1_X_L,PLAYER_SKIN_Y, TILE_SIZEx2,TILE_SIZEx2, TILES_BUFFER
    sprite show safe #1, g_player(0),g_player(1),1
end sub

sub check_scroll_collision()
    if map_colide(g_player()) then
        inc g_player(1),2
        'TODO: Implement player death by crush
    end if
end sub

sub draw_next_map_row()
    inc g_row%,-1
    if g_row% >= 0 then
        draw_map_row(g_row%)
        g_tile_px%=0
    else
        ' Stops the screen scroll
        g_scroll_timer=-1
    end if
end sub

sub process_kb()
    local x=g_player(0)
    local y=g_player(1)

    if g_kb1%=KB_LEFT or g_kb2%=KB_LEFT or g_kb3%=KB_LEFT then
        inc g_player(0), -g_player_speed
        if map_colide(g_player()) then g_player(0)=x
        if g_player(0) < 0 then g_player(0)=SCREEN_WIDTH - TILE_SIZE * 2
    else if g_kb1%=KB_RIGHT or g_kb2%=KB_RIGHT or g_kb3%=KB_RIGHT then
        inc g_player(0), g_player_speed
        if map_colide(g_player()) then g_player(0)=x
        if g_player(0) > SCREEN_WIDTH - TILE_SIZE * 2 then g_player(0)=0
    end if

    if g_kb1%=KB_UP or g_kb2%=KB_UP or g_kb3%=KB_UP then
        inc g_player(1), -g_player_speed
        if map_colide(g_player()) then g_player(1)=y
        if g_player(1) < TILE_SIZE * 4 then g_player(1)=TILE_SIZE * 4
    else if g_kb1%=KB_DOWN or g_kb2%=KB_DOWN or g_kb3%=KB_DOWN then
        inc g_player(1), g_player_speed
        if map_colide(g_player()) then g_player(1)=y
        if g_player(1) > SCREEN_HEIGHT - TILE_SIZE then g_player(1)=SCREEN_HEIGHT - TILE_SIZE
    end if

    if not g_shot_timer and (g_kb1%=KB_SPACE or g_kb2%=KB_SPACE or g_kb3%=KB_SPACE) then
        player_shoot()
    end if

    check_scroll_collision()
end sub

sub player_shoot()
    local i
    for i=0 to choice(g_player(3)>5,2,1)
        if not g_shots(i,0) then
            g_shots(i,0)=g_player(3)             ' weapon
            g_shots(i,1)=g_player(0)+TILE_SIZE/2 ' X
            g_shots(i,2)=g_player(1)-TILE_SIZEx2 ' Y
            g_shots(i,3)=0                       ' Speed X
            g_shots(i,4)=-2.2                    ' Speed Y
            sprite read i+2, WEAPON_ARROW_X, WEAPON_Y, TILE_SIZE, TILE_SIZEx2, TILES_BUFFER
            sprite show safe i+2, g_shots(i,1),g_shots(i,2), 1
            play effect SHOT_EFFECT
            g_shot_timer=timer+g_player_shot_ms
            exit for
        end if
    next
end sub

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
sub draw_map_row(row%)
    local tile%,col%,tx%,ty%

    page write SCREEN_BUFFER
    for col%=MAP_COLS_0 to 0 step -1
        tile%=(g_map(row%*MAP_COLS+col%) AND &HFF)-1
        tx%=(tile% MOD TILES_COLS) * TILE_SIZE
        ty%=(tile% \ TILES_COLS) * TILE_SIZE
        blit tx%,ty%, col%*TILE_SIZE,0, TILE_SIZE,TILE_SIZE, TILES_BUFFER
    next
    page write 0
end sub

'
' Draw initial screen to the screen buffer
sub initialize_screen_buffer()
    local row%

    page write SCREEN_BUFFER
    for row%=MAP_ROWS_0 to MAP_ROWS_0 - (SCREEN_ROWS + 0) step -1
        draw_map_row(row%)
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
