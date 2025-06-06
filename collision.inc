sub check_collision()
    local spr_id% = sprite(S)
    if spr_id% >= 1 then
        process_collision(spr_id%)
    else if spr_id% = 0 then
        local i%
        for i% = 1 to sprite(C, 0) ' Total number of sprites that have active collisions.
            on error skip 2
            spr_id%=sprite(C, 0, i%)
            If spr_id% > 0 Then process_collision(spr_id%)
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
                        if hit_object(collided_id%, true) then destroy_shot(sprite_id%)

                    ' Hits another shot
                    else if collided_id% >= SHOTS_INI_SPRITE_ID then
                        if hit_shot(collided_id%) then destroy_shot(collided_id%, true)
                    end if

                ' Enemies shots
                else
                    select case collided_id%
                        case 1
                            if hit_player(sprite_id%, true) then destroy_shot(sprite_id%)
                        case 2
                            if hit_shield(sprite_id%) then destroy_shot(sprite_id%)
                    end select
                end if
        end select
    next
end sub

sub player_hit_obj(collided_id%)
    local obj_ix%=collided_id% - OBJ_INI_SPRITE_ID
    if obj_ix% < 0 then exit sub
    select case g_obj(obj_ix%, 0)
        case 20 ' Weapon chrystal
            change_weapon(obj_ix%)
            destroy_object(obj_ix%)
        case 21 ' Power-up chrystal
            power_up(obj_ix%)
            destroy_object(obj_ix%)
        case 50 ' Portal
            ' Ignore so that the player can walk through it.
        case else
            local hit
            if g_player(5) = 4 then ' If player has invincibility
                hit=hit_object(collided_id%, true, true)
            else
                hit=hit_player(collided_id%, false)
            end if
    end select
end sub

' Return true if the hit destroys the shot
function hit_block(sprite_id%) as integer
    local i%=sprite_id% - BLOCK_INI_SPRITE_ID
    local max_hits%=block_max_hits(i%)

    if g_blocks(i%,1) = max_hits% then exit function

    ' Increment hits
    inc g_blocks(i%,1), 1
    hit_block = true

    g_cont_sfx$=""
    if g_blocks(i%,1) < max_hits% then
        play_sfx("BLOCK_HIT")
    else if g_blocks(i%,1) = max_hits% then
        play_sfx("BLOCK_OPEN")
    end if

    if g_blocks(i%,1)=1 or g_blocks(i%,1)=max_hits% then
        ' Spawn new block tile
        enqueue_action(1, 22, i%)
    end if
end function

' Return true if the hit destroys the shot
function hit_shot(sprite_id%) as integer
    local shot_id%=sprite_id%-SHOTS_INI_SPRITE_ID
    local weapon%=g_shots(shot_id%, 0)
    ' Check if hit a scythe or bone
    hit_shot=(weapon%=25 or weapon%=33)
end function

' Return true if the hit destroys the shot
function hit_object(collided_id%, sfx%, instakill%) as integer
    local i%=collided_id% - OBJ_INI_SPRITE_ID, obj_id%=g_obj(i%,0),kill_enemy=true

    hit_object=false
    g_cont_sfx$=""
    select case obj_id%
        case 5 ' Cloud
            if instakill% then
                start_fire_animation(g_obj(i%,1), g_obj(i%,2), true)
                destroy_object(i%)
            else
                local angle=deg(sprite(V, collided_id%, 1))
                g_obj(i%,3)=1
                g_obj(i%,4)=CLOUD_ATTACK_SPEED*sin(angle)+0.01 ' Speed X
                g_obj(i%,5)=CLOUD_ATTACK_SPEED*-cos(angle)     ' Speed Y
            end if

        case is < 20 ' Enemies
            ' Special collision cases
            select case obj_id%
                case 7 ' Skeleton
                    ' Is it broken?
                    if g_obj(i%,5) > 0 or g_obj(i%,4) > 1 then exit function
                    ' Has life remaining
                    if g_obj(i%,3) > 1 then
                        g_obj(i%,5) = 1
                        ' split skeleton
                        play_sfx("SPLIT")
                        enqueue_action(1,7,g_obj(i%,1),g_obj(i%,2),1)
                        enqueue_action(1,7,g_obj(i%,1),g_obj(i%,2),2)
                        sprite hide safe collided_id%
                    end if

                case 10 ' Zombie
                    if abs(g_obj(i%,4))=1000 then exit function
                    if g_obj(i%,5) = 0 then
                        kill_enemy=false
                        ' Split zombie
                        play_sfx("SPLIT")
                        enqueue_action(1,10,g_obj(i%,1),g_obj(i%,2),-1000)
                        enqueue_action(1,10,g_obj(i%,1),g_obj(i%,2), 1000)
                    end if

            end select

            inc g_obj(i%,3), -1 ' Decrement life
            if not instakill% and g_obj(i%,3) > 0 then
                if not is_super_weapon() then hit_object=true
                exit function
            end if
            if kill_enemy then
                increment_score(OBJ_POINTS(g_obj(i%,0)))
                start_fire_animation(g_obj(i%,1), g_obj(i%,2), true)
                play_sfx("ENEMY_KILL")
            end if
            destroy_object(i%)

        case 20,21 ' Weapon and power-up chrystal
            inc g_obj(i%,4)
            g_obj(i%,4)=g_obj(i%,4) mod choice(g_obj(i%,0) = 20, 8, 7)
            ' Enable the wobbling movement
            if g_obj(i%,4) > 3 and g_obj(i%,5) < 0 then g_obj(i%,5)=0
            if sfx% then play_sfx("POWER_UP_HIT")
            hit_object=true

        case 34,49 ' Fire, Terrain
            exit function

        case is > 50 ' Boss
            hit_object = hit_boss(collided_id%)
    end select

    if not is_super_weapon() then hit_object=true

end function

' Return true if the hit destroys the shot
function hit_shield(shot_sprite_id%) as integer
    hit_shield=false
    if g_player(6)=0 then exit function

    local force%=1, shot_ix%=shot_sprite_id%-SHOTS_INI_SPRITE_ID
    hit_shield=true

    select case g_shots(shot_ix%,0)
        case 15,32,33 ' Energy ray, Axe, Scythe
            force%=3
        case 28 ' mini explosion
            force%=3
            ' Is it on fire?
            if g_shots(shot_ix%,3)=0 and g_shots(shot_ix%,4)=0 then
                hit_shield=0
                exit function
            end if
    end select

    inc g_player(6), -force%
    play_sfx("SHIELD")
    check_shield()
end function

' Return true if the hit destroys the shot
function hit_player(collided_id%, shot_hit%) as integer
    hit_player=false
    if g_invincible or g_player(8) > 3 or g_player(5) = 2 or g_player(5) = 4 then exit function
    if not shot_hit% then
        local obj_id%=g_obj(collided_id% - OBJ_INI_SPRITE_ID,0)
        select case obj_id%
            case 20,21,34 ' Fire, Crystals
                exit function
        end select
    end if
    hit_player=true
    kill_player()
end function
