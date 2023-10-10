option explicit
option base 0
option angle degrees

#include "constants.inc"

dim stage(MAP_SIZE) as integer
dim gx%,gy%
init()
load_stage(1)

'do
    'intro()

'loop

sub init()
    cls
    mode 7,12 ' 320x240
    page write TILES_BUFFER
    load png "km_tileset.png", 32
    page write 0
end sub

' Loads the stage to the stage global array
sub load_stage(num%)
    local i%=0
    local file_name$ = "stage" + str$(num%) + ".map"
    open file_name$ for input as #1
    do while not eof(1)
        stage(i%)=(asc(input$(1, #1)) << 8) OR asc(input$(1, #1))
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