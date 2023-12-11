option explicit
option base 0
option angle degrees
option default float

#include "constants.inc"
#include "global.inc"
#include "intro.inc"
#include "print.inc"
#include "init.inc"

init_game()
run_stage(1)
page write 0: end

sub run_stage(stage%)
    init_stage(stage%)

    do
        ' Game tick
        if timer - g_prev_frame_timer < GAME_TICK_MS then continue do
        g_delta_time=(timer-g_prev_frame_timer)/1000
        g_prev_frame_timer=timer

        ' Scrolls the map
        if g_freeze_timer < 0 and g_row% >= 0 and timer - g_scroll_timer >= SCROLL_SPEED_MS then
            scroll_map()
            g_scroll_timer=timer
        end if

        ' Process player animation
        if timer - g_player_timer >= g_player_animation_ms then
            g_player(2)=not g_player(2)
            sprite read #1, choice(g_player(2),PLAYER_SKIN1_X_R,PLAYER_SKIN1_X_L), PLAYER_SKIN_Y, TILE_SIZEx2, TILE_SIZEx2, OBJ_TILES_BUFFER
            g_player_timer=timer
        end if

        ' Process objects animation
        if timer - g_obj_timer >= OBJECTS_ANIMATION_MS then
            animate_objects()
            g_obj_timer=timer
        end if

        ' Process enemies shots
        if timer - g_eshot_timer >= ENEMIES_SHOTS_MS then
            if stage% > 1 or g_row% < 125 then enemies_fire()
            g_eshot_timer=timer
        end if

        ' Map rendering
        page write 0
        blit 0,TILE_SIZEx2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SCREEN_BUFFER
        page write SCREEN_BUFFER

        ' Process keyboard
        process_kb()
        ' Move sprites
        move_shots()
        move_and_process_objects()
        ' Spawn enqueued objects
        process_actions_queue()
        ' Move player - ensure player always on top
        sprite show safe #1, g_player(0),g_player(1), 1,,1
        sprite move

        if g_freeze_timer >= 0 then process_freeze_timer()
        if g_power_up_timer >= 0 then process_power_up_timer()
    loop
    ' Close all sprites
    sprite close all
end sub

sub process_freeze_timer()
    local fraction%=fix((g_freeze_timer - fix(g_freeze_timer)) * 100)
    print_freeze_timer(g_freeze_timer)
    if fraction% = 0 or (g_freeze_timer < 4 and fraction% = 50) then play effect "FREEZE_TICK_SFX"

    inc g_freeze_timer, -1 * g_delta_time
    if g_freeze_timer < 0 then
        clear_freeze_timer()
    end if
end sub

sub process_power_up_timer()
    print_power_up_timer(g_power_up_timer)
    inc g_power_up_timer, -0.03
    if g_power_up_timer < 0 then
        clear_power_up_timer()
    end if
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
        if sprite_id% = 1 then
            ' Player hits a block
            if collided_id% >= BLOCK_INI_SPRITE_ID then
                collect_block_bonus(collided_id%)
            else
                hit_player(collided_id%)
            end if

        ' Check shot hit
        else if sprite_id% <= SHOTS_NUM% then
            ' Player shot hits a block
            if sprite_id% <= 4 and collided_id% >= BLOCK_INI_SPRITE_ID then
                if hit_block(collided_id%) then
                    destroy_shot(sprite_id%)
                end if

            ' Player shot hits an enemy
            else if sprite_id% <= 4 and collided_id% >= OBJ_INI_SPRITE_ID then
                hit_object(collided_id%)
                destroy_shot(sprite_id%)
            end if
        end if
    next
end sub

