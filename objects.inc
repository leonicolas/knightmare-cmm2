sub animate_player()
    local offset%
    local speed=choice(g_player(5)=4,PLAYER_MAX_SPEED,g_player(4))
    if g_player(8)=4 then
        offset%=PLAYER_DEATH_ANIM(fix(min(g_player(2),bound(PLAYER_DEATH_ANIM()))))*TILE_SIZE
        inc g_player(2),0.5
        if g_player(2) > bound(PLAYER_DEATH_ANIM())*1.5 then g_player(8)=6
    else
        inc g_player(2),choice(g_player_is_moving,2+speed/PLAYER_INIT_SPEED,1)
        ' Power up
        select case g_player(5)
            case 1
                if sprite(X, 2) = 10000 then sprite show safe 2, g_player(0),g_player(1)-TILE_SIZE, 1
            case is > 1
                if g_power_up_timer > 10 or (g_power_up_timer - fix(g_power_up_timer)) > 0.5 then
                    offset%=TILE_SIZEx2*g_player(5)
                end if
                if sprite(X, 2) < 10000 then sprite hide safe 2
        end select
    end if
    local x%=choice(g_player(8) <> 4 and g_player(2) mod 10 < 5,PLAYER_SKIN1_X_L,PLAYER_SKIN1_X_R)
    sprite read #1, x%+offset%, PLAYER_SKIN_Y, TILE_SIZEx2, TILE_SIZEx2, OBJ_TILES_BUFFER
end sub

sub animate_objects()
    local i%,obj_id%,sprite_id%,offset_x%,offset_y%,offset_h%,effect%,buffer%,x,y,sfx%,red_death%=-1

    for i%=0 to bound(g_obj())
        obj_id% = g_obj(i%, 0)
        ' Skip free slots, shadows and bosses
        if obj_id% = 0 or obj_id%=39 or obj_id% > 50 then continue for
        ' Skip enemies if freeze is enabled, except fire animation
        if obj_id% < 20 and g_freeze_timer >= 0 then continue for

        effect%=-1
        buffer%=OBJ_TILES_BUFFER
        offset_x%=0: offset_y%=0
        sprite_id%=OBJ_INI_SPRITE_ID + i%

        ' Config animation
        select case obj_id%
            case 4,9,10,15 ' Knight, Death Ghost, Zombie, Sorcerer
                effect%=g_anim_tick% mod 6 > 2

            case 7 ' Skeleton
                if g_obj(i%, 4) > 1 then continue for
                if g_anim_tick% mod 6 > 2 then offset_x%=TILE_SIZEx2

            case 8 ' Black demon
                offset_x%=0
                if g_obj(i%,5) <= 0 then
                    obj_id%=5 ' cloud form
                else
                    effect%=g_anim_tick% mod 6 > 2
                end if

            case 11 ' Ghost
                continue for

            case 12 ' Yellow thing
                offset_x%=0

            case 16 ' Red Death ghost
                if red_death%>=0 then g_obj(i%,5)=g_obj(red_death%,5) else red_death%=i%
                select case g_obj(i%,5)
                    case is > 40
                        g_obj(i%,5)=0
                    case is > 32
                        offset_x%=choice(g_obj(i%,5) mod 2,TILE_SIZEx2,0)
                    case is > 8
                        offset_x%=TILE_SIZEx2
                    case is > 0
                        offset_x%=choice(g_obj(i%,5) mod 2,TILE_SIZEx2,0)
                end select
                if g_obj(i%,5) > 0 then inc g_obj(i%,5)
                if g_fire% and g_obj(i%,5)=0 then g_obj(i%,5)=1
                if sfx%=0 and g_anim_tick% mod 20 = 0 then
                    play_sfx("RED_DEATH")
                    sfx%=1
                end if
                if g_obj(i%,5) = 0 then continue for

            case 20 ' Weapon chrystal
                offset_x%=WEAPON_ANIM(max(0,g_obj(i%, 4) - 2), g_obj(i%, 3))*TILE_SIZEx2
                if g_obj(i%, 3) = bound(WEAPON_ANIM(),2) then g_obj(i%, 3)=0 else inc g_obj(i%, 3),1

            case 21 ' Power-up chrystal
                if g_anim_tick% mod 1.5 > 0 then continue for
                offset_x%=PUP_ANIM(max(0,g_obj(i%, 4) - 2), g_obj(i%, 3))*TILE_SIZEx2
                if g_obj(i%, 3) = bound(PUP_ANIM(),2) then g_obj(i%, 3)=0 else inc g_obj(i%, 3),1

            case 34 ' Fire
                if g_obj(i%, 3) > bound(FIRE_ANIM()) then
                    destroy_object(i%)
                    continue for
                end if
                offset_x%=FIRE_ANIM(g_obj(i%, 3))*TILE_SIZEx2
                inc g_obj(i%, 3),1

            case 49 ' Terrain
                if g_anim_tick% mod 1.6 > 0 then continue for
                effect%=-2
                offset_y%=-TILE_SIZE*g_obj(i%,4)
                offset_h%=min(TILE_SIZE*12, abs(offset_y%))
                inc g_obj(i%,4)
                if g_obj(i%,4) > 16 then
                    if g_obj(i%,4) > 22 then destroy_object(i%)
                    continue for
                end if

            case else ' Other objects
                if g_anim_tick% mod 6 > 2 then offset_x%=TILE_SIZEx2
        end select

        ' Read or flip next frame's sprite
        if effect% >= 0 then
            sprite show safe sprite_id%, sprite(X, sprite_id%), sprite(Y, sprite_id%), sprite(L, sprite_id%), effect%
        else
            x=sprite(X, sprite_id%)
            y=sprite(Y, sprite_id%)
            if effect% = -2 then
                sprite hide safe sprite_id%
                sprite close sprite_id%
            end if
            sprite read sprite_id%, OBJ_TILE%(obj_id%,0)+offset_x%, OBJ_TILE%(obj_id%,1)+offset_y%, OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3)+offset_h%, buffer%
            if effect% = -2 then sprite show safe sprite_id%,x,y,1
        end if
    next
