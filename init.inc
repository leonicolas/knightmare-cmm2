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

    ' Interrupts
    sprite interrupt check_collision
    gamepad on handle_gamepad

    ' init stage state variables
    g_tile_px%=0
end sub

sub init_stage()
    timer=0
    g_timer=0
    g_pshot_timer=0
    g_freeze_timer=-1
    g_power_up_timer=-1
    g_prev_frame_timer=0
    g_enemies_count%=0

    ' Initialize buffers and screen
    page write SPRITES_BUFFER: cls 0
    page write SCREEN_BUFFER: cls 0
    page write 0: cls 0
    page write 1: cls 0

    ' Initialize player
    page write SPRITES_BUFFER
    g_player(0)=SCREEN_WIDTH/2-TILE_SIZE  ' x
    g_player(1)=SCREEN_HEIGHT-TILE_SIZE*3 ' y
    g_player(2)=0                         ' animation counter
    sprite read #1, PLAYER_SKIN1_X_L,PLAYER_SKIN_Y, TILE_SIZEx2,TILE_SIZEx2, OBJ_TILES_BUFFER
    sprite show safe #1, g_player(0),g_player(1), 1,,1

    ' Initialize shield
    if g_player(6) > 0 then spawn_shield()

    ' Initialize boss and kb flag
    g_boss(0)=0
    g_kb_released%=true

    ' Load and draw the map
    load_map(g_stage%)
    'draw_map(0)
    draw_map(MAP_ROWS_0)

    ' Print the status bar
    print_status()

    ' Play stage music
    if g_row% > 8 then play_stage_song() else play_boss_song()
end sub

sub init_player(lives)
    g_player(2)=0                 ' animation counter
    g_player(3)=1                 ' weapon
    g_player(4)=PLAYER_INIT_SPEED ' speed
    g_player(5)=0                 ' power up
    g_player(6)=0                 ' shield
    g_player(7)=lives             ' Lives
    g_player(8)=5                 ' Status ready to next stage
end sub
