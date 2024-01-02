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
    local i%, obj_id%, sprite_ix%, offset_x%, offset_y%, flip%, buffer%

    for i%=0 to bound(g_obj())
        obj_id% = g_obj(i%, 0)
        ' Skip free slots, shadows and bosses
        if obj_id% = 0 or obj_id%=39 or obj_id% > 50 then continue for
        ' Skip enemies if freeze is enabled, except fire animation
        if obj_id% < 24 and g_freeze_timer >= 0 then continue for

        buffer%=OBJ_TILES_BUFFER
        offset_x%=0: offset_y%=0
        sprite_ix%=OBJ_INI_SPRITE_ID + i%

        ' Config animation
        select case obj_id%
            case 4  ' Knight
                if g_anim_tick% mod 6 > 2 then flip%=1

            case 24 ' Fire
                if g_obj(i%, 3) > bound(FIRE_ANIM()) then
                    destroy_object(i%)
                    continue for
                end if
                offset_x%=FIRE_ANIM(g_obj(i%, 3))*TILE_SIZEx2
                inc g_obj(i%, 3),1

            case 29 ' Weapon chrystal
                offset_x%=WEAPON_ANIM(max(0,g_obj(i%, 4) - 2), g_obj(i%, 3))*TILE_SIZEx2
                if g_obj(i%, 3) = bound(WEAPON_ANIM(),2) then g_obj(i%, 3)=0 else inc g_obj(i%, 3),1

            case 30 ' Power-up chrystal
                if g_anim_tick% mod 1.5 > 0 then continue for
                offset_x%=PUP_ANIM(max(0,g_obj(i%, 4) - 2), g_obj(i%, 3))*TILE_SIZEx2
                if g_obj(i%, 3) = bound(PUP_ANIM(),2) then g_obj(i%, 3)=0 else inc g_obj(i%, 3),1

            case else ' Other objects
                if g_anim_tick% mod 6 > 2 then offset_x%=TILE_SIZEx2
        end select

        ' Read or flip next frame's sprite
        select case obj_id%
            case 4
                sprite show safe sprite_ix%, sprite(X, sprite_ix%), sprite(Y, sprite_ix%), sprite(L, sprite_ix%), flip%
            case else
                sprite read sprite_ix%, OBJ_TILE%(obj_id%,0)+offset_x%, OBJ_TILE%(obj_id%,1)+offset_y%, OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), buffer%
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
                    if g_cont_sfx$ <> "" and not playing% then
                        play_sfx(g_cont_sfx$)
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

sub auto_move_player_to_portal()
    local y%=choice(fix(g_player(1)) > TILE_SIZE*6, TILE_SIZE*6, 0)
    move_player_to(SCREEN_WIDTH/2-TILE_SIZE, y%)
    if fix(g_player(1)) = 0 then
        g_player(8)=5 ' Ready to next stage
    end if
end sub

sub move_player_to(x%, y%)
    local ang=atan2(y% - g_player(1) + TILE_SIZE, x% - g_player(0))
    inc g_player(0), PLAYER_INIT_SPEED*g_delta_time*cos(ang)*1.5
    if fix(g_player(1)) >= y% then
        inc g_player(1), -PLAYER_INIT_SPEED*g_delta_time
    end if
end sub

sub move_player_to_portal()
    create_portal()
    g_player(8)=3 ' Move to portal state
    play_sfx("END_STAGE_SFX")
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

    local i%, sprite_id%, obj_id%, screen_offset%, offset_y%, offset_x%, data_id%

    for i%=0 to bound(g_obj())
        offset_x%=0: offset_y%=0
        obj_id%=g_obj(i%,0)

        ' Checks object id or if it is a boss
        if not obj_id% or obj_id% > 50 then continue for

        data_id%=OBJ_TILE%(obj_id%,4)
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
                inc g_obj(i%,4),OBJ_DATA(data_id%,0)*g_delta_time
                ' Increment Y
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time
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
                    inc g_obj(i%,1),OBJ_DATA(data_id%,0)*g_delta_time*g_obj(i%,5)
                else
                    inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time
                end if

            case 29,30 ' Weapon and power-up chrystal
                if g_obj(i%,5) >= 0 then
                    offset_x%=-TILE_SIZEx4*sin(g_obj(i%,5))
                    ' Increment angle by the speed
                    inc g_obj(i%,5),OBJ_DATA(data_id%,0)*g_delta_time
                end if
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time

            case 39 ' Shadow
                g_obj(i%,1)=g_obj(g_obj(i%,6),1)
                g_obj(i%,2)=g_obj(g_obj(i%,6),2)+g_obj(i%,4)
                screen_offset%=-TILE_SIZE

            case else
                ' Increment X and Y position
                inc g_obj(i%,1),OBJ_DATA(data_id%,0)*g_delta_time
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time
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