end sub

sub animate_shots()
    local i%, sprite_id%, rot%, tile_id%, playing%
    local offset_x%, offset_y%
    for i%=0 to bound(g_shots())
        ' Checks weapon id
        if g_shots(i%,0)=0 then continue for

        sprite_id%=i% + SHOTS_INI_SPRITE_ID

        ' Animate shot
        select case g_shots(i%,0)
            case 4,9 ' Boomerang
                inc g_shots(i%,3)
                select case g_shots(i%,3) mod 12
                    case 9: rot%=3: tile_id%=44
                    case 6: rot%=3: tile_id%=43
                    case 3: rot%=0: tile_id%=44
                    case 0: rot%=0: tile_id%=43
                    case else: continue for
                end select

            case 25 ' Bone
                inc g_shots(i%,5)
                select case g_shots(i%,5) mod 4
                    case 3: rot%=2: tile_id%=26
                    case 2: rot%=0: tile_id%=27
                    case 1: rot%=0: tile_id%=26
                    case 0: rot%=0: tile_id%=25
                end select

            case 28 ' White explosion
                if g_shots(i%,3) = 0 and g_shots(i%,4) = 0 then
                    tile_id%=35
                    inc g_shots(i%,5)
                    if g_shots(i%,5) > 18 then
                        destroy_shot(sprite_id%)
                        continue for
                    else if g_shots(i%,5) mod 6 > 2 then
                        offset_x%=TILE_SIZEx2
                    end if
                else
                    continue for
                end if

            case 32 ' Axe
                inc g_shots(i%,5)
                tile_id%=32
                select case g_shots(i%,5) mod 8
                    case 6: rot%=1: offset_x%=TILE_SIZEx2
                    case 4: rot%=0: offset_x%=TILE_SIZEx2
                    case 2: rot%=1
                    case 0: rot%=0
                    case else: continue for
                end select

            case 33 ' Scythe
                tile_id%=33: rot%=0
                offset_x%=TILE_SIZEx2*(g_anim_tick% mod 4)

            case else
                continue for
        end select

        ' Play SFX
        if g_cont_sfx$ <> "" and not playing% then
            play_sfx(g_cont_sfx$)
            playing%=true
        end if

        ' Render next frame
        sprite hide safe sprite_id%
        sprite close sprite_id%
        sprite read sprite_id%, OBJ_TILE%(tile_id%,0)+offset_x%, OBJ_TILE%(tile_id%,1)+offset_y%, OBJ_TILE%(tile_id%,2), OBJ_TILE%(tile_id%,3), OBJ_TILES_BUFFER
        sprite show safe sprite_id%, g_shots(i%,1),g_shots(i%,2), 1, rot%
    next
end sub

sub auto_move_player_to_portal()
    local y%=choice(fix(g_player(1)) > TILE_SIZE*6, TILE_SIZE*6, TILE_SIZE)
    move_player_to(SCREEN_WIDTH/2-TILE_SIZE, y%)
    if fix(g_player(1)) = TILE_SIZE then
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
    play_sfx("START_STAGE")
end sub

sub move_player(direction%)
    if g_player(8)=4 then exit sub

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
    local i%,x,y,speed_x,speed_y
    for i%=0 to bound(g_shots())
        ' Checks weapon id
        if g_shots(i%,0)=0 then continue for
        ' Moves shot
        select case g_shots(i%,0)
            case 4,9 ' Boomerang
                g_shots(i%,1) = g_player(0)
                inc g_shots(i%,4),2.5
                speed_y=g_shots(i%,4)

            case 28 ' White explosion
                speed_x=g_shots(i%,3)
                speed_y=g_shots(i%,4)
                y=g_shots(i%,5)
                if y > 0 and speed_y > 0 and g_shots(i%,2) >= y then
                    x=g_shots(i%,1)
                    g_shots(i%,3)=0: speed_x=0
                    g_shots(i%,4)=0: speed_y=0
                    g_shots(i%,5)=0
                end if

            case 33 ' Scythe
                inc g_shots(i%,5),choice(i% mod 2,1,-1)
                if abs(g_shots(i%,5)) > 720 then
                    destroy_shot(i% + SHOTS_INI_SPRITE_ID)
                    continue for
                end if

                g_shots(i%,1)=g_shots(i%,3)+TILE_SIZE*7*sin(g_shots(i%,5))
                g_shots(i%,2)=g_shots(i%,4)+TILE_SIZE*7*-cos(g_shots(i%,5))

            case else ' Other weapons
                speed_x=g_shots(i%,3)
                speed_y=g_shots(i%,4)
        end select

        inc g_shots(i%,1),speed_x*g_delta_time
        inc g_shots(i%,2),speed_y*g_delta_time

        ' Adjust shot position
        select case g_shots(i%,0)
            case 4,9 ' Boomerang
                if g_shots(i%,2) < 0 then g_shots(i%,2) = 0
        end select

        x=g_shots(i%,1): y=g_shots(i%,2)

        if y < 0 or y > SCREEN_HEIGHT+TILE_SIZEx2 or x < 0 or x > SCREEN_WIDTH then
            destroy_shot(i% + SHOTS_INI_SPRITE_ID)
        else
            sprite next i% + SHOTS_INI_SPRITE_ID, x, y
        end if
    next
end sub

