sub activate_boss()
    local id%=g_boss(1)
    g_boss(0)=2
    g_obj(id%, 2)=TILE_SIZE*6
    activate_hit_box(true)

    select case g_stage%
        case 1
            ' Enqueue clouds spawning
            enqueue_action(1, 5,,,right,3000)
            enqueue_action(1, 5,,,left,6500)
            enqueue_action(1, 5,,,right,9500)
        case 2,3
            g_obj(g_obj(id%,6),5) = true ' Fix shadow hight

    end select
end sub

sub activate_hit_box(active%)
    local sprite_id%, i%
    for i%=2 to 7
        if g_boss(i%) < 0 then continue for
        sprite_id%=OBJ_INI_SPRITE_ID + g_boss(i%)
        if sprite(X, sprite_id%) = 10000 and active% then
            sprite show sprite_id%, g_obj(g_boss(1), 1)+TILE_SIZEx2,g_obj(g_boss(1), 2)+TILE_SIZE, 1
        else if sprite(X, sprite_id%) <> 10000 and not active% then
            sprite hide safe sprite_id%
        end if
    next
end sub

sub boss_enemy_destroyed(obj_id%, gpr1, gpr2)
    if g_boss(0) < 2 then exit sub
    select case g_stage%
        case 1
            ' Enqueue cloud spawning
            enqueue_action(1, 5,,,gpr2,fix(rnd()*2000+3000))
    end select
end sub

function is_boss_hit_box(sprite_id%) as integer
    local obj_id%=sprite_id% - OBJ_INI_SPRITE_ID
    select case g_stage%
        case 7
            is_boss_hit_box=false
        case else
            is_boss_hit_box=(g_boss(0)> 1 and g_boss(2)=obj_id%)
    end select
end function

sub hit_boss(sprite_id%)
    local obj_id%=g_boss(1)
    local sprite_obj_id%=sprite_id% - OBJ_INI_SPRITE_ID

    ' Hit the body, not the hit box
    if sprite_obj_id% = obj_id% or g_boss(0) < 2 then exit sub

    ' Decrement hit box life
    inc g_obj(sprite_obj_id%,3), choice(is_super_weapon(), -2, -1)
    local hit_box_life%= g_obj(sprite_obj_id%,3)
    ' Decrement boss life
    if hit_box_life% <= 0 then inc g_obj(obj_id%,3), -1

    ' Replace boss skin
    local aux%=BOSS_LIFE(g_stage%) \ 3
    select case g_stage%
        case 2
            if hit_box_life% mod aux% = 0 then
                enqueue_action(1, 90, obj_id%, choice(hit_box_life%\aux%<=1,TILE_SIZE*10,TILE_SIZE*5))
            end if
        case 3
            if hit_box_life% = aux% or hit_box_life% = aux%*2 then inc g_boss(0)
    end select

    if g_obj(obj_id%,3) <= 0 then
        kill_boss()
    else
        play_sfx("BOSS_HIT_SFX")
    end if
end sub

sub kill_boss()
    g_boss(0)=1    ' Deactivate boss
    g_player(8)=2  ' Freeze player
    increment_score(10000)
    clear_queue()
    destroy_enemies_shots()
    kill_all_enemies()
    animate_boss() ' Animates boss to initial state
    play_song("SILENCE_MOD")
    play_sfx("BOSS_KILL_SFX")

    ' Blink screen 30 times
    enqueue_action(30, 100,,,,40)
end sub

sub destroy_boss()
    local i%,x%,y%

    ' Destroy boss objects
    for i%=1 to 7
        if g_boss(i%) >= 0 then destroy_object(g_boss(i%))
    next
    g_boss(0)=0
    x%=g_obj(g_boss(1),1)
    y%=g_obj(g_boss(1),2)

    ' Spawn fire
    enqueue_action(1, 27, x%+TILE_SIZE*2, y%+TILE_SIZE, true)
    enqueue_action(1, 27, x%, y%+TILE_SIZE*2, true)
    enqueue_action(1, 27, x%+TILE_SIZE*4, y%+TILE_SIZE*3, true)
    enqueue_action(1, 27, x%+TILE_SIZE*5, y%+TILE_SIZE*4, true)
    enqueue_action(1, 27, x%+TILE_SIZE, y%+TILE_SIZE*5, true)
    ' Enqueue player animation
    enqueue_action(1, 101,,,,1500)
end sub

sub spawn_boss(off_screen%)
    local i%, y%=choice(off_screen%, -TILE_SIZE*3, TILE_SIZEx4)
    local obj_id%=allocate_object(50+g_stage%, TILE_SIZE*14, y%, 1, 0, TILE_SIZE*14)
    local sprite_id%=OBJ_INI_SPRITE_ID + obj_id%

    g_boss(0)=1       ' Status - Waiting start
    g_boss(1)=obj_id% ' Body Object Id
    for i%=2 to 7
        g_boss(i%)=-1 ' Hit box
    next

    if g_stage% = 8 then
        ' Final boss
    else
        show_boss_body(obj_id%,0,true)
        ' Hit box
        g_boss(2)=allocate_object(50+g_stage%, g_obj(obj_id%, 1)+TILE_SIZEx2, g_obj(obj_id%, 2)+TILE_SIZE, BOSS_LIFE(g_stage%))
        sprite read OBJ_INI_SPRITE_ID + g_boss(2), TRANSPARENT_BLOCK_X, TRANSPARENT_BLOCK_Y, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
    end if

    ' Boss body
    sprite show sprite_id%, g_obj(obj_id%, 1),g_obj(obj_id%, 2), 2
