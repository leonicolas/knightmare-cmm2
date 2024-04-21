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
#include "intro.inc"
#include "map.inc"
#include "music.inc"
#include "objects.inc"
#include "power_ups.inc"
#include "print.inc"
#include "queue.inc"
#include "timer.inc"

init_game()
start_game()

sub start_game()
    local first_stage%=true
    init_player(2)
    g_stage%=1
    g_row%=MAP_ROWS_0
    do
        timer=0
        play_song("SILENCE_MOD")
        show_stage_screen(g_stage%)
        select case g_player(8)
            case 5 ' Start stage
                play_sfx("START_STAGE")
                do while g_player(8) = 5
                    if timer > choice(first_stage%,6000,4000) then g_player(8)=0
                loop
            case 6 ' Start after die
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

page write 1: cls 0
page write 0: cls 0
end

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

    select case g_player(8)
        case 4 ' Dead
    end select
end sub

sub process_kb()
    if g_player(8) then exit sub

    local kb1%=KeyDown(1), kb2%=KeyDown(2), kb3%=KeyDown(3)

    g_player_is_moving=false

    if not kb1% and not kb2% and not kb3% then
        g_kb_released%=true
        exit sub
    end if

    if g_kb_released% and (kb1%=KB_SPACE or kb2%=KB_SPACE or kb3%=KB_SPACE) then
        fire()
        g_kb_released%=false
    else if kb1%<>KB_SPACE and kb2%<>KB_SPACE and kb3%<>KB_SPACE then
        g_kb_released%=true
    end if

    if kb1%=KB_LEFT or kb2%=KB_LEFT or kb3%=KB_LEFT then
        move_player(KB_LEFT)
    else if kb1%=KB_RIGHT or kb2%=KB_RIGHT or kb3%=KB_RIGHT then
        move_player(KB_RIGHT)
    end if

    if kb1%=KB_UP or kb2%=KB_UP or kb3%=KB_UP then
        move_player(KB_UP)
    else if kb1%=KB_DOWN or kb2%=KB_DOWN or kb3%=KB_DOWN then
        move_player(KB_DOWN)
    end if
end sub

sub process_gamepad()
    if g_player(8) or not g_gamepad% then exit sub

    local dig_pos=gamepad(LY)
    if g_gamepad% and 128 or (dig_pos >= 0 and dig_pos < 124) then move_player(KB_UP)
    if g_gamepad% and 32  or dig_pos > 130 then move_player(KB_DOWN)

    dig_pos=gamepad(LX)
    if g_gamepad% and 256 or (dig_pos >= 0 and dig_pos < 124) then move_player(KB_LEFT)
    if g_gamepad% and 64  or dig_pos > 130 then move_player(KB_RIGHT)

    if g_gamepad% and 8192 then
        if g_gp_released% then fire()
        g_gp_released%=false
    else if not (g_gamepad% and 8192) then
        g_gp_released%=true
    end if
end sub

sub handle_gamepad()
    g_gamepad%=gamepad(B)
end sub