sub start_enemy_death_animation(i%, sfx%)
    enqueue_action(1, 24, g_obj(i%,1), g_obj(i%,2), sfx%)
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
        play_sfx(sfx$)

        ' Increment timer
        g_pshot_timer=timer
        if g_player(3) <> 3 and g_player(3) <> 8 then exit for
    next
end sub

sub enemies_fire()
    if g_freeze_timer >= 0 then exit sub

    local i%,c%,slot_ix%
    local free_slots%(bound(g_shots()))
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
    local obj_id%=g_obj(enemy_ix%,0)
    if obj_id% < 50 and (g_obj(enemy_ix%,2) > MAX_ENEMIES_SHOOT_Y or rnd > ENEMIES_SHOOT_CHANCE) then exit sub
    if shot_ix%=0 then
        shot_ix%=get_free_shot_slot()
        if shot_ix% < 0 then exit sub
    end if

    local enemy_spr_id%=OBJ_INI_SPRITE_ID + enemy_ix%
    local shot_spr_id%=shot_ix%+SHOTS_INI_SPRITE_ID
    local cx=sprite(W, enemy_spr_id%)/2, cy=sprite(H, enemy_spr_id%)/2
    local angle=deg(sprite(V, enemy_spr_id%, 1))
    local idx%=14,rot%,speed

    select case obj_id%
        case 1   ' Blob
            speed=BLOB_SHOT_SPEED
        case 2,3 ' Bat
            speed=BAT_SHOT_SPEED
        case 4   ' Knight
            speed=KNIGHT_SHOT_SPEED
            rot%=fix((angle+22.5)/45)
            if rot%=0 or rot%=4 then
                idx%=20: rot%=choice(rot%=0,2,0)
            else if rot%=2 or rot%=6 then
                idx%=21: rot%=choice(rot%=2,1,0)
            else if rot%=3 or rot%=5 then
                idx%=22: rot%=choice(rot%=3,1,0)
            else
                idx%=22: rot%=choice(rot%=1,3,2)
            end if
        case 51 ' Boss stage 1
            idx%=15
            speed=BOSS_SHOT_SPEED

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

' Returns -1 if there are no free slots
function get_free_shot_slot() as integer
    local i%
    get_free_shot_slot=-1
    ' Starts from 3 because 0 to 2 slots are reserved for the player
    for i%=3 to bound(g_shots())
        ' Check for free slots
        if g_shots(i%,0) then continue for
        get_free_shot_slot=i%
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

sub destroy_enemies_shots()
    local i%
    ' 0 to 2 is reserved for player shots
    for i%=3 to bound(g_shots())
        if g_shots(i%, 0) > 0 then destroy_shot(i% + SHOTS_INI_SPRITE_ID)
    next
end sub

sub destroy_shot(sprite_id%)
    sprite hide safe sprite_id%
    sprite close sprite_id%
    g_shots(sprite_id% - SHOTS_INI_SPRITE_ID, 0) = 0
end sub