sub process_actions_queue()
    local i%, time_ms, obj_id%

    ' Process enqueued actions
    for i%=0 to bound(g_actions_queue())
        if g_actions_queue(i%, 0) = 0 then continue for
        if timer < g_actions_queue(i%, 5) then continue for

        obj_id%=g_actions_queue(i%, 1)
        ' Objects and enemies
        if obj_id% < 31 then
            select case obj_id%
                case 2
                    time_ms=BAT_SPAWN_SPEED_MS
                case 3
                    time_ms=BAT_WAVE_SPAWN_SPEED_MS
            end select
            spawn_object(obj_id%, g_actions_queue(i%, 2), g_actions_queue(i%, 3))

        ' Blocks
        else if obj_id% = 31 then
            replace_block(g_actions_queue(i%, 2))
        end if

        ' Decrement execution count
        inc g_actions_queue(i%, 0), -1
        g_actions_queue(i%, 5)=timer+time_ms
    next
end sub

sub replace_block(i%)
    local max_hits%=block_max_hits(i%)

    if g_blocks(i%,0) < 6 and g_blocks(i%,1) > 1 and g_blocks(i%,1) < max_hits% then
        end sub
    end if

    local x%, y%, l%, sprite_id%, offset%

    ' Calculate the block sprite index
    sprite_id%=BLOCK_INI_SPRITE_ID+i%
    ' Calculate the tile X offset
    offset%=choice(g_blocks(i%,1) < max_hits%, 0, TILE_SIZEx2*g_blocks(i%,0))
    ' Save sprite position and layer
    x%=sprite(X, sprite_id%)
    y%=sprite(Y, sprite_id%)
    l%=sprite(L, sprite_id%)
    ' Replace buffer tiles
    sprite hide safe sprite_id%
    blit OBJ_TILE%(31,0)+offset%,OBJ_TILE%(31,1), x%, y%, OBJ_TILE%(31,2), OBJ_TILE%(31,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, x%, y%, l%
    if g_blocks(i%, 0) = 6 then
        destroy_block(i%)
    end if
end sub

' TODO: implement!!!
sub hit_player(collided_id%)
end sub

sub destroy_all_shots()
    local i%

    for i%=3 to bound(g_shots())
        if g_shots(i%, 0) > 0 then destroy_shot(i% + 2)
    next
end sub

sub destroy_shot(sprite_id%)
    sprite hide safe sprite_id%
    sprite close sprite_id%
    g_shots(sprite_id% - 2, 0) = 0
end sub

sub destroy_object(obj_ix%)
    local sprite_id%=OBJ_INI_SPRITE_ID + obj_ix%
    g_obj(obj_ix%, 0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
end sub

sub destroy_block(block_ix%)
    local sprite_id%=BLOCK_INI_SPRITE_ID + block_ix%
    g_blocks(block_ix%,0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
end sub

function hit_block(sprite_id%) as integer
    local i%=sprite_id% - BLOCK_INI_SPRITE_ID
    local max_hits%=block_max_hits(i%)

    if g_blocks(i%,1) = max_hits% then exit function

    ' Increment hits
    inc g_blocks(i%,1), 1
    hit_block = true

    if g_sound_on% then
        if g_blocks(i%,1) < max_hits% then
            play effect "BLOCK_HIT_SFX"
        else if g_blocks(i%,1) = max_hits% then
            play effect "BLOCK_OPEN_SFX"
        end if
    end if

    if g_blocks(i%,1)=1 or g_blocks(i%,1)=max_hits% then
        ' Spawn new block tile
        enqueue_action(1, 31, i%)
    end if
end function

sub collect_block_bonus(sprite_id%)
    local i%=sprite_id% - BLOCK_INI_SPRITE_ID
    local max_hits%=block_max_hits(i%)

    if g_blocks(i%,1) < max_hits% or g_blocks(i%,0) = 6 then exit sub

    select case g_blocks(i%,0)
        case 1 ' Hook - points
            increment_score(BLOCK_POINTS)
            play effect "POINTS_SFX"
        case 2 ' Knight - kill all enemies in the screen
            kill_all_enemies()
        case 3 ' Queen - Extra life
            update_life(1)
        case 4 ' King - Freeze enemies
            g_freeze_timer=FREEZE_TIME
            destroy_all_shots()
        case 5 ' Barrier
            increment_score(BLOCK_POINTS)
            play effect "POINTS_SFX"
    end select

    g_blocks(i%,0)=6
    enqueue_action(1, 31, i%)
end sub

sub kill_all_enemies()
    local i%
    play effect "KILL_ALL_ENEMIES_SFX"
    for i%=0 to bound(g_obj())
        ' Check for free slots
        if g_obj(i%,0)=0 then continue for
        hit_object(OBJ_INI_SPRITE_ID + i%, true)
    next
end sub

function block_max_hits(i%) as integer
    if g_blocks(i%,0) = 6 then
        ' collected block doesn't have max hits
        block_max_hits = 0
    else
        ' Special blocks need twice of hits
        block_max_hits = choice(g_blocks(i%,0) > 1, BLOCK_HITS * 2, BLOCK_HITS)
    end if
end function

sub hit_object(obj_sprite_id%, instakill%)
    local i%

    i%=obj_sprite_id% - OBJ_INI_SPRITE_ID

    select case g_obj(i%,0)
        case 30 ' Power up
            inc g_obj(i%,4)
            g_obj(i%,4)=g_obj(i%,4) mod 7
            ' Enable the wobbling movement
            if g_obj(i%,4) > 3 and g_obj(i%,5) < 0 then g_obj(i%,5)=0

        case else ' Enemies
            ' Decrement life
            inc g_obj(i%,3), -1 ' TODO: Implement weapon force
            if not instakill% and g_obj(i%,3) > 0 then continue for

            ' Increment score
            increment_score(OBJ_POINTS(g_obj(i%,0)))

            ' Delete enemy's object
            destroy_object(i%)

            delete_shadow(i%)
            start_enemy_death_animation(i%)
            if g_sound_on% and not instakill% then play effect "ENEMY_DEATH_SFX"
    end select
end sub

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
    inc g_lives%, choice(value%, value%, 1)
    print_lives(g_lives%)
end sub

sub start_enemy_death_animation(i%)
    enqueue_action(1, 20, g_obj(i%,1), g_obj(i%,2))
end sub

sub delete_shadow(src_obj_ix%)
    local shadow_ix%=g_obj(src_obj_ix%,6)

    if shadow_ix% < 0 then exit sub

    g_obj(src_obj_ix%,6) = -1
    destroy_object(shadow_ix%)
end sub

sub animate_objects()
    local i%, obj_id%, sprite_ix%, offset%, flip%
    inc g_anim_tick%

    for i%=0 to bound(g_obj())
        obj_id% = g_obj(i%, 0)
        ' Skip free slots and shadows
        if obj_id% = 0 or obj_id%=39 then continue for
        ' Skip enemies if freeze is enabled, except fire animation
        if obj_id% < 20 and g_freeze_timer >= 0 then continue for

        offset%=0
        sprite_ix%=OBJ_INI_SPRITE_ID + i%

        ' Config animation
        select case obj_id%
            case 4  ' Knight
                if g_anim_tick% mod 6 > 2 then flip%=1

            case 20 ' Fire
                if g_obj(i%, 3) > bound(FIRE_ANIM()) then
                    destroy_object(i%)
                    continue for
                end if
                offset%=FIRE_ANIM(g_obj(i%, 3))*TILE_SIZEx2
                inc g_obj(i%, 3),1

            case 30 ' Power up
                if g_anim_tick% mod 1.5 > 0 then continue for
                offset%=PUP_ANIM(max(0,g_obj(i%, 4) - 2), g_obj(i%, 3))*TILE_SIZEx2
                if g_obj(i%, 3) = bound(PUP_ANIM(),2) then
                    g_obj(i%, 3)=0
                else
                    inc g_obj(i%, 3),1
                end if

            case else ' Other objects
                if g_anim_tick% mod 6 > 2 then offset%=TILE_SIZEx2
        end select

        ' Read or flip next frame's sprite
        select case obj_id%
            case 4
                sprite show safe sprite_ix%, sprite(X, sprite_ix%), sprite(Y, sprite_ix%), sprite(L, sprite_ix%), flip%
            case else
                sprite read sprite_ix%, OBJ_TILE%(obj_id%,0)+offset%, OBJ_TILE%(obj_id%,1), OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
        end select
    next
end sub

sub move_shots()
    local i%,x,y
    for i%=0 to bound(g_shots())
        ' Checks weapon id
        if g_shots(i%,0)=0 then continue for
        ' Moves shot
        inc g_shots(i%,1),g_shots(i%,3)*g_delta_time
        inc g_shots(i%,2),g_shots(i%,4)*g_delta_time
        x = g_shots(i%,1)
        y = g_shots(i%,2)
        if y < 0 or y > SCREEN_HEIGHT+TILE_SIZEx2 or x < 0 or x > SCREEN_WIDTH then
            destroy_shot(i%+2)
        else
            sprite next i%+2, x, y
        end if
    next
end sub

sub move_and_process_objects()
    if g_freeze_timer >= 0 then exit sub

    local i%, sprite_id%, obj_id%, screen_offset%, offset_y%, offset_x%

    for i%=0 to bound(g_obj())
        offset_x%=0: offset_y%=0
        obj_id%=g_obj(i%,0)

        ' Checks object id
        if not obj_id% then continue for

        screen_offset%=TILE_SIZE

        ' Moves object
        select case obj_id%
            case 2,3 ' Bat
                ' Calculate new X
                ' Simple bat
                if obj_id%=2 then
                    if g_obj(i%,1) > SCREEN_CENTER then
                        g_obj(i%,1)=SCREEN_WIDTH+SCREEN_CENTER*cos(g_obj(i%,4))
                    else
                        g_obj(i%,1)=(SCREEN_CENTER-TILE_SIZE)*cos(g_obj(i%,4))
                    end if
                else ' Bat wave
                    g_obj(i%,1)=(SCREEN_CENTER-TILE_SIZE)+(SCREEN_CENTER-TILE_SIZEx4)*cos(g_obj(i%,4))
                end if

                ' Increment angle by the speed
                inc g_obj(i%,4),OBJ_DATA(obj_id%,0)*g_delta_time
                ' Increment Y
                inc g_obj(i%,2),OBJ_DATA(obj_id%,1)*g_delta_time
                ' Compensates sprite's tile misalignment
                if g_anim_tick% mod 6 < 3 then offset_y%=OBJ_TILE%(obj_id%,3)/2

            case 4 ' Knight
                ' Check if it is time to change direction
                if g_obj(i%,4)=0 and g_player(1)-g_obj(i%,2) < KNIGHT_CHANGE_DIRECT_DIST_PX then
                    g_obj(i%,4)=g_obj(i%,1) ' Save the last X position
                    g_obj(i%,5)=choice(g_obj(i%,1)>SCREEN_CENTER,-1,1) ' Horizontal movement direction
                else if g_obj(i%,5) and abs(g_obj(i%,4)-g_obj(i%,1)) > KNIGHT_MAX_HORIZONTAL_DIST_PX then
                    g_obj(i%,5)=0
                end if

                if g_obj(i%,5) then
                    inc g_obj(i%,1),OBJ_DATA(obj_id%,0)*g_delta_time*g_obj(i%,5)
                else
                    inc g_obj(i%,2),OBJ_DATA(obj_id%,1)*g_delta_time
                end if

            case 30 ' Power up
                if g_obj(i%,5) >= 0 then
                    offset_x%=-TILE_SIZEx4*sin(g_obj(i%,5))
                    ' Increment angle by the speed
                    inc g_obj(i%,5),OBJ_DATA(obj_id%,0)*g_delta_time
                end if
                inc g_obj(i%,2),OBJ_DATA(obj_id%,1)*g_delta_time

            case 39 ' Shadow
                g_obj(i%,1)=g_obj(g_obj(i%,6),1)
                g_obj(i%,2)=g_obj(g_obj(i%,6),2)+g_obj(i%,4)
                screen_offset%=-TILE_SIZE

            case else
                ' Increment X and Y position
                inc g_obj(i%,1),OBJ_DATA(obj_id%,0)*g_delta_time
                inc g_obj(i%,2),OBJ_DATA(obj_id%,1)*g_delta_time
        end select

        ' Move or destroy the sprite if out of bounds
        sprite_id%=OBJ_INI_SPRITE_ID + i%
        if not sprite(e,sprite_id%) and g_obj(i%,2) <= SCREEN_HEIGHT then
            sprite next sprite_id%, g_obj(i%,1)+offset_x%, g_obj(i%,2)+offset_y%
        else if obj_id% = 39 then ' Shadow
            delete_shadow(g_obj(i%,6))
        else
            destroy_object(i%)
        end if
    next
end sub

sub spawn_object(obj_id%, x%, y%, map_data%)
    local sprite_id%, offset_x%, offset_y%, layer%=1, i%=get_free_object_slot()
    if i% < 0 then exit sub

    g_obj(i%,0)=obj_id%             ' Object Id
    g_obj(i%,1)=x%                  ' X
    g_obj(i%,2)=y%                  ' Y
    g_obj(i%,3)=OBJ_DATA(obj_id%,2) ' GPR 1
    g_obj(i%,4)=OBJ_DATA(obj_id%,3) ' GPR 2
    g_obj(i%,5)=OBJ_DATA(obj_id%,4) ' GPR 3
    g_obj(i%,6)=-1                  ' Shadow index

    select case obj_id%
        case 2,3 ' Bat
            ' Calculate the correct bat angle
            if x% < SCREEN_CENTER then g_obj(i%,4)=g_obj(i%,4)+180
            ' Initialize bat spawning data
            if map_data% then
                enqueue_action(map_data%, obj_id%, x%, y%,, choice(obj_id%=2, BAT_SPAWN_SPEED_MS, BAT_WAVE_SPAWN_SPEED_MS))
            end if
            ' Spawn the bat shadow
            create_shadow(i%, TILE_SIZEx2+TILE_SIZE/2)

        case 20 ' Fire
            offset_x%=FIRE_ANIM(0)*TILE_SIZEx2
            layer%=4
    end select

    sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, OBJ_TILE%(obj_id%,0)+offset_x%, OBJ_TILE%(obj_id%,1)+offset_y%, OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), layer%
end sub

sub spawn_block(x%, y%, type%)
    local i%, block_id%, row%, col%
    for i%=0 to bound(g_blocks())
        if g_blocks(i%, 0) = 0 then exit for
    next

    block_id%=31
    g_blocks(i%,0)=type% ' Block type
    g_blocks(i%,1)=0     ' Hits

    sprite read BLOCK_INI_SPRITE_ID + i%, TRANSPARENT_BLOCK_X, TRANSPARENT_BLOCK_Y, TILE_SIZEx2, TILE_SIZEx2, OBJ_TILES_BUFFER
    sprite show safe BLOCK_INI_SPRITE_ID + i%, x%,0, 1
end sub

sub enqueue_action(exec_count%, action_id%, gpr1, gpr2, gpr3, next_exec_ms)
    local i%
    for i%=0 to bound(g_actions_queue())
        if g_actions_queue(i%,0) then continue for

        g_actions_queue(i%,0)=exec_count% ' Execution count
        g_actions_queue(i%,1)=action_id%  ' Action Id
        g_actions_queue(i%,2)=gpr1        ' GPR 1
        g_actions_queue(i%,3)=gpr2        ' GPR 2
        g_actions_queue(i%,4)=gpr3        ' GPR 3
        g_actions_queue(i%,5)=timer + next_exec_ms
        exit for
    next
end sub

sub create_shadow(obj_ix%, height%)
    local sprite_id%, i%=get_free_object_slot()
    if i% < 0 then exit sub

    g_obj(i%,0)=39                          ' Shadow id
    g_obj(i%,1)=g_obj(obj_ix%,1)         ' X
    g_obj(i%,2)=g_obj(obj_ix%,2)+height% ' Y
    g_obj(i%,4)=height%                     ' Height
    g_obj(i%,6)=obj_ix%                  ' Shadow -> source object index
    g_obj(obj_ix%,6)=i%                  ' Source object -> shadow index
    sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, OBJ_TILE%(39,0), OBJ_TILE%(39,1), OBJ_TILE%(39,2), OBJ_TILE%(39,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), 3
end sub

' Returns -1 if there are no free slots
function get_free_object_slot() as integer
    local i%
    get_free_object_slot=-1
    for i%=0 to bound(g_obj())
        ' Check for free slots
        if g_obj(i%,0) then continue for
        get_free_object_slot=i%
        exit for
    next
end function

sub get_free_shot_slots(free_slots%())
    local i%,c%=0
    ' Starts from 3 because 0 to 2 slots are reserved for the player
    for i%=3 to bound(g_shots())
        ' Check for free slots
        if g_shots(i%,0) then continue for
        free_slots%(c%)=i%
        inc c%
    next
end sub

sub fire()
    ' Cooldown
    if timer - g_pshot_timer < g_player_shot_ms then exit sub

    local i%
    for i%=0 to choice(g_player(3)>5,2,1)
        ' Ignore used slots
        if g_shots(i%,0) then continue for

        ' Create the shot state
        ' TODO: Create wapons data table
        g_shots(i%,0)=g_player(3)                   ' weapon
        g_shots(i%,1)=g_player(0)+OBJ_TILE%(40,2)/2 ' X
        g_shots(i%,2)=g_player(1)-OBJ_TILE%(40,3)-1 ' Y
        g_shots(i%,3)=0                             ' Speed X
        g_shots(i%,4)=-220                          ' Speed Y
        ' Create the shot sprite
        sprite read i%+2, OBJ_TILE%(40,0), OBJ_TILE%(40,1), OBJ_TILE%(40,2), OBJ_TILE%(40,3), OBJ_TILES_BUFFER
        sprite show safe i%+2, g_shots(i%,1),g_shots(i%,2), 1
        ' Play SFX
        if g_sound_on% then play effect "SHOT_SFX"
        ' Increment timer
        g_pshot_timer=timer
        exit for
    next
end sub

sub enemies_fire()
    if g_freeze_timer >= 0 then exit sub

    local i%,c%,slot_ix%
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
    sprite read shot_ix%+2, OBJ_TILE%(idx%,0), OBJ_TILE%(idx%,1), OBJ_TILE%(idx%,2), OBJ_TILE%(idx%,3), OBJ_TILES_BUFFER
    sprite show safe shot_ix%+2, g_shots(shot_ix%,1),g_shots(shot_ix%,2), 1, rot%
end sub

sub move_player(direction%)
    local x=g_player(0), y=g_player(1)

    select case direction%
        case KB_LEFT
            inc g_player(0), -g_player(4)*g_delta_time
            if map_collide(g_player()) then g_player(0)=x
            if g_player(0) < 0 then g_player(0)=SCREEN_WIDTH - TILE_SIZE * 2
            check_scroll_collision()
        case KB_RIGHT
            inc g_player(0), g_player(4)*g_delta_time
            if map_collide(g_player()) then g_player(0)=x
            if g_player(0) > SCREEN_WIDTH - TILE_SIZE * 2 then g_player(0)=0
            check_scroll_collision()
        case KB_UP
            inc g_player(1), -g_player(4)*g_delta_time
            if map_collide(g_player()) then g_player(1)=y
            if g_player(1) < TILE_SIZE * 6 then g_player(1)=TILE_SIZE * 6
            check_scroll_collision()
        case KB_DOWN
            inc g_player(1), g_player(4)*g_delta_time
            if map_collide(g_player()) then g_player(1)=y
            if g_player(1) > SCREEN_HEIGHT then g_player(1)=SCREEN_HEIGHT
            check_scroll_collision()
    end select
end sub

sub check_scroll_collision()
    if map_collide(g_player()) then
        inc g_player(1),2
        'TODO: Implement player kill by crush
    end if
end sub

sub scroll_map()
    local i%, sprite_id%

    ' Blocks scroll
    for i%=0 to bound(g_blocks())
        if g_blocks(i%,0)=0 then continue for
        sprite_id%=BLOCK_INI_SPRITE_ID + i%
        sprite next sprite_id%, sprite(X, sprite_id%), sprite(Y, sprite_id%) + 1
        if sprite(Y, sprite_id%) > SCREEN_HEIGHT+TILE_SIZEx2 then destroy_block(i%)
    next

    ' Sprite scroll 0,-1
    sprite scrollr 0,0,SCREEN_WIDTH,SCREEN_HEIGHT+TILE_SIZE*2,0,-1
    check_scroll_collision()
    inc g_tile_px%,1

    ' Draw the next map tile row
    if g_tile_px% = TILE_SIZE then
        g_tile_px%=0
        inc g_row%,-1
        ' Draw the next map row on the top of the screen buffer
        if g_row% >= 0 then
            process_map_row(g_row%)
        end if
    end if
end sub

sub process_kb()
    local kb1%=KeyDown(1), kb2%=KeyDown(2), kb3%=KeyDown(3)

    if not kb1% and not kb2% and not kb3% then
        g_kb_released%=true
        exit sub
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

    if g_kb_released% and (kb1%=KB_SPACE or kb2%=KB_SPACE or kb3%=KB_SPACE) then
        fire()
        g_kb_released%=false
    else if kb1%<>KB_SPACE and kb2%<>KB_SPACE and kb3%<>KB_SPACE then
        g_kb_released%=true
    end if
end sub

'
' Returns false (0) or true (1)
function map_collide(player()) as integer
    local col%=player(0)\TILE_SIZE
    local row%=(player(1)-g_tile_px%)\TILE_SIZE+g_row%
    ' Check top left
    map_collide=g_map((row%+1)*MAP_COLS+col%) and &H80
    ' Check top right
    if not map_collide then map_collide=g_map((row%+1)*MAP_COLS+(col%+2)) >> 7 and 1
    ' Check bottom left
    if not map_collide then map_collide=g_map((row%+2)*MAP_COLS+col%) >> 7 and 1
    ' Check bottom right
    if not map_collide then map_collide=g_map((row%+2)*MAP_COLS+(col%+2)) >> 7 and 1
    ' TODO: Check horizontal wrapping collision
end function

sub process_map_row(row%)
    local tile%,col%,tx%,ty%,obj_id%,tile_data%,extra%

    page write SCREEN_BUFFER
    for col%=MAP_COLS_0 to 0 step -1
        ' Tile drawing
        tile_data%=g_map(row%*MAP_COLS+col%)
        tile%=(tile_data% AND &H7F) - 1
        tx%=(tile% mod TILES_COLS) * TILE_SIZE
        ty%=(tile% \ TILES_COLS) * TILE_SIZE
        blit tx%,ty%, col%*TILE_SIZE,0, TILE_SIZE,TILE_SIZE, MAP_TILES_BUFFER

        ' Create object
        obj_id%=(tile_data% AND &H1F00) >> 8
        if obj_id% then
            extra%=(tile_data% AND &HE000) >> 13
            select case obj_id%
                case 30 ' Power Up
                    spawn_object(30, min(SCREEN_WIDTH-TILE_SIZE*6,max(TILE_SIZEx4,g_player(0))), 0)
                case 31 ' Block
                    spawn_block(col%*TILE_SIZE, 0, extra%)
                case else ' Enemies and other objects
                    spawn_object(obj_id%, col%*TILE_SIZE, 0, extra%)
            end select
        end if
    next
    page write 0
end sub
