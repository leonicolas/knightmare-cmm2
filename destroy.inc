sub destroy_enemies_all_shots()
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
    if not g_obj(obj_ix%, 0) then exit sub
    local sprite_id%=OBJ_INI_SPRITE_ID + obj_ix%
    g_obj(obj_ix%, 0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
end sub

sub destroy_block(block_ix%)
    local sprite_id%=BLOCK_INI_SPRITE_ID + block_ix%
    g_blocks(block_ix%,0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
end sub

sub destroy_shadow(src_obj_ix%)
    local shadow_ix%=g_obj(src_obj_ix%,6)

    if shadow_ix% < 0 then exit sub

    g_obj(src_obj_ix%,6) = -1
    destroy_object(shadow_ix%)
end sub

sub kill_all_enemies()
    local i%
    if g_sound_on% then play effect "KILL_ALL_ENEMIES_SFX"
    for i%=0 to bound(g_obj())
        ' Check for free slots
        if g_obj(i%,0)=0 then continue for
        hit_object(OBJ_INI_SPRITE_ID + i%, true, true)
    next
end sub