sub destroy_object(obj_ix%)
    if obj_ix% < 0 or g_obj(obj_ix%, 0) <= 0 then
        exit sub
    end if
    local sprite_id%=OBJ_INI_SPRITE_ID + obj_ix%
    g_obj(obj_ix%, 0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
end sub

sub destroy_shadow(src_obj_ix%)
    local shadow_ix%=g_obj(src_obj_ix%,6)

    if shadow_ix% < 0 then exit sub

    g_obj(src_obj_ix%,6) = -1
    destroy_object(shadow_ix%)
end sub

sub destroy_all()
    local i%
    ' Destroy objects
    for i%=0 to bound(g_obj())
        if g_obj(i%,0)=0 then continue for
        destroy_object(i%)
    next
    ' Destroy shots
    for i%=0 to bound(g_shots())
        if g_shots(i%, 0) > 0 then destroy_shot(i% + SHOTS_INI_SPRITE_ID)
    next
    sprite close all()
end sub

sub kill_all_enemies(sfx%)
    local i%
    if sfx% then play_sfx("KILL_ALL_ENEMIES_SFX")
    for i%=0 to bound(g_obj())
        ' Check for free slots or boss id
        if g_obj(i%,0)=0 or g_obj(i%,0) > 50 then continue for
        hit_object(OBJ_INI_SPRITE_ID + i%, false, true)
    next
end sub

function allocate_object(obj_id%, x%, y%, gpr1, gpr2, gpr3) as integer
    local i%=get_free_object_slot()
    allocate_object=i%
    if i% < 0 then exit function

    g_obj(i%,0)=obj_id%
    g_obj(i%,1)=x%
    g_obj(i%,2)=y%
    g_obj(i%,3)=gpr1
    g_obj(i%,4)=gpr2
    g_obj(i%,5)=gpr3
    g_obj(i%,6)=-1   ' Shadow index
end function

sub spawn_object(obj_id%, x%, y%, obj_cfg%)
    local i%=allocate_object(obj_id%, x%, y%)
    if i% < 0 then exit sub

    local sprite_id%, offset_x%, offset_y%, layer%=1, data_id%
    data_id%=OBJ_TILE%(obj_id%,4)

    g_obj(i%,3)=OBJ_DATA(data_id%,2) ' GPR 1
    g_obj(i%,4)=OBJ_DATA(data_id%,3) ' GPR 2
    g_obj(i%,5)=OBJ_DATA(data_id%,4) ' GPR 3

    select case obj_id%
        case 2,3 ' Bat
            ' Calculate the correct bat angle
            if x% < SCREEN_CENTER then g_obj(i%,4)=g_obj(i%,4)+180
            ' Initialize bat spawning data
            if obj_cfg% then
                enqueue_action(obj_data%, obj_id%, x%, y%,, choice(obj_id%=2, BAT_SPAWN_SPEED_MS, BAT_WAVE_SPAWN_SPEED_MS))
            end if
            ' Spawn the bat shadow
            spawn_shadow(i%, TILE_SIZEx2+TILE_SIZE/2)

        case 24 ' Fire
            offset_x%=FIRE_ANIM(0)*TILE_SIZEx2
            layer%=4
            if obj_cfg% then play_sfx("ENEMY_KILL_SFX")
    end select

    sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, OBJ_TILE%(obj_id%,0)+offset_x%, OBJ_TILE%(obj_id%,1)+offset_y%, OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), layer%
end sub

sub spawn_shield()
    sprite read 2, OBJ_TILE%(32,0), OBJ_TILE%(32,1), OBJ_TILE%(32,2), OBJ_TILE%(32,3), OBJ_TILES_BUFFER
    if sprite(X, 2) = 10000 then sprite show 2, g_player(0),g_player(1)-TILE_SIZE, 1
    g_player(5)=1
    g_player(6)=SHIELD_MAX_HITS
end sub

sub spawn_shadow(obj_ix%, height%)
    local sprite_id%, i%=get_free_object_slot()
    if i% < 0 then exit sub

    g_obj(i%,0)=39                       ' Shadow id
    g_obj(i%,1)=g_obj(obj_ix%,1)         ' X
    g_obj(i%,2)=g_obj(obj_ix%,2)+height% ' Y
    g_obj(i%,4)=height%                  ' Height
    g_obj(i%,6)=obj_ix%                  ' Shadow -> source object index
    g_obj(obj_ix%,6)=i%                  ' Source object -> shadow index
    sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, OBJ_TILE%(39,0), OBJ_TILE%(39,1), OBJ_TILE%(39,2), OBJ_TILE%(39,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), 3
end sub

