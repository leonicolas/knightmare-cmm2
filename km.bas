option explicit
option base 0
option angle degrees

#include "constants.inc"

dim g_stage(MAP_SIZE) as integer
dim g_x%,g_y%,g_i%

init()
load_stage(1)
draw_screen_to_buffer()
page write 0

for g_i%=0 to 7
    blit 0,0, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SCREEN_BUFFER
    page scroll SCREEN_BUFFER,0,-1
    do while timer<500: loop
    timer=0
next

page write 0
end

sub draw_screen_to_buffer()
    local tile%,tx%,ty%,row%=SCREEN_ROWS * TILE_SIZE

    page write SCREEN_BUFFER
    for g_y%=MAP_ROWS_0 to MAP_ROWS_0 - SCREEN_ROWS step -1
        for g_x%=MAP_COLS_0 to 0 step -1
            tile%=(g_stage(g_y%*MAP_COLS+g_x%) AND &HFF)-1
            tx%=(tile% MOD TILES_COLS) * TILE_SIZE
            ty%=(tile% \ TILES_COLS) * TILE_SIZE
            blit tx%,ty%, g_x%*TILE_SIZE,row%, TILE_SIZE,TILE_SIZE, TILES_BUFFER
        next
        inc row%, -TILE_SIZE
    next
end sub

'do
    'intro()

'loop

sub init()
    cls
    mode 7,12 ' 320x240
    page write SCREEN_BUFFER:cls
    page write TILES_BUFFER:cls
    load png "km_tileset.png"
    page write 0
end sub

' Loads the stage to the stage global array
sub load_stage(num%)
    local i%=0
    local file_name$ = "stage" + str$(num%) + ".map"
    open file_name$ for input as #1
    do while not eof(1)
        g_stage(i%)=(asc(input$(1, #1)) << 8) OR asc(input$(1, #1))
        inc i%
    loop
    close #1
end sub

sub intro()
    local kb
    do while kb <> KB_SPACE
        kb=KeyDown(1)
        if kb = KB_ESC then mode 1:end
    loop
end sub
