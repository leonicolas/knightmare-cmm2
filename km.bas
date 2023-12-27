option explicit
option base 0
option angle degrees
option default float

#include "constants.inc"
#include "global.inc"
#include "intro.inc"
#include "print.inc"
#include "init.inc"
#include "map.inc"
#include "queue.inc"
#include "timer.inc"
#include "animation.inc"
#include "hit.inc"
#include "spawn.inc"
#include "destroy.inc"

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
            animate_shots()
            animate_objects()
            g_anim_timer=timer
        end if

        ' Process enemies shots
        if timer - g_eshot_timer >= ENEMIES_SHOTS_MS then
            if stage% > 1 or g_row% < 125 then enemies_fire()
            g_eshot_timer=timer
        end if

        ' Move sprites
        move_shots()
        move_and_process_objects()
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

sub check_collision()
    local i%
    if sprite(S) then
        process_collision(sprite(S))
    else
        for i% = 1 to sprite(C, 0)
            process_collision(sprite(C, 0, i%))
        next
    end if
end sub

sub process_collision(sprite_id%)
    local i%, collided_id%

    for i% = 1 to sprite(C, sprite_id%)
        collided_id%=sprite(C, sprite_id%, i%)
        if collided_id% > 63 then continue for

        ' Check player collision
        select case sprite_id%
            case 1 ' Player
                ' Player shot. Three first shots slots
                if collided_id% >= SHOTS_INI_SPRITE_ID and collided_id% < SHOTS_INI_SPRITE_ID + 3 then
                    ' Destroy boomerang
                    if g_player(3) = 4 or g_player(3) = 9 then destroy_shot(collided_id%)

                ' Player hits a block
                else if collided_id% >= BLOCK_INI_SPRITE_ID then
                    collect_block_bonus(collided_id%)

                ' Player hits an object
                else
                    player_hit_obj(collided_id%)
                end if
            case is <= SHOTS_NUM% ' Check shot hit
                ' Player shots. Three first shots slots
                if sprite_id% >= SHOTS_INI_SPRITE_ID and sprite_id% < SHOTS_INI_SPRITE_ID + 3 then
                    ' Hits a block
                    if collided_id% >= BLOCK_INI_SPRITE_ID then
                        if hit_block(collided_id%) then destroy_shot(sprite_id%)

                    ' Hits an objects
                    else if collided_id% >= OBJ_INI_SPRITE_ID then
                        hit_object(collided_id%)
                        if not is_super_weapon() or is_chrystal(collided_id%) then destroy_shot(sprite_id%)
                    end if

                ' Enemies shots
                else
                    ' Hits the shield
                    if collided_id% = 2 then
                        hit_shield()
                        destroy_shot(sprite_id%)
                    end if
                end if
        end select
    next
end sub

sub player_hit_obj(collided_id%)
    local obj_ix%=collided_id% - OBJ_INI_SPRITE_ID
    if obj_ix% < 0 then exit sub
    select case g_obj(obj_ix%, 0)
        case 29 ' Weapon chrystal
            change_weapon(obj_ix%)
            destroy_object(obj_ix%)
        case 30 ' Power-up chrystal
            power_up(obj_ix%)
            destroy_object(obj_ix%)
        case else
            ' If player has invincibility
            if g_player(5) = 4 then
                hit_object(collided_id%, true)
            ' If player is not invisible
            else if g_player(5) <> 2 then
                hit_player(collided_id%)
            end if
    end select
end sub

sub change_weapon(obj_ix%)
    if g_obj(obj_ix%, 0) <> 29 then exit sub

    select case g_obj(obj_ix%, 4)
        case 0 to 2 ' No weapon
            increment_score(1000)
            if g_sound_on% then play effect "POINTS_SFX"
        case else ' Weapon chrystal
            increment_score(200)
            if g_sound_on% then play effect "CHRYSTAL_SFX"
            local weapon% = g_obj(obj_ix%, 4) - 1
            if weapon% = 5 then increase_player_speed()
            g_player(3)=weapon% + choice(g_player(3)=weapon% or g_player(3)=weapon%+5, 5, 0)
    end select