end sub

sub show_boss_body(obj_id%, tile_offset%, show_shadow%)
    local tile_x%, tile_y%, tile_w%=TILE_SIZE*5, tile_h%=TILE_SIZE*5

    select case g_stage%
        case 2
            tile_x%=80
            if show_shadow% then spawn_shadow(obj_id%,,TILE_SIZE*6, false, 37)

        case 3
            tile_x%=200
            tile_h%=TILE_SIZE*4
            if show_shadow% then spawn_shadow(obj_id%, TILE_SIZE+TILE_SIZE/2, choice(g_row%=0,TILE_SIZE*4+TILE_SIZE/2,TILE_SIZE*7), false)

    end select

    sprite read OBJ_INI_SPRITE_ID + obj_id%, tile_x%+tile_offset%, tile_y%, tile_w%, tile_h%, BOSS_TILES_BUFFER
end sub

sub create_portal()
    local i%=allocate_object(50, SCREEN_WIDTH/2-TILE_SIZEx2, TILE_SIZE)
    local sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, g_obj(i%,1),g_obj(i%,2), TILE_SIZEx4, TILE_SIZE*4-2, SCREEN_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2),1,,1
end sub

sub boss_fire()
    local speed
    select case g_stage%
        case 1
            speed=70
        case 2
            speed=120
    end select
    if g_obj(g_boss(1), 4) mod 70 = 0 then enemy_fire(g_boss(1))
end sub

sub animate_boss()
    local tick%, x%,y%,w%=TILE_SIZE*5,h%

    select case g_stage%
        case 1
            h%=TILE_SIZE*5
            tick% = g_anim_tick% mod 8
            if tick% = 4 and g_boss(0) <> 1 then x%=TILE_SIZE*5

        case 3
            local hit_box%=OBJ_INI_SPRITE_ID + g_boss(2)
            x%=choice(g_boss(0)=3,80,200)
            y%=choice(g_boss(0)=2,TILE_SIZE,TILE_SIZE*6)
            h%=TILE_SIZE*4
            tick% = g_anim_tick% mod 48
            if tick% < 24 then
                activate_hit_box(false)
            else
                activate_hit_box(true)
                tick% = g_anim_tick% mod 4
                if tick% < 2 or g_boss(0) = 1 then
                    inc x%,TILE_SIZE*5
                else
                    inc x%,TILE_SIZE*10
                end if
            end if

        case else
            exit sub
    end select

    sprite read OBJ_INI_SPRITE_ID + g_boss(1), x%,y%, w%,h%, BOSS_TILES_BUFFER
end sub

sub move_boss()
    local out_bounds%, id%=g_boss(1)
    local ini_pos, speed_x, speed_y, mov_height

    if g_boss(0) > 1 then
        select case g_stage%
            case 1
                inc g_obj(id%, 4) ' Boss tick
                if fix(g_obj(id%, 1)) = fix(g_obj(id%, 5)) then
                    if g_obj(id%, 4) mod 150 = 0 then
                        g_obj(id%, 5)=min(max(g_player(0)-TILE_SIZEx2, TILE_SIZEx4),SCREEN_WIDTH-TILE_SIZE*9)
                    end if
                else
                    inc g_obj(id%, 1), choice(g_obj(id%, 1) > g_obj(id%, 5), -1, 1)*BOSS_SPEED*g_delta_time
                end if

            case 2, 3
                select case g_stage%
                    case 2
                        speed_x=1.2: speed_y=9
                        mov_height=6
                    case 3
                        speed_x=1.8: speed_y=3.6
                        mov_height=12
                end select

                ini_pos=(SCREEN_WIDTH-TILE_SIZE*6)/2
                inc g_obj(id%, 4),BOSS_SPEED*g_delta_time*speed_x
                inc g_obj(id%, 5),BOSS_SPEED*g_delta_time*speed_y
                g_obj(id%, 1)=ini_pos+(ini_pos-TILE_SIZEx4)*sin(g_obj(id%, 4))
                g_obj(id%, 2)=TILE_SIZE*6+mov_height*cos(g_obj(id%, 5))

        end select

        ' Boss hit box
        sprite next OBJ_INI_SPRITE_ID + g_boss(2), g_obj(id%, 1)+TILE_SIZEx2,g_obj(id%, 2)+TILE_SIZE
    end if

    ' Boss body
    sprite next OBJ_INI_SPRITE_ID + g_boss(1), g_obj(id%, 1), g_obj(id%, 2)
end sub
