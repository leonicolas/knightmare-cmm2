' Game and Stage initialization routines
sub init_game()
    ' screen mode
    mode 3,12 ' 320x200 Original size
    'mode 7,12 ' 320x240
    'mode 4,12 ' debug mode

    ' init screen and buffers
    page write 0: cls 0
    page write 1: cls 0
    page write SCREEN_BUFFER: cls 0
    page write SPRITES_BUFFER: cls 0

    page write BOSS_TILES_BUFFER: cls 0
    load png "BOSS_TILESET_IMG"

    page write MAP_TILES_BUFFER: cls 0
    load png "MAP_TILESET_IMG"

    page write OBJ_TILES_BUFFER: cls 0
    load png "OBJ_TILESET_IMG"

    ' init stage state variables
    g_tile_px%=0
end sub

sub init_stage(stage%)
    page write 0: cls 0
    page write 1: cls 0

    g_boss(0)=0
    g_stage%=stage%
    g_kb_released%=true
    load_map(stage%)
    init_map_tiles(stage%)
    'draw_map(MAP_ROWS_0)
    draw_map(40)

    sprite interrupt check_collision

    page write SCREEN_BUFFER
    init_player()

    ' Print the status bar
    print_status()
    g_player(3)=6
    ' Play stage music
    if g_row% > 8 then play_song(MUSIC$(stage%))

    timer=0
end sub

sub load_map(stage%)
    local i%=0, value%, solid_bit%
    local file_name$ = "MAPS_DIRstage" + str$(stage%) + ".map"
    open file_name$ for input as #1
    do while not eof(1)
        value%=asc(input$(1, #1))
        solid_bit%=value% and &H80
        ' Recalculates the stage tiles position. 96 tiles per stage
        value%=(value% and &H7F) + (96 * (stage% - 1))
        g_map(i%)=((solid_bit% or value%) << 8) or asc(input$(1, #1))
        inc i%
    loop
    close #1
end sub

sub init_map_tiles(stage%)
    page scroll MAP_TILES_BUFFER, 0, (stage% - 1) * 3 * TILE_SIZE
end sub

sub init_player()
    g_player(0)=SCREEN_WIDTH/2-TILE_SIZE  ' x
    g_player(1)=SCREEN_HEIGHT-TILE_SIZE*3 ' y
    g_player(2)=0                         ' animation counter
    g_player(3)=1                         ' weapon
    g_player(4)=PLAYER_INIT_SPEED         ' speed
    g_player(5)=0                         ' power up
    g_player(6)=0                         ' shield
    page write SPRITES_BUFFER
    sprite read #1, PLAYER_SKIN1_X_L,PLAYER_SKIN_Y, TILE_SIZEx2,TILE_SIZEx2, OBJ_TILES_BUFFER
    sprite show safe #1, g_player(0),g_player(1), 1,,1
end sub
