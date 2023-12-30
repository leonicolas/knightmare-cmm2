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
                        hit_object(collided_id%, true)
                        if is_boss_hit_box(collided_id%) or not is_super_weapon() or is_chrystal(collided_id%) then destroy_shot(sprite_id%)
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
                hit_object(collided_id%, true, true)
            ' If player is not invisible
            else if g_player(5) <> 2 then
                hit_player(collided_id%)
            end if
    end select
end sub