sub move_and_process_objects()
    local i%, sprite_ix%, obj_id%, offset_y%, offset_x%, data_id%, direction%, speed, aux, flip%

    for i%=0 to bound(g_obj())
        offset_x%=0: offset_y%=0:flip%=-1
        obj_id%=g_obj(i%,0)

        ' Checks object id or if it is a boss
        if not obj_id% or obj_id% > 50 then continue for
        ' Freeze enemies except crystals
        if g_freeze_timer >= 0 and obj_id%<>20 and obj_id%<>21 then continue for

        data_id%=OBJ_TILE%(obj_id%,4)
        sprite_ix%=OBJ_INI_SPRITE_ID + i%

        ' Moves object
        select case obj_id%
            case 2,3,14 ' Bat
                ' Calculate new X
                select case obj_id%
                    case 2,14
                        if g_obj(i%,5) = right then
                            direction%=SCREEN_CENTER_W+choice(obj_id%=2,0,TILE_SIZE*6)
                            g_obj(i%,1)=SCREEN_WIDTH+direction%*cos(g_obj(i%,4))
                        else
                            direction%=SCREEN_CENTER_W+choice(obj_id%=2,TILE_SIZE,TILE_SIZE*6)
                            g_obj(i%,1)=direction%*cos(g_obj(i%,4))
                        end if
                    case else ' Bat wave
                        g_obj(i%,1)=(SCREEN_CENTER_W-TILE_SIZE)+(SCREEN_CENTER_W-TILE_SIZEx4)*cos(g_obj(i%,4))
                end select

                ' Increment angle by the speed
                inc g_obj(i%,4),OBJ_DATA(data_id%,0)*g_delta_time
                ' Increment Y
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time
                ' Compensates sprite's tile misalignment
                if g_anim_tick% mod 6 < 3 then offset_y%=OBJ_TILE%(obj_id%,3)/2

            case 4 ' Knight
                if g_row%<=0 then
                    ' Check if it is time to change direction
                    if g_obj(i%,5)=0 then
                        if g_player(1)-g_obj(i%,2) < KNIGHT_CHANGE_DIRECT_DIST_PX then
                            g_obj(i%,4)=deg(sprite(V, sprite_ix%, 1))-90
                            g_obj(i%,5)=1
                        else
                            g_obj(i%,4)=90
                        end if
                    end if
                    speed=choice(g_obj(i%,5),60,30)
                    inc g_obj(i%, 1), speed*cos(g_obj(i%,4))*g_delta_time
                    inc g_obj(i%, 2), speed*sin(g_obj(i%,4))*g_delta_time
                else
                    ' Check if it is time to change direction
                    if g_obj(i%,4)=0 and g_player(1)-g_obj(i%,2) < KNIGHT_CHANGE_DIRECT_DIST_PX then
                        g_obj(i%,4)=g_obj(i%,1) ' Save the last X position
                        g_obj(i%,5)=choice(g_obj(i%,1)>SCREEN_CENTER_W,left,right) ' Horizontal movement direction
                    else if g_obj(i%,5) and abs(g_obj(i%,4)-g_obj(i%,1)) > KNIGHT_MAX_HORIZONTAL_DIST_PX then
                        g_obj(i%,5)=0
                    end if

                    if g_obj(i%,5) then
                        inc g_obj(i%,1),OBJ_DATA(4,0)*g_delta_time*g_obj(i%,5)
                    else
                        inc g_obj(i%,2),OBJ_DATA(4,1)*g_delta_time
                    end if
                end if

            case 5 ' Cloud
                ' Attack movement
                if g_obj(i%,3) = 1 then
                    ' Increment X and Y position
                    inc g_obj(i%,1),g_obj(i%,4)*g_delta_time
                    inc g_obj(i%,2),g_obj(i%,5)*g_delta_time

                ' Floating movement
                else
                    if g_obj(i%,2) > SCREEN_HEIGHT/2+TILE_SIZEx2 then
                        if g_obj(i%,1) <=0 or g_obj(i%,1) >= SCREEN_WIDTH-TILE_SIZEx2 then
                            destroy_object(i%)
                            continue for
                        end if
                    else if g_obj(i%,5)=left and g_obj(i%,1) <= TILE_SIZEx2 then
                        g_obj(i%,5)=right
                    else if g_obj(i%,5)=right and g_obj(i%,1) >= SCREEN_WIDTH-TILE_SIZEx4 then
                        g_obj(i%,5)=left
                    end if
                    inc g_obj(i%,1),OBJ_DATA(data_id%,0)*g_delta_time*g_obj(i%,5)

                    if g_obj(i%,4) < 0 then
                        ' Change row
                        inc g_obj(i%,4),10*g_delta_time
                        inc g_obj(i%,2),10*g_delta_time
                    else
                        inc g_obj(i%,2),-TILE_SIZE/2*sin(g_obj(i%,4))
                        inc g_obj(i%,4),1 ' Increment angle
                        inc g_obj(i%,2),TILE_SIZE/2*sin(g_obj(i%,4))
                        ' Prepare for changing line
                        if g_obj(i%,4) mod OBJ_DATA(5,1) = 0 then g_obj(i%,4)=-TILE_SIZE-TILE_SIZE/2
                    end if
                end if

            case 6 ' Blue demon
                ' Check if it is time to attack
                if g_obj(i%,4)=0 and g_player(1)-g_obj(i%,2) < DEMON_CHANGE_DIRECT_DIST_PX then
                    g_obj(i%,4)=choice(g_obj(i%,1)>g_player(0),left,right)
                end if
                inc g_obj(i%,1),OBJ_DATA(data_id%,0)*g_delta_time*g_obj(i%,4)
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time

            case 7 ' Skeleton
                if g_obj(i%,4) > 1 then
                    inc g_obj(i%,5),g_delta_time
                    offset_y%=-TILE_SIZE
                    select case g_obj(i%,5)
                        case is > 3.2
                            destroy_object(i%): continue for
                        case is > 3
                            speed=TILE_SIZE
                        case is > 2.8
                            speed=TILE_SIZE*2
                        case is > 2.6
                            if g_obj(i%,3) = 10 then
                                play_sfx("SKELETON_RESTORED")
                                g_obj(i%,3)=11
                            end if
                            speed=TILE_SIZE*3
                        case is > 0.6
                            speed=TILE_SIZEx4: offset_y%=0
                        case is > 0.4
                            speed=TILE_SIZE*3
                        case is > 0.2
                            speed=TILE_SIZEx2
                        case else
                            speed=TILE_SIZE
                            if g_obj(i%,3) <> 10 then g_obj(i%,3)=10
                    end select
                    offset_x%=speed*choice(g_obj(i%,4)=2,-1,1)

                else if g_obj(i%,5) = 0 then
                    speed=OBJ_DATA(data_id%,0)+OBJ_DATA(data_id%,0)/3*(OBJ_DATA(data_id%,2)-g_obj(i%,3)+1)
                    ' Increment X and Y position
                    inc g_obj(i%,1),speed*g_delta_time*g_obj(i%,4)
                    inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time
                    if g_obj(i%,1) < TILE_SIZEx4 then
                        g_obj(i%,4)=right
                    else if g_obj(i%,1) > SCREEN_WIDTH - TILE_SIZE*6 then
                        g_obj(i%,4)=left
                    end if
                else
                    inc g_obj(i%,5),g_delta_time
                    if g_obj(i%,5) > 4.2 then
                        sprite show safe sprite_ix%, g_obj(i%,1)+offset_x%, g_obj(i%,2)+offset_y%, 1
                        g_obj(i%,5)=0
                    end if
                end if
            case 8 ' Black demon
                ' Check if it is time to attack
                if g_obj(i%,5)=0 and g_player(1)-g_obj(i%,2) < DEMON_ATTACK_DIST_PX then
                    g_obj(i%,5)=1 ' Starts attack timer
                    ' Transform cloud to demon
                    replace_sprite_skin(sprite_ix%, obj_id%, g_obj(i%,1),g_obj(i%,2))
                end if
                if g_obj(i%,5) <= 0 then
                    direction%=choice(g_obj(i%,5) < 0, -1, 1)
                    inc g_obj(i%,1),g_obj(i%,4)*g_delta_time
                    inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time*direction%
                else
                    offset_y%=TILE_SIZE/2 ' Land
                    inc g_obj(i%,5),g_delta_time
                    if g_obj(i%,5) > 2 then
                        g_obj(i%,5)=-1
                        enemy_fire(i%)
                        ' Transform demon to cloud
                        replace_sprite_skin(sprite_ix%, 5, g_obj(i%,1),g_obj(i%,2))
                    end if
                end if

            case 9 ' Death Ghost
                if (g_obj(i%,2) < SCREEN_CENTER_H+TILE_SIZE and g_obj(i%,4) <= 180) or g_obj(i%,4) = 999 then
                    inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time*choice(g_obj(i%,4)=999,-1,1)
                else
                    speed=choice(g_obj(i%,5)=left,-1,1)
                    if speed > 0 then ' right
                        aux=TILE_SIZE*9+cos(g_obj(i%,4)*0.666)*TILE_SIZEx2
                    else ' left
                        aux=TILE_SIZE*9-cos((g_obj(i%,4)+90)*0.666)*TILE_SIZEx2
                    end if
                    g_obj(i%,1)=SCREEN_CENTER_W-TILE_SIZE+aux*cos(g_obj(i%,4)*speed)
                    g_obj(i%,2)=SCREEN_CENTER_H+TILE_SIZE+(SCREEN_CENTER_H-TILE_SIZE*3)*sin(g_obj(i%,4)*speed)
                    inc g_obj(i%,4),0.6
                    if (speed > 0 and g_obj(i%,4) >= 540) or (speed < 0 and g_obj(i%,4) >= 720) then g_obj(i%,4) = 999
                end if

            case 10 ' Zombie
                if abs(g_obj(i%,4))=1000 then
                    offset_x%=g_obj(i%,5)
                    inc g_obj(i%,5),120*g_delta_time*choice(g_obj(i%,4)<0,-1,1)
                    if abs(g_obj(i%,5)) >= 30 then
                        inc g_obj(i%,1),g_obj(i%,5)
                        g_obj(i%,4)=deg(sprite(V, sprite_ix%, 1))-90
                        offset_x%=0
                    end if
                else
                    aux=OBJ_DATA(data_id%,choice(abs(g_obj(i%,5)) >= 30,3,1))
                    inc g_obj(i%,1),aux*g_delta_time*cos(g_obj(i%,4))
                    inc g_obj(i%,2),aux*g_delta_time*sin(g_obj(i%,4))
                end if

            case 11 ' Ghost
                aux=g_obj(i%,1)
                if g_obj(i%,1)<=SCREEN_CENTER_W then
                    g_obj(i%,1)=(SCREEN_CENTER_W-TILE_SIZEx2)*-cos(g_obj(i%,4))
                else
                    g_obj(i%,1)=(SCREEN_WIDTH-TILE_SIZEx2)+(SCREEN_CENTER_W-TILE_SIZEx2)*-cos(g_obj(i%,4))
                end if
                aux=choice(g_obj(i%,1)>aux,right,left)
                if aux<>g_obj(i%,5) then
                    flip%=choice(aux=left,1,0)
                    g_obj(i%,5)=aux
                end if
                ' Move ghost to the other side of the screen
                if g_obj(i%,1) < 0 then
                    g_obj(i%,1)=SCREEN_WIDTH-TILE_SIZEx2
                else if g_obj(i%,1) > SCREEN_WIDTH-TILE_SIZEx2 then
                    g_obj(i%,1)=0
                end if
                ' Increment angle by the speed
                inc g_obj(i%,4),OBJ_DATA(data_id%,0)*g_delta_time
                ' Increment Y
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time

            case 12,16 ' Yellow thing, Red death ghost
                inc g_obj(i%,1),g_obj(i%,4)*g_delta_time
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time

            case 13 ' Red thing
                ' Normal movement
                if g_obj(i%,4) = 0 then
                    ' Start circle movement when moving to the right
                    if g_obj(i%,5) > 0 and g_obj(i%,1)-TILE_SIZE > SCREEN_CENTER_W + TILE_SIZEx4 then
                        g_obj(i%,4)=315
                        g_obj(i%,5)=g_obj(i%,2)+TILE_SIZEx4+TILE_SIZE/2

                    ' Start circle movement when moving to the left
                    else if g_obj(i%,5) < 0 and g_obj(i%,1)+TILE_SIZE < SCREEN_CENTER_W - TILE_SIZE*6 then
                        g_obj(i%,4)=-135
                        g_obj(i%,5)=g_obj(i%,2)+TILE_SIZEx4+TILE_SIZE/2

                    else
                        inc g_obj(i%,1),OBJ_DATA(data_id%,0)*g_delta_time*g_obj(i%,5)
                        inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time
                    end if

                ' Upwards movement
                else if g_obj(i%,4)=999 then
                    inc g_obj(i%,1),OBJ_DATA(data_id%,0)*g_delta_time*g_obj(i%,5)
                    inc g_obj(i%,2),OBJ_DATA(data_id%,1)*-g_delta_time

                ' Circle movement
                else
                    g_obj(i%,1)=SCREEN_CENTER_W-TILE_SIZE+TILE_SIZE*9*cos(g_obj(i%,4))
                    g_obj(i%,2)=g_obj(i%,5)+TILE_SIZE*6*sin(g_obj(i%,4))
                    inc g_obj(i%,4), choice(g_obj(i%,4) < 0,-0.9,0.9)
                    if g_obj(i%,4) < -405 or g_obj(i%,4) >= 585 then
                        g_obj(i%,5)=choice(g_obj(i%,4)>0,1,-1)
                        g_obj(i%,4)=999
                    end if
                end if

            case 20,21 ' Weapon and power-up chrystal
                if g_obj(i%,5) >= 0 then
                    offset_x%=-TILE_SIZEx4*sin(g_obj(i%,5))
                    ' Increment angle by the speed
                    inc g_obj(i%,5),OBJ_DATA(data_id%,0)*g_delta_time
                end if
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time

            case 38,39 ' Shadow
                g_obj(i%,1)=g_obj(g_obj(i%,6),1)
                ' Changes Y if it is not fixed height
                if not g_obj(i%,5) then
                    g_obj(i%,2)=g_obj(g_obj(i%,6),2)
                end if
                offset_x%=g_obj(i%,4) >> 8
                offset_y%=g_obj(i%,4) and &HFF

            case else
                ' Increment X and Y position
                inc g_obj(i%,1),OBJ_DATA(data_id%,0)*g_delta_time
                inc g_obj(i%,2),OBJ_DATA(data_id%,1)*g_delta_time
        end select

        ' Move or destroy the sprite if out of bounds
        if sprite(e,sprite_ix%) = 0 and g_obj(i%,2) <= SCREEN_HEIGHT+TILE_SIZE then
            if flip%>=0 then
                sprite show safe sprite_ix%, g_obj(i%,1)+offset_x%, g_obj(i%,2)+offset_y%, sprite(L, sprite_ix%), flip%
            else
                sprite next sprite_ix%, g_obj(i%,1)+offset_x%, g_obj(i%,2)+offset_y%
            end if
        else if obj_id% = 39 then ' Shadow
            destroy_shadow(g_obj(i%,6))
        else
            destroy_object(i%)
        end if
    next
