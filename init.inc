' Game and Stage initialization routines
sub init_game()
    ' screen mode
    mode 3,12 ' 320x200 Original size
    'mode 7,12 ' 320x240
    'mode 4,12 ' debug mode

    ' init screen and buffers
    clear_screen()
    page write SCREEN_BUFFER: cls 0
    page write SPRITES_BUFFER: cls 0

    page write BOSS_TILES_BUFFER: cls 0
    load png BOSS_TILESET_IMG

    page write MAP_TILES_BUFFER: cls 0
    load png MAP_TILESET_IMG

    page write OBJ_TILES_BUFFER: cls 0
    load png OBJ_TILESET_IMG

    ' Interrupts
    sprite interrupt check_collision
    gamepad on handle_gamepad
end sub

sub init_stage()
    local i%

    sprite close all
    reset_globals()

    ' Clear arrays
    for i%=0 to bound(g_blocks()): g_blocks(i%, 0) = 0: next
    for i%=0 to bound(g_boss()): g_boss(i%) = 0: next
    for i%=0 to bound(g_shots()): g_shots(i%, 0) = 0: next
    for i%=0 to bound(g_obj()): g_obj(i%, 0) = 0: next
    for i%=0 to bound(g_actions_queue()): g_actions_queue(i%, 0) = 0: next

    ' Initialize buffers and screen
    page write SPRITES_BUFFER: cls 0
    page write SCREEN_BUFFER: cls 0
    page write 0: cls 0
    page write 1: cls 0

    ' Initialize player
    page write SPRITES_BUFFER
    g_player(0)=PLAYER_INIT_COL*TILE_SIZE ' x
    g_player(1)=PLAYER_INIT_ROW*TILE_SIZE ' y
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
    draw_map(g_row%)

    ' Print the status bar
    print_status()

    ' Play stage music
    if g_row% > 8 then play_stage_song() else play_boss_song()
end sub

sub reset_globals()
    timer=0
    g_timer=0
    g_pshot_timer=0
    g_freeze_timer=-1
    g_power_up_timer=-1
    g_prev_frame_timer=0
    g_enemies_count%=0
    g_scroll_on=true
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
