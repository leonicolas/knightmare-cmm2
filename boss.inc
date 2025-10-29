sub activate_boss()
    local id%=g_boss(1), shadow_id%=g_obj(id%,6), i%
    g_boss(0)=2
    if shadow_id% > 0 then
        g_obj(shadow_id%,2) = g_obj(id%, 2)
        g_obj(shadow_id%,5) = true ' Fix shadow height
    end if

    for i%=2 to 7
        activate_hit_box(i%, true)
    next

    select case g_stage%
        case 1,8
            ' Enqueue clouds spawning
            enqueue_action(1, 5,,,right,3000)
            enqueue_action(1, 5,,,left,6500)
            enqueue_action(1, 5,,,right,9500)
        case 3
            activate_hit_box(2, false)
            g_obj(g_obj(id%,6),5) = true ' Fix shadow height
        case 5
            activate_shield(true)
            activate_hit_box(2, false)
        case 7
            ' Enqueue knight spawning
            enqueue_action(1, 4,SCREEN_WIDTH/2-TILE_SIZE,TILE_SIZE*5,,4000)
            enqueue_action(1, 4,SCREEN_WIDTH/2-TILE_SIZE,TILE_SIZE*5,,8000)
    end select
end sub

sub boss_enemy_destroyed(obj_id%, gpr1, gpr2)
    select case g_stage%
        case 1
            ' Enqueue cloud spawning
            enqueue_action(1, 5,,,gpr2,fix(rnd()*6000+2000))
        case 7
            ' Enqueue knight spawning
            if g_enemies_count% < 2 then
                enqueue_action(1, 4,SCREEN_WIDTH/2-TILE_SIZE,TILE_SIZE*5,,fix(rnd()*5000+2000))
            end if
    end select
end sub

' Return true if the hit destroys the shot
function hit_boss(sprite_id%) as integer
    hit_boss = false

    ' Boss inactive
    if g_boss(0) < 2 then exit function

    local i%,obj_id%=g_boss(1)
    local hit_box_id%=sprite_id%-OBJ_INI_SPRITE_ID

    ' Invulnerable
    if g_obj(hit_box_id%,3) > 1000 then exit function


    ' Verifies if was blocked
    select case g_stage%
        case 3
            if sprite(X, OBJ_INI_SPRITE_ID + g_boss(3)) <> 10000 then
                play_sfx("BOSS_SHIELD")
                hit_boss = true
                exit function
            end if
        case 5
            if g_obj(obj_id%, 6) = 0 then
                play_sfx("BOSS_SHIELD")
                hit_boss = true
                exit function
            end if
    end select

    hit_boss = true

    ' Decrement hit box life
    if g_obj(hit_box_id%,3) > 0 and g_obj(hit_box_id%,3) < 1000 then
        inc g_obj(hit_box_id%,3), choice(is_super_weapon(), -2, -1)
        if g_obj(hit_box_id%,3) < 0 then g_obj(hit_box_id%,3)=0
        if g_stage%=8 then animate_objects(true)
    end if
    local hit_box_life% = g_obj(hit_box_id%,3)

    if hit_box_life% <= 0 then
        ' Decrement boss life
        inc g_obj(obj_id%,3), -1
        ' Kill eye
        if g_stage%=8 then
            kill_enemy(hit_box_id%, true)
            ' Create final eye
            if g_obj(obj_id%,3) = 1 then
                create_boss_eye(1)
                animate_objects(true)
            end if
        end if
    end if

    ' Replace boss skin
    local aux%=BOSS_LIFE(g_stage%) \ 3
    select case g_stage%
        case 2,4
            if hit_box_life% mod aux% = 0 then
                enqueue_action(1, 90, obj_id%, choice(hit_box_life%\aux%<=1,TILE_SIZE*10,TILE_SIZE*5))
            end if
        case 3
            if hit_box_life%=aux% or hit_box_life%=aux%*2 then inc g_boss(0)
    end select

    if g_obj(obj_id%,3) <= 0 then
        kill_boss()
    else
        play_sfx("BOSS_HIT")
    end if
end function

sub kill_boss()
    g_boss(0)=1    ' Deactivate boss
    g_player(8)=2  ' Freeze player
    increment_score(10000)
    clear_queue()
    destroy_enemies_shots()
    kill_all_enemies()
    animate_boss() ' Animates boss to initial state
    play_song("SILENCE")
    play_sfx("BOSS_KILL")

    ' Blink screen 30 times and destroy boss
    enqueue_action(30, 100,,,,40)