end sub

sub start_fire_animation(x%, y%, sfx%)
    enqueue_action(1, 34, x%, y%, sfx%)
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
        if g_shots(i%,0) > 0 then continue for
        get_free_shot_slot=i%
        exit for
    next
end function

sub destroy_enemies_shots()
    local i%
    ' 0 to 2 is reserved for player shots
    for i%=3 to bound(g_shots())
        if g_shots(i%, 0) > 0 then destroy_shot(i% + SHOTS_INI_SPRITE_ID)
    next
end sub

sub destroy_shot(sprite_id%, animate%)
    local i%=sprite_id% - SHOTS_INI_SPRITE_ID
    sprite hide safe sprite_id%
    sprite close sprite_id%
    g_shots(i%, 0) = 0
    if animate% then start_fire_animation(g_shots(i%,1), g_shots(i%,2), true)
end sub

sub destroy_object(id%)
    local i%,obj_id%=g_obj(id%, 0)
    if id% < 0 or obj_id% <= 0 then exit sub

    local sprite_id%=OBJ_INI_SPRITE_ID + id%
    destroy_shadow(id%)

    if sprite(X, sprite_id%) <> 10000 then sprite hide safe sprite_id%
    sprite close sprite_id%
    if is_enemy(obj_id%) then
        inc g_enemies_count%, -1
        if g_boss(0) > 1 then boss_enemy_destroyed(obj_id%, g_obj(id%, 4), g_obj(id%, 5))
    end if
    ' Clear object data
    for i%=0 to 5
        g_obj(id%,i%)=0
    next
