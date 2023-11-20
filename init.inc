' Game and Stage initialization routines

'
' Initialize the game
sub init()
    ' screen mode
    font 1,1
    mode 3,12 ' 320x200
    'mode 7,12 ' 320x240
    'mode 4,12 ' debug mode

    ' init screen and tiles buffers
    page write SCREEN_BUFFER:cls

    page write MAP_TILES_BUFFER:cls
    load png MAP_TILESET_IMG

    page write OBJ_TILES_BUFFER:cls
    load png OBJ_TILESET_IMG

    ' clear screen
    page write 0: cls
    ' init game state variables
    g_tile_px%=0
end sub

'
' Initialize the given stage
sub init_stage(stage%)
    g_kb_released%=true
    load_map(stage%)
    init_map_tiles(stage%)
    initialize_screen_buffer()

    sprite interrupt check_collision

    page write 0: cls
    page write SCREEN_BUFFER
    init_player()

    ' Print the HUD
    print_text(SCREEN_ROWS  , 3, "SCORE  HISCORE  REST  STAGE")
    print_text(SCREEN_ROWS+1, 3, "   00       00    00     00")
    print_stage(stage%)
    print_lives(g_lives%)
    print_hiscore(g_hiscore%)
    print_score(g_score%)

    ' Play stage music
    if g_sound_on% then play modfile MUSIC$(stage%), 16000

    timer=0
end sub

'
' Load the map to the map global array g_map
sub load_map(stage%)
    local i%=0, value%, solid_bit%
    local file_name$ = MAPS_DIR + "stage" + str$(stage%) + ".map"
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

'
' Update the map tiles position moving the current stage tiles
' to the 0,0 buffer position. Each stage has 3 tiles rows.
sub init_map_tiles(stage%)
    page scroll MAP_TILES_BUFFER, 0, (stage% - 1) * 3 * TILE_SIZE
end sub

'
' Draw initial screen to the screen buffer
sub initialize_screen_buffer()
    local row%

    page write SCREEN_BUFFER
    for row%=MAP_ROWS_0 to MAP_ROWS_0 - (SCREEN_ROWS + 1) step -1
        process_map_row(row%)
        page scroll SCREEN_BUFFER,0,-TILE_SIZE
    next
    g_row%=row%+1
    page scroll SCREEN_BUFFER,0,TILE_SIZE
end sub

'
' Initialize player state and sprite
sub init_player()
    g_player(0)=SCREEN_WIDTH/2-TILE_SIZE  ' x
    g_player(1)=SCREEN_HEIGHT-TILE_SIZE*3 ' y
    g_player(2)=0                         ' animation counter
    g_player(3)=1                         ' weapon
    g_player(4)=0.6                       ' speed

    sprite read #1, PLAYER_SKIN1_X_L,PLAYER_SKIN_Y, TILE_SIZEx2,TILE_SIZEx2, OBJ_TILES_BUFFER
    sprite show safe #1, g_player(0),g_player(1), 1,,1
end sub
