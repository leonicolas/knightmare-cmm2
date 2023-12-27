sub spawn_object(obj_id%, x%, y%, map_data%)
    local sprite_id%, offset_x%, offset_y%, layer%=1, i%=get_free_object_slot()
    if i% < 0 then exit sub

    g_obj(i%,0)=obj_id%             ' Object Id
    g_obj(i%,1)=x%                  ' X
    g_obj(i%,2)=y%                  ' Y
    g_obj(i%,3)=OBJ_DATA(obj_id%,2) ' GPR 1
    g_obj(i%,4)=OBJ_DATA(obj_id%,3) ' GPR 2
    g_obj(i%,5)=OBJ_DATA(obj_id%,4) ' GPR 3
    g_obj(i%,6)=-1                  ' Shadow index

    select case obj_id%
        case 2,3 ' Bat
            ' Calculate the correct bat angle
            if x% < SCREEN_CENTER then g_obj(i%,4)=g_obj(i%,4)+180
            ' Initialize bat spawning data
            if map_data% then
                enqueue_action(map_data%, obj_id%, x%, y%,, choice(obj_id%=2, BAT_SPAWN_SPEED_MS, BAT_WAVE_SPAWN_SPEED_MS))
            end if
            ' Spawn the bat shadow
            create_shadow(i%, TILE_SIZEx2+TILE_SIZE/2)

        case 20 ' Fire
            offset_x%=FIRE_ANIM(0)*TILE_SIZEx2
            layer%=4

    end select

    sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, OBJ_TILE%(obj_id%,0)+offset_x%, OBJ_TILE%(obj_id%,1)+offset_y%, OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), layer%
end sub

sub spawn_block(x%, y%, type%)
    local i%, block_id%, row%, col%
    for i%=0 to bound(g_blocks())
        if g_blocks(i%, 0) = 0 then exit for
    next

    block_id%=31
    g_blocks(i%,0)=type% ' Block type
    g_blocks(i%,1)=0     ' Hits

    sprite read BLOCK_INI_SPRITE_ID + i%, TRANSPARENT_BLOCK_X, TRANSPARENT_BLOCK_Y, TILE_SIZEx2, TILE_SIZEx2, OBJ_TILES_BUFFER
    sprite show safe BLOCK_INI_SPRITE_ID + i%, x%,0, 1
end sub

sub spawn_shield()
    sprite read 2, OBJ_TILE%(32,0), OBJ_TILE%(32,1), OBJ_TILE%(32,2), OBJ_TILE%(32,3), OBJ_TILES_BUFFER
    if sprite(X, 2) = 10000 then sprite show 2, g_player(0),g_player(1)-TILE_SIZE, 1
    g_player(5)=1
    g_player(6)=SHIELD_MAX_HITS
end sub

sub create_shadow(obj_ix%, height%)
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
