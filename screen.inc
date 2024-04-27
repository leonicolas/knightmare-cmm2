sub show_stage_screen(stage%)
    page write 1: cls 0
    page write 0: cls 0
    print_status()
    print_centralized_stage(stage%, "STAGE ")
end sub

sub show_game_over_screen(stage%)
    page write 1: cls 0
    page write 0: cls 0
    print_centralized("GAME OVER")
end sub

sub clear_screen()
    page write 0: cls 0
    page write 1: cls 0
end sub

sub init_menu()
    show_intro()
    clear_screen()
    reset_globals()
    g_stage%=1
    print_centralized("KNIGHTMARE",-8)
    print_centralized("ORIGINAL GAME BY KONAMI 1986",9)
    print_centralized("MMBASIC VERSION BY",11)
    print_centralized("LEONARDO BERARDINO 2024",12)
    print_centralized("  START GAME        ",-1)
    print_centralized("  QUIT              ", 5)
end sub

sub show_menu()
    local released%=true, kb%, item%=1, i%
    local diff$(2)=("EASY  ","NORMAL","HARD  ")
    init_menu()
    do
        kb%=KeyDown(1)
        if released% and kb% = KB_UP then
            item%=max(1,item%-1)
            released%=false

        else if released% and kb% = KB_DOWN then
            item%=min(4,item%+1)
            released%=false

        else if released% and kb% = KB_RIGHT then
            select case item%
                case 2
                    g_difficulty%=min(2,g_difficulty%+1)
                case 3
                    g_stage%=min(8,g_stage%+1)
            end select
            released%=false

        else if released% and kb% = KB_LEFT then
            select case item%
                case 2
                    g_difficulty%=max(0,g_difficulty%-1)
                case 3
                    g_stage%=max(1,g_stage%-1)
            end select
            released%=false

        else if released% and kb% = KB_SPACE then
            select case item%
                case 1
                    start_game()
                    init_menu()
                case 4
                    clear_screen()
                    end
            end select
            released%=false

        else if not kb% then
            released%=true
        end if
        print_centralized("  DIFFICULTY: "+diff$(g_difficulty%), 1)
        print_centralized("  STAGE: "+str$(g_stage%)+space$(10), 3)

        page write 0
        blit 112,160, center_x(11),center_y(3,-6),TILE_SIZE*11,TILE_SIZE*3, OBJ_TILES_BUFFER
        sprite read 1, 112,184, TILE_SIZE,TILE_SIZE, OBJ_TILES_BUFFER
        sprite show safe 1, center_x(TILE_SIZE,-5.5), center_y(TILE_SIZE,item%*2), 1

        pause 50
    loop
end sub

sub show_intro()
    local width=14
    local y=mm.vres-1, x_center%=center_x(width), y_center%=center_y(5)
    clear_screen()
    reset_globals()
    page write 0: cls rgb(blue)
    sprite read 1, 0,160, TILE_SIZE*width,TILE_SIZE*3, OBJ_TILES_BUFFER
    do while y > y_center%
        inc y, 200*-g_delta_time
        g_delta_time=(timer-g_prev_frame_timer)/1000
        g_prev_frame_timer=timer
        sprite show safe 1, x_center%, y, 1
        pause 10
    loop
    pause 300
    sprite close 1
    sprite read 1, 0,160, TILE_SIZE*width,TILE_SIZE*5, OBJ_TILES_BUFFER
    sprite show safe 1, x_center%, y, 1
    pause 2000
    sprite close 1
end sub

function center_x(tile_cols, offset_x) as float
    center_x=SCREEN_COLS*TILE_SIZE/2-TILE_SIZE*tile_cols/2+offset_x*TILE_SIZE+SCREEN_OFFSET
end function

function center_y(tile_rows,offset_y) as float
    center_y=SCREEN_ROWS*TILE_SIZE/2-TILE_SIZE*tile_rows/2+offset_y*TILE_SIZE
end function
