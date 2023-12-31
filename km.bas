option explicit
option base 0
option angle degrees
option default float

#include "constants.inc"
#include "global.inc"

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
g_player(7)=2 ' Lives
run_stage(1)
page write 1: cls 0
page write 0: cls 0: end

sub run_stage(stage%)
    init_stage(stage%)

    do
        ' Game tick
        if timer - g_prev_frame_timer < GAME_TICK_MS then continue do
        g_delta_time=(timer-g_prev_frame_timer)/1000
        g_prev_frame_timer=timer
        'debug_print("FPS: "+str$(1/g_delta_time))

        page write SPRITES_BUFFER

        ' Scrolls the map
        if g_freeze_timer < 0 and g_row% >= 0 and timer - g_scroll_timer >= SCROLL_SPEED_MS then
            scroll_map()
            g_scroll_timer=timer
        end if

        ' Process keyboard
        process_kb()

        ' Process animations
        if timer - g_anim_timer >= ANIM_TICK_MS then
            inc g_anim_tick%
            animate_player()
            if g_boss(0) > 1 then animate_boss()
            animate_shots()
            animate_objects()
            g_anim_timer=timer
        end if

        ' Process enemies shots
        if timer - g_eshot_timer >= ENEMIES_SHOTS_MS then
            if stage% > 1 or g_row% < 125 then enemies_fire()
            g_eshot_timer=timer
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
        sprite show safe 1, g_player(0), g_player(1),1,,1
        if g_player(6) > 0 then sprite show safe 2, g_player(0), g_player(1)-TILE_SIZE,1,,1

        ' Map and sprites rendering
        page write 0
        blit 0,TILE_SIZEx2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SCREEN_BUFFER
        page write 1
        blit 0,TILE_SIZEx2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SPRITES_BUFFER
        page write SPRITES_BUFFER

        if g_freeze_timer >= 0 then process_freeze_timer()
        if g_power_up_timer >= 0 then process_power_up_timer()
    loop

    ' Close all sprites
    sprite close all
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