end sub

sub destroy_boss()
    local i%, x%=g_obj(g_boss(1),1), y%=g_obj(g_boss(1),2)

    ' Destroy boss objects
    for i%=1 to 7
        if g_boss(i%) >= 0 then destroy_object(g_boss(i%))
        g_boss(i%) = -1
    next
    g_boss(0)=0

    ' Spawn fire
    enqueue_action(1, 34, x%+TILE_SIZE*2, y%+TILE_SIZE, true)
    enqueue_action(1, 34, x%, y%+TILE_SIZE*2, true)
    enqueue_action(1, 34, x%+TILE_SIZE*4, y%+TILE_SIZE*3, true)
    enqueue_action(1, 34, x%+TILE_SIZE*5, y%+TILE_SIZE*4, true)
    enqueue_action(1, 34, x%+TILE_SIZE, y%+TILE_SIZE*5, true)
    ' Enqueue player animation
    enqueue_action(1, 101,,,,1500)
end sub

sub spawn_boss()
    local x%=SCREEN_WIDTH/2-TILE_SIZE*2.5
    local y%, i%, id%=allocate_object(50+g_stage%, x%, y%, 1, 0, TILE_SIZE*14)

    g_boss(0)=1   ' Status - Waiting start
    g_boss(1)=id% ' Body Object Id
    for i%=2 to 7
        g_boss(i%)=-1 ' Hit box
    next
    g_obj(id%, 4)=0: g_obj(id%, 5)=0

    select case g_stage%
        case 1
            g_obj(id%,2)=TILE_SIZE*(choice(g_row%=0,6,-3))
            g_obj(id%,5)=SCREEN_CENTER_W-TILE_SIZEx2
            create_boss_hit_box(2,TILE_SIZEx2,TILE_SIZEx2)
            show_boss_body(id%,0)
        case 2
            g_obj(id%,2)=TILE_SIZE*(choice(g_row%=0,5,-4))
            spawn_shadow(id%,,TILE_SIZE*6, false, 37)
            create_boss_hit_box(2,TILE_SIZEx2,TILE_SIZEx2)
            show_boss_body(id%,0)
        case 3
            g_obj(id%,2)=TILE_SIZE*(choice(g_row%=0,6,-3))
            g_obj(id%,4)=45
            spawn_shadow(id%,TILE_SIZE+TILE_SIZE/2, TILE_SIZE*7, false)
            create_boss_shield(TILE_SIZE, TILE_SIZE, TILE_SIZE*3)
            create_boss_hit_box(2,TILE_SIZEx2,TILE_SIZEx2)
            show_boss_body(id%,0)
        case 4
            g_obj(id%,2)=TILE_SIZE*(choice(g_row%=0,5,-4))
            g_obj(id%,4)=45
            spawn_shadow(id%,0, TILE_SIZE*8, false, 37)
            show_boss_body(id%,0)
            create_boss_hit_box(2,TILE_SIZEx2,TILE_SIZEx2)
        case 5
            g_obj(id%,2)=TILE_SIZE*(choice(g_row%=0,6,-3))
            g_obj(id%,5)=choice(g_player(0)<SCREEN_WIDTH,-120,120)
            create_boss_shield(TILE_SIZE, TILE_SIZE, TILE_SIZE*3)
            show_boss_body(id%,0)
            create_boss_hit_box(2,TILE_SIZEx2,TILE_SIZEx2)
        case 6
            g_obj(id%,2)=TILE_SIZE*(choice(g_row%=0,6,-3))
            g_obj(id%, 5)=TILE_SIZE*6*choice(g_player(0)<0,-1,1)
            show_boss_body(id%,0)
            create_boss_hit_box(2,TILE_SIZEx2,TILE_SIZEx2)
        case 7
            g_obj(id%,1)=SCREEN_WIDTH/2-TILE_SIZE
            g_obj(id%,2)=TILE_SIZE*5
            show_boss_body(id%,0)
            create_boss_hit_box(2,0,0,TILE_SIZEx2)
        case 8
            g_obj(id%,2)=TILE_SIZE*(choice(g_row%=0,6,-3))
            g_obj(id%,3)=6 ' Boss life, 6 eyes
            g_obj(id%, 5)=TILE_SIZE*6*choice(g_player(0)<0,-1,1)
            show_boss_body(id%,0)
            for i%=2 to 7 ' Create boss eyes
                create_boss_eye(i%)
            next
    end select
end sub