end sub

sub destroy_shadow(src_obj_ix%)
    local shadow_ix%=g_obj(src_obj_ix%,6)
    if shadow_ix% < 0 then exit sub

    local sprite_id%=OBJ_INI_SPRITE_ID + shadow_ix%
    g_obj(src_obj_ix%,6) = -1
    g_obj(shadow_ix%, 0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
end sub

sub destroy_all()
    local i%
    ' Destroy objects
    for i%=0 to bound(g_obj())
        if g_obj(i%,0) > 0 then destroy_object(i%)
    next
    ' Destroy shots
    for i%=0 to bound(g_shots())
        if g_shots(i%, 0) > 0 then destroy_shot(i% + SHOTS_INI_SPRITE_ID)
    next
    ' Free actions queue
    for i%=0 to bound(g_actions_queue())
        g_actions_queue(i%, 0) = 0
    next
    sprite close all
end sub

sub kill_all_enemies(sfx%)
    local i%,hit
    if sfx% then play_sfx("KILL_ALL_ENEMIES")
    for i%=0 to bound(g_obj())
        ' Check for free slots or boss id
        if g_obj(i%,0)=0 or g_obj(i%,0) > 50 then continue for
        hit=hit_object(OBJ_INI_SPRITE_ID + i%, false, true)
    next
end sub

sub kill_player()
    g_scroll_on=false
    play_song("SILENCE_MOD")
    play_sfx("PLAYER_DEATH")
    init_player(g_player(7) - 1)
    g_player(8)=4 ' Dead state
    check_shield()
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

sub replace_sprite_skin(sprite_id%, obj_id%, x%, y%)
    sprite hide safe sprite_id%
    sprite close sprite_id%
    sprite read sprite_id%, OBJ_TILE%(obj_id%,0), OBJ_TILE%(obj_id%,1), OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, x%, y%, 1,, 1
end sub

sub spawn_object(obj_id%, x%, y%, obj_cfg%)
    local i%=allocate_object(obj_id%, x%, y%)
    if i% < 0 then exit sub

    local sprite_id%=OBJ_INI_SPRITE_ID + i%, spawn_speed
    local offset_x%, offset_y%, layer%=1, data_id%, angle
    data_id%=OBJ_TILE%(obj_id%,4)

    g_obj(i%,3)=OBJ_DATA(data_id%,2) ' GPR 1 / Life
    g_obj(i%,4)=OBJ_DATA(data_id%,3) ' GPR 2
    g_obj(i%,5)=OBJ_DATA(data_id%,4) ' GPR 3

    select case obj_id%
        case 2,3,14 ' Bat
            spawn_speed=choice(obj_id%=2, 500, 200)
            ' Calculate the correct bat angle
            if x% < SCREEN_CENTER_W then
                g_obj(i%,4)=g_obj(i%,4)+180
                g_obj(i%,5)=left
            else
                g_obj(i%,5)=right
            end if
            ' If bat reverse
            if obj_id%=14 then
                g_obj(i%,2)=SCREEN_HEIGHT-TILE_SIZEx4
                spawn_speed=1000
            end if
            ' Spawn the bat shadow
            spawn_shadow(i%,,TILE_SIZEx2+TILE_SIZE/2)

        case 4 ' Knight
            spawn_speed=500

        case 5 ' Cloud
            g_obj(i%,5)=choice(x% < SCREEN_CENTER_W, left, right)
            g_obj(i%,1)=choice(g_obj(i%,5)=left, 0, SCREEN_WIDTH-TILE_SIZEx2)
            g_obj(i%,2)=TILE_SIZEx4
            spawn_shadow(i%,,TILE_SIZEx2)

        case 6 ' Blue demon
            spawn_shadow(i%,,TILE_SIZEx2+TILE_SIZE/2)
            spawn_speed=400

        case 7 ' Skeleton
            select case obj_cfg%
                case 1 ' Body flying to the left
                    offset_x%=TILE_SIZEx4
                    g_obj(i%,4)=2

                case 2 ' Body flying to the right
                    offset_x%=TILE_SIZE*6
                    g_obj(i%,4)=3

                case else
                    if x% < SCREEN_CENTER_W then
                        g_obj(i%,1)=TILE_SIZEx4
                        g_obj(i%,4)=right
                    else
                        g_obj(i%,1)=SCREEN_WIDTH-TILE_SIZE*6
                        g_obj(i%,4)=left
                    end if
                end select

        case 8,12 ' Black demon, Yellow thing
            angle=min(20,math(ATAN3 abs(g_obj(i%,1)-g_player(0)), abs(g_obj(i%,2)-g_player(1))))
            g_obj(i%,4)=OBJ_DATA(data_id%,0)*choice(g_obj(i%,1)>g_player(0),left,right)*sin(angle)
            spawn_shadow(i%,,TILE_SIZEx2)
            if obj_id%=8 then obj_id%=5 ' Spawns as a cloud

        case 9 ' Death Ghost
            spawn_speed=300
            if x% < SCREEN_CENTER_W then
                g_obj(i%,1)=TILE_SIZEx4
                g_obj(i%,4)=180
                g_obj(i%,5)=left
            else
                g_obj(i%,1)=SCREEN_WIDTH-TILE_SIZEx4-TILE_SIZEx2
                g_obj(i%,4)=1
                g_obj(i%,5)=right
            end if

        case 10 ' Zombie
            if obj_cfg% then
                offset_x%=TILE_SIZEx2
                g_obj(i%,4)=obj_cfg%
            else
                g_obj(i%,4)=atan2(sprite(Y,1)-y%, sprite(X,1)-x%)
            end if

        case 11 ' Ghost
            spawn_speed=400
            if x% < SCREEN_CENTER_W then
                g_obj(i%,1)=0
                g_obj(i%,4)=90
            else
                g_obj(i%,1)=SCREEN_WIDTH
                g_obj(i%,4)=270
            end if
            ' Spawn the ghost shadow
            spawn_shadow(i%,,TILE_SIZEx2+TILE_SIZE/2)

        case 13 ' Red thing
            g_obj(i%,1)=choice(g_obj(i%,1) < SCREEN_CENTER_W, 0, SCREEN_WIDTH-TILE_SIZEx2)
            g_obj(i%,2)=TILE_SIZEx2
            g_obj(i%,5)=choice(g_obj(i%,1)=0, 1, -1)
            spawn_shadow(i%,,TILE_SIZEx2+TILE_SIZE/2)
            spawn_speed=300

        case 16 ' Red death ghost
            angle=math(ATAN3 g_obj(i%,1)-g_player(0), g_obj(i%,2)-g_player(1)-200)
            g_obj(i%,4)=OBJ_DATA(data_id%,0)*sin(-angle)

        case 31 ' Fire CHECK
            offset_x%=FIRE_ANIM(0)*TILE_SIZEx2
            layer%=4
            if obj_cfg% then play_sfx("ENEMY_KILL")

    end select

    if obj_cfg% > 0 and spawn_speed > 0 then enqueue_action(obj_cfg%, obj_id%, x%, y%,,spawn_speed)

    ' Enemies range
    if is_enemy(obj_id%) then inc g_enemies_count%

    sprite read sprite_id%, OBJ_TILE%(obj_id%,0)+offset_x%, OBJ_TILE%(obj_id%,1)+offset_y%, OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), layer%,, 1
