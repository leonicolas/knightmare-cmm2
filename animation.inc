sub animate_player()
    local offset%
    local speed=choice(g_player(5)=4,PLAYER_MAX_SPEED,g_player(4))
    inc g_player(2),choice(g_player_is_moving,2+speed/PLAYER_INIT_SPEED,1)
    select case g_player(5)
        case 1
            if sprite(X, 2) > 1000 then sprite show safe 2, g_player(0),g_player(1)-TILE_SIZE, 1
        case is > 1
            if g_power_up_timer > 10 or (g_power_up_timer - fix(g_power_up_timer)) > 0.5 then
                offset%=TILE_SIZEx2*g_player(5)
            end if
            if sprite(X, 2) < 1000 then sprite hide safe 2
    end select

    local x%=choice(g_player(2) mod 10 < 10/2,PLAYER_SKIN1_X_R,PLAYER_SKIN1_X_L)
    sprite read #1, x%+offset%, PLAYER_SKIN_Y, TILE_SIZEx2, TILE_SIZEx2, OBJ_TILES_BUFFER
end sub

sub animate_objects()
    local i%, obj_id%, sprite_ix%, offset%, flip%

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

            case 29 ' Weapon chrystal
                offset%=WEAPON_ANIM(max(0,g_obj(i%, 4) - 2), g_obj(i%, 3))*TILE_SIZEx2
                if g_obj(i%, 3) = bound(WEAPON_ANIM(),2) then g_obj(i%, 3)=0 else inc g_obj(i%, 3),1

            case 30 ' Power-up chrystal
                if g_anim_tick% mod 1.5 > 0 then continue for
                offset%=PUP_ANIM(max(0,g_obj(i%, 4) - 2), g_obj(i%, 3))*TILE_SIZEx2
                if g_obj(i%, 3) = bound(PUP_ANIM(),2) then g_obj(i%, 3)=0 else inc g_obj(i%, 3),1

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

sub animate_shots()
    local i%, sprite_id%, rot%=-1, tile_id%, playing%
    for i%=0 to bound(g_shots())
        ' Checks weapon id
        if g_shots(i%,0)=0 then continue for

        sprite_id%=i% + SHOTS_INI_SPRITE_ID

        ' Animate shot
        select case g_shots(i%,0)
            case 4,9 ' Boomerang
                inc g_shots(i%,3)
                select case g_shots(i%,3) mod 12
                    case 9
                        rot%=3: tile_id%=44
                    case 6
                        rot%=3: tile_id%=43
                    case 3
                        rot%=0: tile_id%=44
                    case 0
                        rot%=0: tile_id%=43
                end select
                ' Rotate boomerang
                if rot% >= 0 then
                    if g_sound_on% and g_cont_sfx$ <> "" and not playing% then
                        play effect g_cont_sfx$
                        playing%=true
                    end if
                    sprite hide safe sprite_id%
                    sprite close sprite_id%
                    sprite read sprite_id%, OBJ_TILE%(tile_id%,0), OBJ_TILE%(tile_id%,1), OBJ_TILE%(tile_id%,2), OBJ_TILE%(tile_id%,3), OBJ_TILES_BUFFER
                    sprite show safe sprite_id%, g_shots(i%,1),g_shots(i%,2), 1, rot%
                end if

        end select
    next
end sub

sub move_player(direction%)
    local x=g_player(0), y=g_player(1)
    ' Player speed or max speed for invincibility power-up
    local speed=choice(g_player(5)=4,PLAYER_MAX_SPEED,g_player(4))
    select case direction%
        case KB_LEFT
            inc g_player(0), -speed*g_delta_time
            if map_collide(g_player()) then g_player(0)=x
            if g_player(0) < 0 then g_player(0)=SCREEN_WIDTH - TILE_SIZE * 2
            g_player_is_moving=true
            check_scroll_collision()
        case KB_RIGHT
            inc g_player(0), speed*g_delta_time
            if map_collide(g_player()) then g_player(0)=x
            if g_player(0) > SCREEN_WIDTH - TILE_SIZE * 2 then g_player(0)=0
            g_player_is_moving=true
            check_scroll_collision()
        case KB_UP
            inc g_player(1), -speed*g_delta_time
            if map_collide(g_player()) then g_player(1)=y
            if g_player(1) < TILE_SIZE * 6 then g_player(1)=TILE_SIZE * 6
            g_player_is_moving=true
            check_scroll_collision()
        case KB_DOWN
            inc g_player(1), speed*g_delta_time
            if map_collide(g_player()) then g_player(1)=y
            if g_player(1) > SCREEN_HEIGHT then g_player(1)=SCREEN_HEIGHT
            g_player_is_moving=true
            check_scroll_collision()
    end select
end sub

sub move_shots()
    local i%,x,y
    for i%=0 to bound(g_shots())
        ' Checks weapon id
        if g_shots(i%,0)=0 then continue for
        ' Moves shot
        select case g_shots(i%,0)
            case 4,9 ' Boomerang
                x = g_player(0)
                inc g_shots(i%,4),2.5
            case else ' Other weapons
                inc g_shots(i%,1),g_shots(i%,3)*g_delta_time
                x = g_shots(i%,1)
        end select

        inc g_shots(i%,2),g_shots(i%,4)*g_delta_time
        select case g_shots(i%,0)
            case 4,9 ' Boomerang
                if g_shots(i%,2) < 0 then g_shots(i%,2) = 0
        end select
        y = g_shots(i%,2)

        if y < 0 or y > SCREEN_HEIGHT+TILE_SIZEx2 or x < 0 or x > SCREEN_WIDTH then
            destroy_shot(i% + SHOTS_INI_SPRITE_ID)
        else
            sprite next i% + SHOTS_INI_SPRITE_ID, x, y
        end if
    next
end sub

sub move_and_process_objects()
    ' Objects and enemies
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

            case 29,30 ' Weapon and power-up chrystal
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
            destroy_shadow(g_obj(i%,6))
        else
            destroy_object(i%)
        end if
    next
end sub

sub start_enemy_death_animation(i%)
    enqueue_action(1, 20, g_obj(i%,1), g_obj(i%,2))
end sub
