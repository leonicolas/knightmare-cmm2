sub show_stage_screen(stage%)
    page write 1: cls 0
    page write 0: cls 0
    print_status()
    if stage% = 9 then
        print_centralized("YOU HAVE BEATEN ALL DEMONS !",2)
    else
        print_centralized_stage(stage%, "STAGE ")
    end if
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
    clear_screen()
    reset_globals()
    g_stage%=1
    print_centralized("KNIGHTMARE",-8)
    print_centralized("ORIGINAL GAME BY KONAMI(C) 1986",7)
    print_centralized("MMBASIC VERSION BY",9)
    print_centralized("LEONARDO BERARDINO 2025",10)
    print_centralized("MUSIC BY H0FFMAN",12)
    print_centralized("  START GAME        ",-1)
    print_centralized("  QUIT              ", 5)
end sub

sub show_menu()
    local released%=true, gp%, kb%, ctrl%, item%=1, i%
    init_menu()
    play_song("TITLE")
    do
        update_menu(item%)
        gp% = read_gamepad%()
        kb% = read_keyboard%()

        ctrl%=0
        if kb% then
            ctrl% = kb%
        else if gp% then
            ctrl% = gp%
        end if

        if not ctrl% then released% = true : continue do
        if not released% then continue do
        released% = false

        if ctrl% = CTRL_START then
          item% = 1
          ctrl% = CTRL_FIRE
        end if

        select case ctrl%
            case CTRL_UP
              item%=max(1,item%-1)

            case CTRL_DOWN
              item%=min(4,item%+1)

            case CTRL_RIGHT
                select case item%
                    case 2
                        g_difficulty%=min(2,g_difficulty%+1)
                    case 3
                        g_stage%=min(8,g_stage%+1)
                end select

            case CTRL_LEFT
                select case item%
                    case 2
                        g_difficulty%=max(0,g_difficulty%-1)
                    case 3
                        g_stage%=max(1,g_stage%-1)
                end select

            case CTRL_FIRE
                select case item%
                    case 1
                        play_song("STAGE_INTRO")
                        for i%=0 to 20
                            if i% mod 2 then
                                print_centralized("  START GAME        ",-1)
                            else
                                print_centralized("                    ",-1)
                            end if
                            pause 100
                        next
                        start_game()
                        init_menu()
                    case 4
                        clear_screen()
                        end_game()
                end select
        end select
    loop
end sub

sub update_menu(item%)
    local diff$(2)=("EASY  ","NORMAL","HARD  ")
    print_centralized("  DIFFICULTY: "+diff$(g_difficulty%), 1)
    print_centralized("  STAGE: "+str$(g_stage%)+space$(10), 3)

    page write 0
    blit 112,160, center_x(11),center_y(3,-6),TILE_SIZE*11,TILE_SIZE*3, OBJ_TILES_BUFFER
    sprite read 1, 112,184, TILE_SIZE,TILE_SIZE, OBJ_TILES_BUFFER
    sprite show safe 1, center_x(TILE_SIZE,-5.5), center_y(TILE_SIZE,item%*2), 1
end sub

sub show_intro()
    local width=11
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

sub play_final_cutscene()
end sub