end sub

sub spawn_shield()
    sprite read 2, OBJ_TILE%(36,0), OBJ_TILE%(36,1), OBJ_TILE%(36,2), OBJ_TILE%(36,3), OBJ_TILES_BUFFER
    if sprite(X, 2) = 10000 then sprite show 2, g_player(0),g_player(1)-TILE_SIZE, 1
    g_player(5)=1
    g_player(6)=SHIELD_MAX_HITS
end sub

sub spawn_shadow(obj_ix%, offset_x%, offset_y%, fixed_y%, shadow_tile_id%)
    local i%=get_free_object_slot()
    if i% < 0 then exit sub

    local sprite_id%, id%=choice(shadow_tile_id%, shadow_tile_id%, 39)

    g_obj(i%,0)=39                            ' Shadow id
    g_obj(i%,1)=g_obj(obj_ix%,1)+offset_x%    ' X
    g_obj(i%,2)=g_obj(obj_ix%,2)+offset_y%    ' Y
    g_obj(i%,4)=(offset_x% << 8) or offset_y% ' Offset X or Offset Y
    g_obj(i%,5)=fixed_y%                      ' Is fixed hight?
    g_obj(i%,6)=obj_ix%                       ' Shadow -> source object index
    g_obj(obj_ix%,6)=i%                       ' Source object -> shadow index
    sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, OBJ_TILE%(id%,0), OBJ_TILE%(id%,1), OBJ_TILE%(id%,2), OBJ_TILE%(id%,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), 3