sub create_boss_hit_box(ix%, offset_x, offset_y, width, life)
    if life=0 then life=BOSS_LIFE(g_stage%)
    if width=0 then width=TILE_SIZE
    local id%=g_boss(1)
    local hit_box_id%=allocate_object(50+g_stage%, g_obj(id%, 1)+offset_x, g_obj(id%, 2)+offset_y, life)
    g_boss(ix%)=hit_box_id%
    'sprite read OBJ_INI_SPRITE_ID+hit_box_id%,TRANSPARENT_BLOCK_X, TRANSPARENT_BLOCK_Y, width, TILE_SIZE, OBJ_TILES_BUFFER
    sprite read OBJ_INI_SPRITE_ID+hit_box_id%, 0, 0, width, TILE_SIZE, OBJ_TILES_BUFFER
    g_obj(hit_box_id%, 4)=offset_x
    g_obj(hit_box_id%, 5)=offset_y
    On Error Skip ' Workaround bug with sprite displayed completely off screen.
    sprite show safe OBJ_INI_SPRITE_ID+hit_box_id%, g_obj(hit_box_id%, 1),g_obj(hit_box_id%, 2), 1
end sub

sub create_boss_shield(offset_x, offset_y, width)
    create_boss_hit_box(3, offset_x, offset_y, width, 1000000)
end sub

sub show_boss_body(id%, tile_offset%)
    local tile_x%, tile_y%, sprite_id%=OBJ_INI_SPRITE_ID + id%
    local tile_w%=TILE_SIZE*5, tile_h%=TILE_SIZE*5

    select case g_stage%
        case 2
            tile_x%=80
        case 3
            tile_x%=200
        case 4
            tile_y%=80
            tile_h%=TILE_SIZE*6
        case 5
            tile_x%=120
            tile_y%=80
            tile_h%=TILE_SIZE*6
        case 6
            tile_y%=128
            tile_h%=TILE_SIZE*6
        case 7
            tile_x%=0
            tile_y%=56
            tile_w%=TILE_SIZEx2
            tile_h%=TILE_SIZEx2
        case 8
            tile_x%=80
            tile_y%=128
            tile_w%=TILE_SIZE*6
            tile_h%=TILE_SIZE*6
    end select
    sprite read sprite_id%, tile_x%+tile_offset%, tile_y%, tile_w%, tile_h%, BOSS_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(id%, 1),g_obj(id%, 2), 2
end sub

sub create_boss_eye(eye_id%)
    local offset_x%, offset_y%
    select case eye_id%
        case 1
            offset_x%=TILE_SIZEx2: offset_y%=0
        case 2
            offset_x%=3: offset_y%=6
        case 3
            offset_x%=TILE_SIZE*3+5: offset_y%=6
        case 4
            offset_x%=TILE_SIZEx2: offset_y%=TILE_SIZEx2
        case 5
            offset_x%=3: offset_y%=TILE_SIZEx2+6
        case 6
            offset_x%=TILE_SIZE*3+5: offset_y%=TILE_SIZEx2+6
        case else
            exit sub
    end select
    local boss_id%=g_boss(1)
    local id%=spawn_object(50,g_obj(boss_id%,1)+offset_x%, g_obj(boss_id%,2)+offset_y%)
    g_boss(eye_id%+1)=id%
    g_obj(id%,3)=BOSS_LIFE(g_stage%)
    g_obj(id%,4)=offset_x%
    g_obj(id%,5)=offset_y%
end sub

sub boss_fire()
    local delay, disable_rnd%,i%
    select case g_stage%
        case 1,6
            delay=70
        case 2,4
            delay=120
            disable_rnd%=true
        case 3
            exit sub
        case 5
            delay=70
            disable_rnd%=true
        case 7
            delay=70
        case 8
            delay=-1
            for i%=2 to 7
                if g_boss(i%) <= 0 then continue for
                if g_obj(g_boss(i%),0) > 0 and g_timer mod (130+10*i%) = 0 then
                    if g_obj(g_boss(i%),3) <= 0 then continue for
                    enemy_fire(g_boss(i%), disable_rnd%)
                end if
            next
    end select
    if delay > 0 and g_timer mod delay = 0 then enemy_fire(g_boss(1), disable_rnd%)
end sub