end sub

sub power_up(obj_ix%)
    if g_obj(obj_ix%, 0) <> 30 then exit sub
    select case g_obj(obj_ix%, 4)
        case 0 to 2 ' Black pill
            increment_score(1000)
            if g_sound_on% then play effect "POINTS_SFX"
            exit sub
        case 3 ' Dark blue pill - speed
            increase_player_speed()
        case 4 ' Blue pill - shield
            enqueue_action(1, 32)
        case 5 ' White pill - invisibility
            g_player(5)=2
            g_power_up_timer=POWER_UP_TIME
        case 6 ' Red pill - invincibility
            g_player(5)=4
            g_power_up_timer=POWER_UP_TIME
    end select

    increment_score(200)
    if g_sound_on% then play effect "CHRYSTAL_SFX"
end sub

sub increase_player_speed()
    if g_player(4) < PLAYER_MAX_SPEED then inc g_player(4), PLAYER_SPEED_INC
end sub

function is_chrystal(obj_sprite_id%) as integer
    local obj_id%=g_obj(obj_sprite_id% - OBJ_INI_SPRITE_ID, 0)
    is_chrystal=(obj_id%=29 or obj_id%=30)
end function

sub increment_score(points%)
    local i%=g_score%\NEW_LIFE_POINTS
    inc g_score%, points%
    print_score(g_score%)

    if g_score%\NEW_LIFE_POINTS > i% then
        update_life(1)
    end if

    if g_score% > g_hiscore% then
        g_hiscore% = g_score%
        print_hiscore(g_hiscore%)
    end if
end sub

sub update_life(value%)
    if value% >= 0 then
        play effect "NEW_LIFE_SFX"
    else
        play effect "PLAYER_DEATH_SFX"
    end if
    inc g_player(7), choice(value%, value%, 1)
    print_lives(g_player(7))
end sub

sub fire()
    ' Cooldown or invincibility power-up
    if timer - g_pshot_timer < g_player_shot_ms or g_player(5) = 4 then exit sub

    local i%, sprite_id%, shots%=choice(g_player(3)>6,2,1)
    local tile_id%=40
    local speed_x=0, speed_y=-220, offset%
    local sfx$="SHOT_SFX"
    local x=g_player(0)+(TILE_SIZE-OBJ_TILE%(tile_id%,2)/2)
    local y=g_player(1)-OBJ_TILE%(tile_id%,3)-1

    select case g_player(3)
        case 2 ' Twin arrows
            tile_id%=41
        case 3,8 ' Triple flames
            if g_shots(0,0) or g_shots(1,0) or g_shots(2,0) then exit sub
            shots%=2
            inc x, -TILE_SIZEx2
            speed_y=-200
            tile_id%=42
            sfx$="FLAME_SFX"
        case 4,9 ' Boomerang
            tile_id%=43
            sfx$=""
            g_cont_sfx$="BOOMERANG_SFX"
        case 5,10 ' Sword
            shots%=2
            tile_id%=choice(g_player(3) = 5, 45, 46)
            sfx$="SWORD_SFX"
        case 6,11 ' Fire arrow
            tile_id%=47
            sfx$="FIRE_ARROW_SFX"
    end select

    ' One, two or three shots depending on the weapon level
    for i%=0 to shots%
        offset%=0
        ' Ignore used slots
        if g_shots(i%,0) then continue for

        select case g_player(3)
            case 3, 8 ' Flames
                if i% > 0 then inc x, TILE_SIZEx2
                if i% = 1 then offset%=TILE_SIZE
                if g_player(3) = 3 then speed_x=50*i%-50
        end select

        ' Create the shot state
        g_shots(i%,0)=g_player(3) ' weapon
        g_shots(i%,1)=x           ' X
        g_shots(i%,2)=y-offset%   ' Y
        g_shots(i%,3)=speed_x     ' Speed X
        g_shots(i%,4)=speed_y     ' Speed Y

        ' Create the shot sprite
        sprite_id%=SHOTS_INI_SPRITE_ID+i%
        sprite read sprite_id%, OBJ_TILE%(tile_id%,0), OBJ_TILE%(tile_id%,1), OBJ_TILE%(tile_id%,2), OBJ_TILE%(tile_id%,3), OBJ_TILES_BUFFER
        sprite show safe sprite_id%, g_shots(i%,1),g_shots(i%,2), 1
        ' Play SFX
        if g_sound_on% and sfx$ <> "" then play effect sfx$

        ' Increment timer
        g_pshot_timer=timer
        if g_player(3) <> 3 and g_player(3) <> 8 then exit for
    next