end sub

sub fire()
    ' Cooldown or invincibility power-up
    if timer - g_pshot_timer < g_player_shot_ms or g_player(5) = 4 then exit sub

    local i%, sprite_id%, shots%=choice(g_player(3)>6,2,1)
    local tile_id%=40
    local speed_x=0, speed_y=-220, offset%
    local sfx$="SHOT"
    local x=g_player(0)+(TILE_SIZE-OBJ_TILE%(tile_id%,2)/2)
    local y=g_player(1)-OBJ_TILE%(tile_id%,3)-1

    g_fire%=true
    select case g_player(3)
        case 2 ' Twin arrows
            tile_id%=41
        case 3,8 ' Triple flames
            if g_shots(0,0) or g_shots(1,0) or g_shots(2,0) then exit sub
            shots%=2
            inc x, -TILE_SIZEx2
            speed_y=-200
            tile_id%=42
            sfx$="FLAME"
        case 4,9 ' Boomerang
            tile_id%=43
            sfx$=""
            g_cont_sfx$="BOOMERANG"
        case 5,10 ' Sword
            shots%=2
            tile_id%=choice(g_player(3) = 5, 45, 46)
            sfx$="SWORD"
        case 6,11 ' Fire arrow
            tile_id%=47
            sfx$="FIRE_ARROW"
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
    local i%

    for i%=0 to bound(g_obj())
        ' Has object or is in the min Y position?
        if g_obj(i%,0)=0 or g_obj(i%,0)>20 or (g_stage%<5 and g_obj(i%,2)<TILE_SIZEx2) then continue for

        ' If it is not a black demon
        if g_obj(i%,0) <> 8 then
            enemy_fire(i%)
        end if
    next
end sub