sub animate_boss()
    local x%,y%,id%=g_boss(1),spr_id%=OBJ_INI_SPRITE_ID + g_boss(1)
    select case g_stage%
        case 1
            if g_anim_tick% mod 8 > 3 and g_boss(0) <> 1 then x%=TILE_SIZE*5

        case 3
            local hit_box%=OBJ_INI_SPRITE_ID + g_boss(2)
            local tick%=g_anim_tick% mod 48
            x%=choice(g_boss(0)=3,80,200)
            y%=choice(g_boss(0)=2,0,40)
            if tick% > 23 then
                if tick% = 24 then
                    activate_hit_box(2, true)
                    activate_hit_box(3, false)
                else if tick% = 34 then
                    enemy_fire(g_boss(1), true)
                end if
                ' Wings animation
                tick% = g_anim_tick% mod 4
                inc x%, choice(tick% < 2 or g_boss(0) = 1, TILE_SIZE*5, TILE_SIZE*10)
            else if tick% = 0 then
                activate_hit_box(2, false)
                activate_hit_box(3, true)
            end if

        case 5
            x%=120: y%=80
            if g_anim_tick% mod 10 > 4 and g_boss(0) <> 1 then inc x%, TILE_SIZE*5
            if g_obj(id%,6)<0 then inc x%, TILE_SIZE*10

        case 6
            y%=128
            if g_anim_tick% mod 12 > 5 and g_boss(0) <> 1 then x%=TILE_SIZE*5

        case 7
            local hit_box_life%=g_obj(g_boss(2),3)
            if g_boss(0) <> 1 and g_anim_tick% mod 4 < 2 then
                if hit_box_life% < 11 then
                    x%=TILE_SIZEx4
                else if hit_box_life% < 21 then
                    x%=TILE_SIZEx2
                end if
            end if
            y%=56
            sprite show spr_id%, x%,y%, 2, choice(g_anim_tick% mod 6 > 2,0,1)

        case 8
            x%=80: y%=128
            if g_boss(0) <> 1 and fix(g_obj(g_boss(1), 1)) <> fix(g_obj(g_boss(1), 5)) then
                if g_anim_tick% mod 12 > 5 then inc x%, TILE_SIZE*6
            end if
        case else
            exit sub
    end select

    sprite read spr_id%, x%,y%, SPRITE(W,spr_id%), SPRITE(H,spr_id%), BOSS_TILES_BUFFER
end sub

sub scroll_boss()
    local id%=g_boss(1)
    inc g_obj(id%, 2)
    sprite next OBJ_INI_SPRITE_ID + id%, g_obj(id%, 1), g_obj(id%, 2)
    move_hit_box()
end sub

