' TODO: implement!!!
sub hit_player(collided_id%)
end sub

function hit_block(sprite_id%) as integer
    local i%=sprite_id% - BLOCK_INI_SPRITE_ID
    local max_hits%=block_max_hits(i%)

    if g_blocks(i%,1) = max_hits% then exit function

    ' Increment hits
    inc g_blocks(i%,1), 1
    hit_block = true

    g_cont_sfx$=""
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

sub hit_object(obj_sprite_id%, instakill%, no_sfx%)
    local obj_id%=obj_sprite_id% - OBJ_INI_SPRITE_ID

    g_cont_sfx$=""
    select case g_obj(obj_id%,0)
        case 29,30 ' Weapon and power-up chrystal
            inc g_obj(obj_id%,4)
            g_obj(obj_id%,4)=g_obj(obj_id%,4) mod choice(g_obj(obj_id%,0) = 29, 8, 7)
            ' Enable the wobbling movement
            if g_obj(obj_id%,4) > 3 and g_obj(obj_id%,5) < 0 then g_obj(obj_id%,5)=0
            if g_sound_on% and not no_sfx% then play effect "POWER_UP_HIT_SFX"

        case else ' Enemies
            ' Decrement life
            inc g_obj(obj_id%,3), -1 ' TODO: Implement weapon force
            if not instakill% and g_obj(obj_id%,3) > 0 then exit sub

            ' Increment score
            increment_score(OBJ_POINTS(g_obj(obj_id%,0)))

            ' Delete enemy's object
            destroy_object(obj_id%)

            destroy_shadow(obj_id%)
            start_enemy_death_animation(obj_id%)
            if g_sound_on% and not no_sfx% then play effect "ENEMY_DEATH_SFX"
    end select
end sub

sub hit_shield()
    if g_player(6)=0 then exit sub
    inc g_player(6), -1
    if g_sound_on% then play effect "SHIELD_SFX"
    select case g_player(6)
        case 0
            g_player(5)=0
            sprite hide safe 2
        case fix(SHIELD_MAX_HITS/6)
            sprite read 2, OBJ_TILE%(32,0)+TILE_SIZEx4, OBJ_TILE%(32,1), OBJ_TILE%(32,2), OBJ_TILE%(32,3), OBJ_TILES_BUFFER
        case fix(SHIELD_MAX_HITS/6*2)
            sprite read 2, OBJ_TILE%(32,0)+TILE_SIZEx2, OBJ_TILE%(32,1), OBJ_TILE%(32,2), OBJ_TILE%(32,3), OBJ_TILES_BUFFER
    end select
end sub