sub enemy_fire(ix%, disable_rnd%)
    local obj_id%=g_obj(ix%,0)
    ' Checks screen limit for shooting and shoot chance
    if disable_rnd%=false and rnd > SHOOT_CHANCE(g_difficulty%,g_stage%) then exit sub
    if g_stage% < 6 and g_obj(ix%,2) > MAX_ENEMIES_SHOOT_Y then exit sub
    ' Ignores boss body
    if obj_id% > 50 and g_boss(1) = obj_id% then exit sub

    ' Allocate new shot
    local shot_ix%=get_free_shot_slot()
    if shot_ix% < 0 then exit sub

    local enemy_spr_id%=OBJ_INI_SPRITE_ID + ix%
    local x=g_obj(ix%,1) + sprite(W, enemy_spr_id%)/2
    local y=g_obj(ix%,2) + sprite(H, enemy_spr_id%)/2
    local angle=deg(sprite(V, enemy_spr_id%, 1))
    local rot%, weapon_ix%=23 ' Dot bullet
    local i%, gpr1, gpr2, gpr3, offset_x%, speed=70

    select case obj_id%
        case 5 ' Cloud
            exit sub ' Do not shoot

        case 4, 57 ' Knight, boss stage 07
            if obj_id%=57 then speed=100
            ' Arrow weapon_ix%
            select case angle
                case  23 to  67 ' Up Right
                    weapon_ix%=31: rot%=3
                    inc x,TILE_SIZE
                    inc y,-TILE_SIZE*2.5
                case 292 to 337 ' Up Left
                    weapon_ix%=31: rot%=2
                    inc x,-TILE_SIZEx2
                    inc y,-TILE_SIZE*2.5
                case 112 to 157 ' Down Right
                    weapon_ix%=31: rot%=1
                    inc x,TILE_SIZE
                    inc y,TILE_SIZE
                case 202 to 247 ' Down Left
                    weapon_ix%=31
                    inc x,-TILE_SIZEx2
                    inc y,TILE_SIZE
                case  67 to 112 ' Right
                    weapon_ix%=30: rot%=1
                    inc x,TILE_SIZE
                case 247 to 292 ' Left
                    weapon_ix%=30
                    inc x,-TILE_SIZE*2.5
                case 157 to 202 ' Down
                    weapon_ix%=29
                    inc y,TILE_SIZE
                case else       ' Up
                    weapon_ix%=29: rot%=2
                    inc y,-TILE_SIZE*2.5
            end select

        case 7 ' Skeleton
            if g_obj(ix%,5) <> 0 or g_obj(ix%,4) > 1 then exit sub
            weapon_ix%=25

        case 8 ' Black Demon
            weapon_ix%=24 ' Energy ray
            speed=speed*1.3
            create_shot(shot_ix%            , weapon_ix%, x, y, speed*sin(angle-30), speed*-cos(angle-30))
            create_shot(get_free_shot_slot(), weapon_ix%, x, y, speed*sin(angle-10), speed*-cos(angle-10))
            create_shot(get_free_shot_slot(), weapon_ix%, x, y, speed*sin(angle+10), speed*-cos(angle+10))
            create_shot(get_free_shot_slot(), weapon_ix%, x, y, speed*sin(angle+30), speed*-cos(angle+30))
            exit sub
        case 9,16 ' Death Ghost, Red Death Ghost
            weapon_ix%=24 ' Energy ray
            speed=100

        case 12, 13 ' Yellow thing and Red thing
            if g_stage% > 2 and g_row% > MAP_ROWS \ 2 then exit sub

        case 15 ' Sorcerer
            for i%=45 to 315 step 45
                if i% <> 180 then create_shot(shot_ix%, weapon_ix%, x, y, speed*-cos(i%), speed*sin(i%))
                if i% < 315 then shot_ix%=get_free_shot_slot()
            next
            exit sub

        case 51 ' Boss stage 1
            weapon_ix%=24 ' Energy ray
            speed=120

        case 52 ' Boss stage 2
            weapon_ix%=33 ' Scythe
            ' Center of rotation
            gpr1=g_obj(ix%,1)             ' X
            gpr2=g_obj(ix%,2)+TILE_SIZE*8 ' Y

        case 53 ' Boss stage 3
            weapon_ix%=28 ' White explosion
            angle=min(200,max(160,angle))
            speed=110
            for i%=-60 to 60 step 20
                create_shot(shot_ix%, weapon_ix%, x, y, speed*sin(angle-i%), speed*-cos(angle-i%))
                if i% < 60 then shot_ix%=get_free_shot_slot()
            next
            exit sub

        case 54, 56 ' Boss stage 4 and 6
            weapon_ix%=28 ' White explosion
            speed=110
            angle=max(min(210, angle), 150)
            gpr1=speed*sin(angle)
            gpr2=speed*-cos(angle)

            if obj_id% = 54 then
                offset_x%=TILE_SIZEx2 ' Red explosion
                gpr3=g_player(1)
            end if

        case 55 ' Boss stage 5
            weapon_ix%=32 ' Axe
            gpr2=100      ' Y speed
            x=g_obj(ix%,1)-TILE_SIZE
            create_shot(get_free_shot_slot(), weapon_ix%, x, y, gpr1, gpr2, gpr3, rot%, offset_x%)
            inc x, TILE_SIZE*6
            shot_ix%=get_free_shot_slot()
    end select

    if gpr1 = 0 and gpr2 = 0 then
        gpr1=speed*sin(angle)
        gpr2=speed*-cos(angle)
    end if
    create_shot(shot_ix%, weapon_ix%, x, y, gpr1, gpr2, gpr3, rot%, offset_x%)
end sub

sub create_shot(shot_ix%, weapon_ix%, x, y, gpr1, gpr2, gpr3, rot%, offset_x%)
    if shot_ix% < 0 then exit sub
    local shot_spr_id%=shot_ix%+SHOTS_INI_SPRITE_ID
    ' Create the shot state
    g_shots(shot_ix%,0)=weapon_ix% ' Weapon Index
    g_shots(shot_ix%,1)=x          ' X
    g_shots(shot_ix%,2)=y          ' Y
    g_shots(shot_ix%,3)=gpr1
    g_shots(shot_ix%,4)=gpr2
    g_shots(shot_ix%,5)=gpr3
    ' Create the shot sprite
    sprite read shot_spr_id%, OBJ_TILE%(weapon_ix%,0)+offset_x%, OBJ_TILE%(weapon_ix%,1), OBJ_TILE%(weapon_ix%,2), OBJ_TILE%(weapon_ix%,3), OBJ_TILES_BUFFER
    sprite show safe shot_spr_id%, g_shots(shot_ix%,1),g_shots(shot_ix%,2), 1, rot%
end sub

function is_enemy(obj_id%) as integer
    is_enemy = (obj_id% > 0 and obj_id% < 20)
end function

sub check_shield()
    select case g_player(6)
        case is <= 0
            g_player(5)=0
            g_player(6)=0
            if sprite(x, 2) <> 10000 then sprite hide safe 2
        case is <= SHIELD_MAX_HITS\6
            sprite read 2, OBJ_TILE%(36,0)+TILE_SIZEx4, OBJ_TILE%(36,1), OBJ_TILE%(36,2), OBJ_TILE%(36,3), OBJ_TILES_BUFFER
        case is <= SHIELD_MAX_HITS\6*2
            sprite read 2, OBJ_TILE%(36,0)+TILE_SIZEx2, OBJ_TILE%(36,1), OBJ_TILE%(36,2), OBJ_TILE%(36,3), OBJ_TILES_BUFFER
    end select
end sub