end sub

function is_super_weapon() as integer
    select case g_player(3)
        case 3,4,6,8,9,11
            is_super_weapon = true
    end select
end function

sub enemies_fire()
    if g_freeze_timer >= 0 then exit sub

    local i%,c%,slot_ix%
    ' Three slots are reserved for player shots
    local free_slots%(bound(g_shots())-3)
    get_free_shot_slots(free_slots%())

    for i%=0 to bound(g_obj())
        ' Has object or it is in the min Y position?
        if g_obj(i%,0) = 0 or g_obj(i%,0) > 16 or g_obj(i%,2) < TILE_SIZEx2 then
            continue for
        end if

        slot_ix%=free_slots%(c%)
        ' Has free slot?
        if slot_ix% = 0 then exit for

        enemy_fire(i%, slot_ix%)
        inc c%
    next
end sub

sub enemy_fire(enemy_ix%, shot_ix%)
    if g_obj(enemy_ix%,2) > MAX_ENEMIES_SHOOT_Y or rnd > ENEMIES_SHOOT_CHANCE then exit sub
    local enemy_spr_id%=OBJ_INI_SPRITE_ID + enemy_ix%
    local shot_spr_id%=shot_ix%+SHOTS_INI_SPRITE_ID
    local cx=sprite(W, enemy_spr_id%)/2, cy=sprite(H, enemy_spr_id%)/2
    local angle=deg(sprite(V, enemy_spr_id%, 1))
    local idx%=22,rot%,speed

    select case g_obj(enemy_ix%,0)
        case 1   ' Blob
            speed=BLOB_SHOT_SPEED
        case 2,3 ' Bat
            speed=BAT_SHOT_SPEED
        case 4   ' Knight
            speed=KNIGHT_SHOT_SPEED
            rot%=fix((angle+22.5)/45)
            if rot%=0 or rot%=4 then
                idx%=23: rot%=choice(rot%=0,2,0)
            else if rot%=2 or rot%=6 then
                idx%=24: rot%=choice(rot%=2,1,0)
            else if rot%=3 or rot%=5 then
                idx%=25: rot%=choice(rot%=3,1,0)
            else
                idx%=25: rot%=choice(rot%=1,3,2)
            end if
    end select
    ' Create the shot state
    g_shots(shot_ix%,0)=1                     ' weapon
    g_shots(shot_ix%,1)=g_obj(enemy_ix%,1)+cx ' X
    g_shots(shot_ix%,2)=g_obj(enemy_ix%,2)+cy ' Y
    g_shots(shot_ix%,3)=speed*sin(angle)      ' Speed X
    g_shots(shot_ix%,4)=speed*-cos(angle)     ' Speed Y
    ' Create the shot sprite
    sprite read shot_spr_id%, OBJ_TILE%(idx%,0), OBJ_TILE%(idx%,1), OBJ_TILE%(idx%,2), OBJ_TILE%(idx%,3), OBJ_TILES_BUFFER
    sprite show safe shot_spr_id%, g_shots(shot_ix%,1),g_shots(shot_ix%,2), 1, rot%
end sub

sub process_kb()
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
