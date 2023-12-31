sub activate_boss()
    local sprite_id%, i%, id%=g_boss(1)
    g_boss(0)=2

    for i%=2 to 7
        if g_boss(i%) >= 0 then
            sprite_id%=OBJ_INI_SPRITE_ID + g_boss(i%)
            sprite show sprite_id%, g_obj(g_boss(1), 1)+TILE_SIZEx2,g_obj(g_boss(1), 2)+TILE_SIZE, 1
        end if
    next
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
    local id%=g_boss(1)
    local sprite_obj_id%=sprite_id% - OBJ_INI_SPRITE_ID

    ' Hit the body, not the hit box
    if sprite_obj_id% = id% then exit sub

    select case g_stage%
        case 7
        case else
            ' Decrement hit box life
            inc g_obj(sprite_obj_id%,3), choice(is_super_weapon(), -2, -5)
            ' Decrement boss life
            if g_obj(sprite_obj_id%,3) <= 0 then inc g_obj(id%,3), -1

            if g_obj(id%,3) <= 0 then
                increment_score(10000)
                destroy_enemies_shots()
                kill_all_enemies()
                kill_boss()
            else
                play_sfx("BOSS_HIT_SFX")
            end if
    end select
end sub

sub kill_boss()
    local i%,x%,y%
    g_boss(0)=0
    x%=g_obj(g_boss(1),1)
    y%=g_obj(g_boss(1),2)
    for i%=1 to 7
        if g_boss(i%) >= 0 then
            destroy_object(g_boss(i%))
        end if
    next
    play_song("SILENCE_MOD")
    play_sfx("BOSS_KILL_SFX")

    ' Spawn fire
    enqueue_action(1, 24, x%+TILE_SIZE*2, y%+TILE_SIZE)
    enqueue_action(1, 24, x%, y%+TILE_SIZE*2)
    enqueue_action(1, 24, x%+TILE_SIZE*4, y%+TILE_SIZE*3)
    enqueue_action(1, 24, x%+TILE_SIZE*5, y%+TILE_SIZE*4)
    enqueue_action(1, 24, x%+TILE_SIZE, y%+TILE_SIZE*5)
end sub

sub spawn_boss(stage%, off_screen%)
    local i%, y%=choice(off_screen%, -TILE_SIZE*3, TILE_SIZEx4)
    local id%=allocate_object(50+stage%, TILE_SIZE*14, y%, 1, 0, TILE_SIZE*14)
    local sprite_id%=OBJ_INI_SPRITE_ID + id%

    g_boss(0)=1
    g_boss(1)=id%
    for i%=2 to 7
        g_boss(i%)=-1
    next

    select case stage%
        case else
            ' Boss body
            sprite read sprite_id%, 0, 0, TILE_SIZE*5, TILE_SIZE*5, BOSS_TILES_BUFFER
            ' Hit box
            g_boss(2)=allocate_object(50+stage%, g_obj(id%, 1)+TILE_SIZEx2, g_obj(id%, 2)+TILE_SIZE, BOSS_LIFE)
            sprite read OBJ_INI_SPRITE_ID + g_boss(2), TRANSPARENT_BLOCK_X, TRANSPARENT_BLOCK_Y, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
    end select
    ' Boss body
    sprite show sprite_id%, g_obj(id%, 1),g_obj(id%, 2), 2
end sub

sub boss_fire()
    select case g_stage%
        case 1
            if g_obj(g_boss(1), 4) mod BOSS_FIRE_SPEED = 0 then
                enemy_fire(g_boss(1))
            end if
    end select
end sub

sub animate_boss()
    local tick%

    select case g_stage%
        case 1
            tick% = g_anim_tick% mod 8
            if tick% = 0 then
                sprite read OBJ_INI_SPRITE_ID + g_boss(1), 0, 0, TILE_SIZE*5, TILE_SIZE*5, BOSS_TILES_BUFFER
            else if tick% = 4 then
                sprite read OBJ_INI_SPRITE_ID + g_boss(1), TILE_SIZE*5, 0, TILE_SIZE*5, TILE_SIZE*5, BOSS_TILES_BUFFER
            end if
    end select
end sub

sub move_boss()
    local out_bounds%, id%=g_boss(1)
    if g_boss(0) > 1 then
        select case g_stage%
            case 1
                inc g_obj(id%, 4) ' Boss tick
                if fix(g_obj(id%, 1)) = fix(g_obj(id%, 5)) then
                    if g_obj(id%, 4) mod 120 = 0 then
                        g_obj(id%, 5)=min(max(g_player(0)-TILE_SIZEx2, TILE_SIZEx4),SCREEN_WIDTH-TILE_SIZE*9)
                    end if
                else
                    inc g_obj(id%, 1), choice(g_obj(id%, 1) > g_obj(id%, 5), -1, 1)*BOSS_SPEED*g_delta_time
                end if
                ' hit box
                sprite next OBJ_INI_SPRITE_ID + g_boss(2), g_obj(id%, 1)+TILE_SIZEx2,g_obj(id%, 2)+TILE_SIZE
        end select
    end if

    sprite next OBJ_INI_SPRITE_ID + g_boss(1), g_obj(id%, 1), g_obj(id%, 2)
end sub
