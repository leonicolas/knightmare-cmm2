option explicit
option base 0
option angle degrees

#include "constants.inc"

' Global variables
dim g_map(MAP_SIZE) as integer ' Map data
dim g_row%                     ' Current top map row. Zero is the bottom row.
dim g_scroll_timer             ' Scroll timer
dim g_tilepx%                  ' The rendered tile row pixel
dim g_x%,g_y%,g_i%             ' Auxiliary global variables

init()
load_map(1)
initialize_screen_buffer()
page write 0

timer=0
do
    if timer >= g_scroll_timer then
        'page copy SCREEN_BUFFER to 0
        blit 0,TILE_SIZE, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SCREEN_BUFFER
        page scroll SCREEN_BUFFER,0,-1
        inc g_tilepx%,1
        inc g_scroll_timer!,SCROLL_SPEED
    end if

    if g_tilepx% = TILE_SIZE then
        inc g_row%,-1
        if g_row% < 0 then exit do
        draw_map_row(g_row%)
        g_tilepx%=0
    end if

    'inc g_row%,1
    'print@(0,200) "r: "+str$(g_row%)+" p: "+str$(g_tilepx%)+" T: "+str$(timer)+" ST: "+str$(g_scroll_timer!)
loop

page write 0: end

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
    g_tilepx%=0
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
    local kb
    do while kb <> KB_SPACE
        kb=KeyDown(1)
        if kb = KB_ESC then mode 1:end
    loop
end sub
