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
                        if hit_object(collided_id%, true) then destroy_shot(sprite_id%)

                    ' Hits another shot
                    else if collided_id% >= SHOTS_INI_SPRITE_ID then
                        if hit_shot(collided_id%) then destroy_shot(collided_id%, true)

                    end if

                ' Enemies shots
                else
                    ' Hits the shield
                    if collided_id% = 2 then
                        hit_shield(sprite_id%)
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
                local hit=hit_object(collided_id%, true, true)
            ' If player is not invisible
            else if g_player(5) <> 2 then
                hit_player(collided_id%)
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
        enqueue_action(1, 31, i%)
    end if
end function

' Return true if the hit destroys the shot
function hit_shot(sprite_id%) as integer
    local shot_id%=sprite_id% - SHOTS_INI_SPRITE_ID
    local weapon%=g_shots(shot_id%, 0)
    ' Check if hit a scythe or bone
    hit_shot=(weapon% = 18 or weapon% = 26)
end function

' Return true if the hit destroys the shot
function hit_object(collided_id%, sfx%, instakill%) as integer
    local i%=collided_id% - OBJ_INI_SPRITE_ID, obj_id%=g_obj(i%,0)

    hit_object=false
    g_cont_sfx$=""

    select case obj_id%
        case 29,30 ' Weapon and power-up chrystal
            inc g_obj(i%,4)
            g_obj(i%,4)=g_obj(i%,4) mod choice(g_obj(i%,0) = 29, 8, 7)
            ' Enable the wobbling movement
            if g_obj(i%,4) > 3 and g_obj(i%,5) < 0 then g_obj(i%,5)=0
            if sfx% then play_sfx("POWER_UP_HIT")
            hit_object=true

        case is > 50 ' Boss
            hit_object = hit_boss(collided_id%)

        case 5 ' Cloud
            if instakill% then
                start_fire_animation(g_obj(i%,1), g_obj(i%,2), true)
                destroy_object(i%)
            else
                local angle=deg(sprite(V, collided_id%, 1))
                g_obj(i%,3)=CLOUD_ATTACK_SPEED*sin(angle)+0.01 ' Speed X
                g_obj(i%,4)=CLOUD_ATTACK_SPEED*-cos(angle)     ' Speed Y
            end if

        case is < 15 ' Enemies
            ' Skeleton
            if obj_id% = 7 then
                ' Is it broken?
                if g_obj(i%,5) > 0 or g_obj(i%,4) > 1 then exit function
                ' Has life remaining
                if g_obj(i%,3) > 1 then
                    g_obj(i%,5) = 1
                    enqueue_action(1,7,g_obj(i%,1),g_obj(i%,2),1)
                    enqueue_action(1,7,g_obj(i%,1),g_obj(i%,2),2)
                    sprite hide safe collided_id%
                end if
            end if

            inc g_obj(i%,3), -1 ' Decrement life
            if not instakill% and g_obj(i%,3) > 0 then exit function
            increment_score(OBJ_POINTS(g_obj(i%,0)))
            start_fire_animation(g_obj(i%,1), g_obj(i%,2), true)
            destroy_object(i%)
    end select

    if not is_super_weapon() then hit_object=true

end function

sub hit_shield(shot_sprite_id%)
    if g_player(6)=0 then exit sub

    local force%=1

    select case g_shots(shot_sprite_id% - SHOTS_INI_SPRITE_ID,0)
        ' Energy ray, mini explosion, Scythe
        case 15,21,26
            force%=3
    end select

    inc g_player(6), -force%
    play_sfx("SHIELD")
    select case g_player(6)
        case is <= 0
            g_player(5)=0
            g_player(6)=0
            sprite hide safe 2
        case is <= SHIELD_MAX_HITS\6
            sprite read 2, OBJ_TILE%(32,0)+TILE_SIZEx4, OBJ_TILE%(32,1), OBJ_TILE%(32,2), OBJ_TILE%(32,3), OBJ_TILES_BUFFER
        case is <= SHIELD_MAX_HITS\6*2
            sprite read 2, OBJ_TILE%(32,0)+TILE_SIZEx2, OBJ_TILE%(32,1), OBJ_TILE%(32,2), OBJ_TILE%(32,3), OBJ_TILES_BUFFER
    end select
end sub

' TODO: implement!!!
sub hit_player(collided_id%)
end sub
