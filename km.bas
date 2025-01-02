option explicit
option base 0
option angle degrees
option default float

#include "constants.inc"
#include "global.inc"
#include "screen.inc"
#include "boss.inc"
#include "collision.inc"
#include "init.inc"
#include "input.inc"
#include "map.inc"
#include "music.inc"
#include "objects.inc"
#include "power_ups.inc"
#include "print.inc"
#include "queue.inc"
#include "timer.inc"

init_game()
show_intro()
show_menu()

sub start_game()
    local first_stage%=true
    init_player(2)
    g_row%=MAP_ROWS_0

    ' Dev variables
    'g_stage%=7     ' Stage
    'g_row%=0       ' Position into the map
    'g_player(3)=11 ' Best weapon
    'g_player(6)=30 ' Shield with support to 30 hits
    ' End Dev variables
    do
        timer=0
        show_stage_screen(g_stage%)
        select case g_player(8)
            case 5 ' Start stage
                if not first_stage% then play_song("STAGE_INTRO")
                'g_player(8)=0 ' Dev hack! Skip stage intro!
                do while g_player(8) = 5
                    if timer > 4000 then g_player(8)=0
                loop
            case 6 ' Player died
                if g_player(7) < 0 then
                    show_game_over_screen()
                    pause 2000
                    exit do
                end if
                calculate_start_row()
                do while g_player(8) = 6
                    if timer > 2000 then g_player(8)=0
                loop
        end select
        first_stage%=false
        run_stage()

        ' Next stage
        if g_player(8) = 5 then
            g_row%=MAP_ROWS_0
            inc g_stage%
        end if
    loop
end sub

sub run_stage()
    local on_top%
    init_stage()

    do
        ' Game tick
        if timer - g_prev_frame_timer < GAME_TICK_MS then continue do
        g_delta_time=(timer-g_prev_frame_timer)/1000
        g_prev_frame_timer=timer
        inc g_timer
        'debug_print("FPS: "+str$(1/g_delta_time))
        page write SPRITES_BUFFER

        ' Scrolls the map
        if g_freeze_timer < 0 and g_row% >= -1 and g_timer mod 16 = 0 then scroll_map()

        ' Process keyboard and game pad
        process_kb()
        process_gamepad()

        ' Auto move player
        if g_player(8) = 3 then auto_move_player_to_portal()

        ' Process animations
        if g_timer mod 6 = 0 then
            inc g_anim_tick%
            animate_player()
            if g_boss(0) > 1 then animate_boss()
            animate_shots()
            animate_objects()
            ' Clean state
            g_fire%=false
        end if

        ' Process enemies shots
        if g_timer mod 250 = 0 then
            if g_stage% > 1 or g_row% < 125 then enemies_fire()
        end if
        ' Process boss shots
        if g_boss(0) > 1 then boss_fire()

        ' Move sprites
        move_shots()
        move_and_process_objects()
        if g_boss(0) > 0 then move_boss()

        ' Spawn enqueued objects
        process_actions_queue()
        sprite move
        ' Move player and shield ensuring always on top
        on_top%=choice(g_player(8) = 3,0,1)
        sprite show safe 1, g_player(0), g_player(1),1,,on_top%
        if g_player(5) = 1 and g_player(6) > 0 then sprite show safe 2, g_player(0), g_player(1)-TILE_SIZE,1,,on_top%

        ' Map and sprites rendering
        page write 0
        blit 0,TILE_SIZEx2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SCREEN_BUFFER
        page write 1
        blit 0,TILE_SIZEx2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SPRITES_BUFFER
        page write SPRITES_BUFFER

        ' Power up timers
        if g_freeze_timer >= 0 then process_freeze_timer()
        if g_power_up_timer >= 0 then process_power_up_timer()

        ' Check player status
        if g_player(8) > 4 then exit do
    loop

    ' Close all sprites and free memory
    destroy_all()
end sub
