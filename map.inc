sub replace_block(block_ix%)
    local max_hits%=block_max_hits(block_ix%)

    if g_blocks(block_ix%,0) <> 6 and g_blocks(block_ix%,1) > 1 and g_blocks(block_ix%,1) < max_hits% then
        exit sub
    end if

    local offset%, sprite_id%=BLOCK_INI_SPRITE_ID+block_ix%

    ' Calculate the tile X offset
    if g_blocks(block_ix%,0) = 6 or g_blocks(block_ix%,1) >= max_hits% then
        offset%=TILE_SIZEx2*g_blocks(block_ix%,0)
    end if

    ' Collected block
    if g_blocks(block_ix%,0) = 6 then
        ' Save sprite position and layer
        local x%=sprite(X, sprite_id%), y%=sprite(Y, sprite_id%)
        destroy_block(block_ix%)
        ' Replace map tile
        page write SCREEN_BUFFER
        blit OBJ_TILE%(22,0)+offset%, OBJ_TILE%(22,1), x%,y%, OBJ_TILE%(22,2),OBJ_TILE%(22,3), OBJ_TILES_BUFFER
        page write SPRITES_BUFFER

    ' Bridge block
    else if g_blocks(block_ix%,0) = 7 and g_blocks(block_ix%,1) >= max_hits% then
        spawn_bridge(sprite(X, sprite_id%), sprite(Y, sprite_id%))
        make_bridge_ground(g_blocks(block_ix%, 2)+1, g_blocks(block_ix%, 3)-1, false)
        destroy_block(block_ix%)
        play_sfx("BLOCK_OPEN")

    ' Other blocks
    else
        ' Replace buffer tiles
        sprite read sprite_id%, OBJ_TILE%(22,0)+offset%, OBJ_TILE%(22,1), OBJ_TILE%(22,2), OBJ_TILE%(22,3), OBJ_TILES_BUFFER
        ' Makes the barrier solid
        if g_blocks(block_ix%, 0) = 5 and g_blocks(block_ix%,1) = max_hits% then
            make_block_solid(g_blocks(block_ix%, 2), g_blocks(block_ix%, 3))
        end if
    end if
end sub

sub spawn_bridge(x%, y%, down%)
    local i%, offset%
    page write SCREEN_BUFFER
    for i%=0 to 3
        offset%=TILE_SIZE*i%*choice(down%,1,-1)+TILE_SIZE
        blit OBJ_TILE%(48,0), OBJ_TILE%(48,1), x%-TILE_SIZE,y%+offset%, OBJ_TILE%(48,2),OBJ_TILE%(48,3), OBJ_TILES_BUFFER
    next
    page write SPRITES_BUFFER
end sub

sub make_bridge_ground(row%, col%, down%)
    local i%
    for i%=0 to 3
        make_tile_solid(row%, col%  , false)
        make_tile_solid(row%, col%+1, false)
        make_tile_solid(row%, col%+2, false)
        make_tile_solid(row%, col%+3, false)
        inc row%, choice(down%, 1, -1)
    next
end sub

sub make_block_solid(row%, col%)
    make_tile_solid(row%  , col%  , true)
    make_tile_solid(row%  , col%+1, true)
    make_tile_solid(row%+1, col%  , true)
    make_tile_solid(row%+1, col%+1, true)
end sub

sub make_tile_solid(row%, col%, solid%)
    local i%=row%*MAP_COLS+col%
    g_map(i%)=choice(solid%, g_map(i%) or &H80, g_map(i%) and &HFF7F)
end sub