sub move_boss()
    if g_boss(0) < 2 then exit sub

    local i%=g_boss(1), sprite_ix%=OBJ_INI_SPRITE_ID + i%
    local ini_pos, speed_x, speed_y, mov_height, y%, right_edge%=EDGE_RIGHT-TILE_SIZE*5

    ' Body position
    select case g_stage%
        case 1,8
            inc g_obj(i%, 4) ' Boss tick
            if fix(g_obj(i%, 1)) = fix(g_obj(i%, 5)) then
                if g_obj(i%, 4) mod 150 = 0 then g_obj(i%, 5)=min(max(g_player(0)-TILE_SIZEx2, EDGE_LEFT),right_edge%)
            else
                inc g_obj(i%, 1), choice(g_obj(i%, 1) > g_obj(i%, 5), -1, 1)*BOSS_SPEED*g_delta_time
            end if

        case 2,3,4
            select case g_stage%
                case 2
                    speed_x=1.2: speed_y=9
                    mov_height=6
                    y%=TILE_SIZE*5
                case 3, 4
                    speed_x=1.8: speed_y=3.6
                    mov_height=12
                    y%=TILE_SIZE*6
            end select
            ini_pos=(SCREEN_WIDTH-TILE_SIZE*6)/2
            inc g_obj(i%, 4),BOSS_SPEED*g_delta_time*speed_x
            inc g_obj(i%, 5),BOSS_SPEED*g_delta_time*speed_y
            g_obj(i%, 1)=ini_pos+(ini_pos-TILE_SIZEx4)*sin(g_obj(i%, 4))
            g_obj(i%, 2)=y%+mov_height*cos(g_obj(i%, 5))

        case 5
            inc g_obj(i%, 4) ' Boss tick
            ' Prepare lateral movement
            if g_obj(i%, 4) mod 200 = 0 then
                g_obj(i%, 5)=choice(g_obj(i%, 1)>g_player(0),-120,120)
                ' Not using OR to keep the code short
                if g_obj(i%, 1)=EDGE_LEFT and g_obj(i%, 5)<0 then
                    g_obj(i%, 5)=-g_obj(i%, 5)
                else if g_obj(i%, 1)=right_edge% and g_obj(i%, 5)>0 then
                    g_obj(i%, 5)=-g_obj(i%, 5)
                end if
                ' Close helmet
                g_obj(i%, 6)=0
                activate_hit_box(2, false)
                activate_shield(true)
            end if
            ' Open helmet
            if g_obj(i%, 6)=0 and g_obj(i%, 4) mod 300 = 0 then
                g_obj(i%, 6)=-1
                activate_hit_box(2, true)
                activate_shield(false)
                play_sfx("HELMET")
            end if
            ' Check edges
            if g_obj(i%, 5)<>0 then
                speed_x=choice(g_obj(i%, 5)<0,1,-1)
                inc g_obj(i%, 5),speed_x
                inc g_obj(i%, 1), BOSS_SPEED*g_delta_time*-speed_x
                g_obj(i%, 1)=min(max(g_obj(i%, 1), EDGE_LEFT), right_edge%)
            end if

        case 6
            inc g_obj(i%, 4) ' Boss tick
            if fix(g_obj(i%, 1)) = fix(g_obj(i%, 5)) then
                if g_obj(i%, 4) mod 300 = 0 then
                    ' Open the terrain
                    play_sfx("TERRAIN")
                    enqueue_action(1,49,g_obj(i%, 1)+TILE_SIZEx2,g_obj(i%, 2)+TILE_SIZE*6)
                    ' Select next pos
                    speed_x=choice(g_obj(i%, 1) > g_player(0),-1,1)
                    if fix(g_obj(i%, 1))=EDGE_LEFT then speed_x=1
                    if fix(g_obj(i%, 1))=right_edge% then speed_x=-1
                    g_obj(i%, 5)=min(max(g_obj(i%, 1)+TILE_SIZE*9*speed_x, EDGE_LEFT), right_edge%)
                end if
            else
                inc g_obj(i%, 1), choice(g_obj(i%, 1) > g_obj(i%, 5), -1, 1)*BOSS_SPEED*g_delta_time
            end if

        case 7
            speed_x=30+BOSS_LIFE(7)-g_obj(g_boss(2),3)
            if g_obj(i%, 4)=0 or g_obj(i%, 4) mod 200=0 or g_obj(i%, 1) < TILE_SIZEx4 or g_obj(i%, 1) > SCREEN_WIDTH-TILE_SIZE*6 then
                g_obj(i%,5)=deg(sprite(V, sprite_ix%, 1))-90
            end if
            inc g_obj(i%, 4) ' Boss tick
            inc g_obj(i%, 1), speed_x*cos(g_obj(i%,5))*g_delta_time
            inc g_obj(i%, 2), speed_x*sin(g_obj(i%,5))*g_delta_time
            if g_obj(i%, 2) > SCREEN_HEIGHT+1 then
                g_obj(i%, 1)=SCREEN_WIDTH/2-TILE_SIZE
                g_obj(i%, 2)=TILE_SIZE*5
            end if
    end select

    ' Boss body
    sprite next sprite_ix%, g_obj(i%, 1), g_obj(i%, 2)

    ' Boss hit box
    move_hit_box()
end sub

sub activate_hit_box(i%, active%)
    if g_boss(i%) < 0 then exit sub
    local sprite_id%=OBJ_INI_SPRITE_ID + g_boss(i%)

    if sprite(X, sprite_id%) = 10000 and active% then
        'sprite show safe sprite_id%, g_obj(i%, 1),g_obj(i%, 2), 1
    else if sprite(X, sprite_id%) <> 10000 and not active% then
        sprite hide safe sprite_id%
    end if
end sub

sub activate_shield(active%)
    activate_hit_box(3,active%)
end sub

function is_hit_box_active(i%)
    is_hit_box_active=sprite(X, OBJ_INI_SPRITE_ID + g_boss(i%))<>10000
end function

sub move_hit_box()
    local i%, sprite_id%, box_id%, id%=g_boss(1)
    for i%=2 to 7
        if g_boss(i%) < 0 then continue for
        box_id% = g_boss(i%)
        g_obj(box_id%, 1)=g_obj(id%, 1)+g_obj(box_id%, 4)
        g_obj(box_id%, 2)=g_obj(id%, 2)+g_obj(box_id%, 5)
        sprite_id%=OBJ_INI_SPRITE_ID + box_id%
        if sprite(x, sprite_id%) <> 10000 then sprite next sprite_id%, g_obj(box_id%, 1),g_obj(box_id%, 2)
    next
end sub