sub load_map(stage%)
    local i%=0, lhs%, rhs%, solid_bit%
    local file_name$ = MAPS_DIR + "/stage" + str$(stage%) + ".map"
    open file_name$ for input as #5
    do while not eof(5)
        lhs%=asc(input$(1, #5)) ' Object data
        rhs%=asc(input$(1, #5)) ' Tile data
        g_map(i%)=(lhs% << 8) or rhs%
        inc i%
    loop
    close #5
end sub

sub scroll_map()
    if not g_scroll_on then exit sub
    local i%, sprite_id%

    ' Scroll map blocks sprites
    for i%=0 to bound(g_blocks())
        if g_blocks(i%,0)=0 then continue for
        sprite_id%=BLOCK_INI_SPRITE_ID + i%
        sprite next sprite_id%, sprite(X, sprite_id%), sprite(Y, sprite_id%) + 1
        if sprite(Y, sprite_id%) > SCREEN_HEIGHT+TILE_SIZEx2 then destroy_block(i%)
    next

    ' Scroll boss vertically
    if g_boss(0) > 0 then scroll_boss()

    ' Scroll map tiles
    page scroll SCREEN_BUFFER, 0, -1

    check_scroll_collision()
    inc g_tile_px%,1

    ' Draw the next map tile row
    if g_tile_px% = TILE_SIZE then
        g_tile_px%=0
        inc g_row%,-1
        ' Draw the next map row on the top of the screen buffer
        process_map_row(g_row%, true)
    end if
end sub

sub check_scroll_collision()
    if map_collide(g_player()) then
        inc g_player(1),2
        if fix(g_player(1)+TILE_SIZE/2) > SCREEN_HEIGHT then kill_player()
    end if
end sub

sub calculate_start_row()
    if g_row% <= 10 then
        g_row%=0
        exit sub
    end if
    local solid=1
    inc g_row%, 5
    do while solid
        solid=is_solid(g_row%+PLAYER_INIT_ROW,PLAYER_INIT_COL)
        solid=solid and is_solid(g_row%+PLAYER_INIT_ROW,PLAYER_INIT_COL+1)
        solid=solid and is_solid(g_row%+PLAYER_INIT_ROW+1,PLAYER_INIT_COL)
        solid=solid and is_solid(g_row%+PLAYER_INIT_ROW+1,PLAYER_INIT_COL)
        if solid then inc g_row%
    loop
end sub

function is_solid(row%,col%) as integer
    is_solid=g_map(row%*MAP_COLS+col%) and &H80
end function

'
' Returns false (0) or true (1)
function map_collide(player()) as integer
    local x=fix(player(0))

    if player(0)-x >= 0.9 then inc x
    local col%=x\TILE_SIZE
    local row%=(player(1)-g_tile_px%)\TILE_SIZE+g_row%+1
    local ix_top%=row%*MAP_COLS, ix_bottom%=(row%+1)*MAP_COLS
    ' Check top left
    map_collide=g_map(ix_top%+col%) and &H80
    ' Check top right
    if not map_collide then map_collide=g_map(ix_top%+col%+2) and &H80
    ' Check bottom left
    if not map_collide then map_collide=g_map(ix_bottom%+col%) and &H80
    ' Check bottom right
    if not map_collide then map_collide=g_map(ix_bottom%+col%+2) and &H80
    ' Check horizontal wrapping collision
    if not map_collide and x < 0 then map_collide=(g_map((row%-1)*MAP_COLS+MAP_COLS_0) and &H80) or (g_map((row%+4)*MAP_COLS+MAP_COLS_0) and &H80)
    if not map_collide and x > SCREEN_WIDTH - TILE_SIZE * 2 then map_collide=(g_map((row%-1)*MAP_COLS) and &H80) or (g_map((row%+4)*MAP_COLS) and &H80)
end function

sub draw_map(row%)
    local r%, offset_y%=SCREEN_HEIGHT+TILE_SIZE
    row%=min(MAP_ROWS_0-SCREEN_ROWS,row%)
    g_row%=row%
    page write SCREEN_BUFFER
    for r%=SCREEN_ROWS+row%-1 to row%-2 step -1
        page scroll SCREEN_BUFFER,0,-TILE_SIZE
        process_map_row(r%, false, row%=0, offset_y%)
        inc offset_y%, -TILE_SIZE
    next
    g_row%=r%+1
end sub

sub process_map_row(row%, spawn%, line_zero%, offset_y%)
    local tile%,col%,tx%,ty%,obj_id%,tile_data%,extra%

    ' Boss triggers
    select case row%
        case is < -1
            activate_boss()
            exit sub
        case is < 0
            exit sub
        case 7
            spawn_boss()
        case 8
            play_boss_song()
            destroy_enemies_shots()
            kill_all_enemies()
    end select

    for col%=MAP_COLS_0 to 0 step -1
        ' Tile drawing
        page write SCREEN_BUFFER
        tile_data%=g_map(row%*MAP_COLS+col%)
        tile%=tile_data% AND &H7F
        tx%=(tile% mod TILES_COLS) * TILE_SIZE
        ty%=(tile% \ TILES_COLS) * TILE_SIZE + TILES_OFFSET(g_stage% - 1)
        blit tx%,ty%, col%*TILE_SIZE,0, TILE_SIZE,TILE_SIZE, MAP_TILES_BUFFER
        page write SPRITES_BUFFER

        ' Create object
        obj_id%=tile_data% >> 8 AND &H1F
        if not spawn% and obj_id% <> 22 then continue for
        if obj_id% then
            extra%=tile_data% >> 13
            select case obj_id%
                case 20, 21 ' Weapon chrystal or power-up chrystal
                    spawn_object(obj_id%, min(SCREEN_WIDTH-TILE_SIZE*6,max(TILE_SIZEx4,g_player(0))), 0)
                case 22 ' Block
                    spawn_block(row%, col%, extra%, offset_y%)
                case else ' Enemies and other objects
                    spawn_object(obj_id%, col%*TILE_SIZE, 0, choice(g_difficulty%=0,min(3, extra%),extra%))
            end select
        end if
    next
end sub

function block_max_hits(i%) as integer
    if g_blocks(i%,0) = 6 then
        ' collected block doesn't have max hits
        block_max_hits = 0
    else
        ' Special blocks need twice of hits
        block_max_hits = choice(g_blocks(i%,0) > 1 and g_blocks(i%,0) < 5, BLOCK_HITS * 2, BLOCK_HITS)
    end if
end function

sub collect_block_bonus(sprite_id%)
    local i%=sprite_id% - BLOCK_INI_SPRITE_ID

    if g_blocks(i%,1) < block_max_hits(i%) or g_blocks(i%,0) > 4 then exit sub

    select case g_blocks(i%,0)
        case 1 ' Hook - points
            increment_score(BLOCK_POINTS)
            play_sfx("POINTS")
        case 2 ' Knight - kill all enemies in the screen
            kill_all_enemies(true)
            destroy_enemies_shots()
        case 3 ' Queen - Extra life
            update_life(1)
        case 4 ' King - Freeze enemies
            g_freeze_timer=FREEZE_TIME
            destroy_enemies_shots()
    end select

    ' Replace block content by empty block
    g_blocks(i%,0)=6
    enqueue_action(1, 22, i%)
end sub

sub destroy_block(block_ix%)
    local sprite_id%=BLOCK_INI_SPRITE_ID + block_ix%
    g_blocks(block_ix%,0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
end sub

sub spawn_block(row%, col%, type%, offset_y%)
    local i%, block_id%

    for i%=0 to bound(g_blocks())
        if g_blocks(i%, 0) = 0 then exit for
    next

    block_id%=22
    g_blocks(i%,0)=type% ' Block type
    g_blocks(i%,1)=0     ' Hits
    g_blocks(i%,2)=row%  ' Row
    g_blocks(i%,3)=col%  ' Col

    sprite read BLOCK_INI_SPRITE_ID + i%, TRANSPARENT_BLOCK_X, TRANSPARENT_BLOCK_Y, TILE_SIZEx2, TILE_SIZEx2, OBJ_TILES_BUFFER
    sprite show safe BLOCK_INI_SPRITE_ID + i%, col%*TILE_SIZE,offset_y%, 1
end sub
