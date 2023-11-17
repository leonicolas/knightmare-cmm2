option explicit
option base 0
option angle degrees
option default float

#include "constants.inc"

' Global variables
dim g_sound_on%=0
dim g_map(MAP_SIZE) as integer ' Map data
dim g_row%                     ' Current top map row. Zero is the bottom row.
dim g_game_tick                ' The game tick
dim g_tile_px%                 ' The rendered tile row pixel
dim g_kb_released%             ' Tracks kb keys releasing
dim g_i%                       ' Auxiliary global integer variables
dim g_temp1,g_temp2,g_x,g_y    ' Auxiliary global float variables

' Timers
dim g_scroll_timer             ' Scroll timer
dim g_player_timer             ' Player timer
dim g_obj_timer                ' Objects timer
dim g_shot_timer               ' Shot timer

' Player state: x, y, animation counter, weapon, speed
dim g_player(4)
dim g_player_shot_ms=SHOT_COOLDOWN_MS
dim g_player_animation_ms=PLAYER_ANIMATION_MS

' Supports 12 shots at the same time (id, x, y, speed x, speed y).
' The three first slots are for the player shots, the other ones for enemies
' 0 - No shot
' 1 - Player Arrow
' 2 - Player Double arrow
' 3 - Player Fire
' 4 - Player Boomerang
' 5 - Player Sword
' 6 - Player Double arrow (level 2)
' 7 - Player Fire (level 2)
' 8 - Player Boomerang (level 2)
' 9 - Player Sword (level 2)
dim g_shots(12,4)
const SHOTS_NUM%=bound(g_shots())
' Object data: 0: obj id | 1: x | 2: y | 3: life | 4: gpr1 | 5: gpr2 | 6: shadow
dim g_obj(20,6)
' Object spawn data queue: 0: quantity | 1: obj id | 2: x | 3: y | 4: next spawn time
dim g_spawn_queue(bound(g_obj()),4)
' Blocks: 0: type | 1: hits
dim g_blocks(5,1)
dim g_anim_tick%=0

const OBJ_INI_SPRITE_ID=bound(g_shots()) + 1 ' Initial sprite Id for objects
const BLOCK_INI_SPRITE_ID=OBJ_INI_SPRITE_ID+bound(g_obj()) + 1 ' Initial sprite Id for blocks

init()
run_stage(1)
page write 0: end

sub run_stage(stage%)
    init_stage(stage%)

    do
        ' Game tick
        if timer - g_game_tick < GAME_TICK_MS then continue do
        g_game_tick=timer

        ' Process keyboard
        process_kb()

        ' Scrolls the map
        if g_row% >= 0 and timer - g_scroll_timer >= SCROLL_SPEED_MS then
            scroll_map()
            g_scroll_timer=timer
        end if

        ' Process player animation
        if timer - g_player_timer >= g_player_animation_ms then
            g_player(2)=not g_player(2)
            sprite read #1, choice(g_player(2),PLAYER_SKIN1_X_R,PLAYER_SKIN1_X_L), PLAYER_SKIN_Y, TILE_SIZEx2, TILE_SIZEx2, OBJ_TILES_BUFFER
            g_player_timer=timer
        end if

        ' Process objects animation
        if timer - g_obj_timer >= OBJECTS_ANIMATION_MS then
            animate_objects()
            g_obj_timer=timer
        end if

        ' Map rendering
        page write 0
        blit 0,TILE_SIZE*2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SCREEN_BUFFER
        page write SCREEN_BUFFER

        ' Move sprites
        move_shots()
        move_and_process_objects()
        ' Spawn enqueued objects
        process_spawn_queue()
        ' Move player - ensure player always on top
        sprite show safe #1, g_player(0),g_player(1), 1,,1
        sprite move
    loop
end sub

'
' Check the collision interruption
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

'
' Process the collision
sub process_collision(sprite_id%)
    local i%, collided_id%
    local o

    for i% = 1 to sprite(C, sprite_id%)
        collided_id%=sprite(C, sprite_id%, i%)
        if collided_id% > 63 then continue for

        ' Check player collision
        if sprite_id% = 1 or collided_id% = 1 then
            hit_player(collided_id%)

        ' Check shot hit
        else if sprite_id% <= SHOTS_NUM% or collided_id% <= SHOTS_NUM% then
            ' Player shot hits a block
            if sprite_id% <= 4 and collided_id% >= BLOCK_INI_SPRITE_ID then
                if hit_block(collided_id%) <= BLOCK_HITS then
                    destroy_shot(sprite_id%)
                end if

            ' Player shot hits an enemy
            else if sprite_id% <= 4 and collided_id% >= OBJ_INI_SPRITE_ID then
                hit_enemy(collided_id%)
                destroy_shot(sprite_id%)
            end if
            inc o,16
        end if
    next
end sub

'
' Spawn new objects from the spawn queue
sub process_spawn_queue()
    local i%, x%, y%, l%, time_ms, obj_id%,sprite_id%

    ' Spawn new objects from the queue
    for i%=0 to bound(g_spawn_queue())
        if g_spawn_queue(i%, 0) = 0 then continue for
        if timer < g_spawn_queue(i%, 4) then continue for

        obj_id%=g_spawn_queue(i%, 1)
        ' Objects and enemies
        if obj_id% < 31 then
            select case obj_id%
                case 2
                    time_ms=BAT_SPAWN_SPEED_MS
                case 3
                    time_ms=BAT_WAVE_SPAWN_SPEED_MS
            end select
            spawn_object(obj_id%, g_spawn_queue(i%, 2), g_spawn_queue(i%, 3))

        ' Blocks
        else
            ' Calculate the block index
            obj_id%=31-obj_id%
            ' Calculate the sprite index
            sprite_id%=BLOCK_INI_SPRITE_ID+obj_id%
            ' Calculate the object id. Block id + block type
            obj_id%=31+g_blocks(obj_id%,0)
            ' Save sprite position and layer
            x%=sprite(X, sprite_id%)
            y%=sprite(Y, sprite_id%)
            l%=sprite(L, sprite_id%)
            ' Replace buffer tiles
            sprite hide safe sprite_id%
            blit OBJ_TILE%(obj_id%,0),OBJ_TILE%(obj_id%,1), x%, y%, OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
            sprite show safe sprite_id%, x%, y%, l%
        end if

        ' Decrement spawn count
        inc g_spawn_queue(i%, 0), -1
        g_spawn_queue(i%, 4)=timer+time_ms
    next
end sub

'
' Process the player hit
' TODO: implement!!!
sub hit_player(collided_id%)
end sub

'
' Destroy the shot sprite
sub destroy_shot(sprite_id%)
    sprite hide safe sprite_id%
    sprite close sprite_id%
    g_shots(sprite_id% - 2, 0) = 0
end sub

'
' Process block hit
function hit_block(sprite_id%) as integer
    local i%=sprite_id%-BLOCK_INI_SPRITE_ID
    ' Increment hits
    inc g_blocks(i%,1), 1
    hit_block = g_blocks(i%,1)

    if g_blocks(i%,1)=BLOCK_HITS then
        debug_print("Ix: " + str$(i%) + ", Tp: " + str$(g_blocks(i%,0)) + ", H: " + str$(g_blocks(i%,1)))
        enqueue_object_spawn(1, 31+i%, sprite(X, sprite_id%), sprite(Y, sprite_id%))
    end if
end function

'
' Process the enemy hit
sub hit_enemy(enemy_sprite_id%)
    local i%,sprite_id%
    local o=16

    ' Delete the enemy
    for i%=0 to bound(g_obj())
        sprite_id%=OBJ_INI_SPRITE_ID + i%
        if sprite_id% <> enemy_sprite_id% then continue for

        ' Decrement life
        inc g_obj(i%,3), -1 ' TODO: Implement weapon force
        if g_obj(i%,3) > 0 then continue for

        ' Delete enemy's object
        g_obj(i%,0) = 0
        sprite hide safe sprite_id%
        sprite close sprite_id%
        inc o,16

        delete_shadow(i%)
        start_enemy_death_animation(i%)
        if g_sound_on% then play effect ENEMY_DEATH_EFFECT
        exit for
    next
end sub

'
' Initialize and start the enemy death animation
sub start_enemy_death_animation(i%)
    enqueue_object_spawn(1, 20, g_obj(i%,1), g_obj(i%,2))
end sub

'
' Delete an object shadow
sub delete_shadow(src_obj_index%)
    local shadow_index%=g_obj(src_obj_index%,6)

    if shadow_index% < 0 then exit sub

    g_obj(shadow_index%,0) = 0
    g_obj(src_obj_index%,6) = -1
    sprite hide safe OBJ_INI_SPRITE_ID + shadow_index%
    sprite close OBJ_INI_SPRITE_ID + shadow_index%
end sub

'
' Animate the game objects (enemies, power-ups)
sub animate_objects()
    local i%, obj_id%, sprite_index%, offset%, flip%
    inc g_anim_tick%

    for i%=0 to bound(g_obj())
        obj_id% = g_obj(i%, 0)
        ' Skip free slots and shadows
        if obj_id% = 0 or obj_id%=39 then continue for

        offset%=0
        sprite_index%=OBJ_INI_SPRITE_ID + i%

        ' Config animation
        select case obj_id%
            case 4  ' Knight
                if g_anim_tick% mod ANIM_TICK_DIVIDER < HALF_ANIM_TICK_DIVIDER then flip%=1

            case 20 ' Fire
                if g_obj(i%, 3) > bound(FIRE_ANIM()) then
                    g_obj(i%, 0)=0
                    sprite hide safe sprite_index%
                    sprite close sprite_index%
                    continue for
                end if
                offset%=FIRE_ANIM(g_obj(i%, 3))*TILE_SIZEx2
                inc g_obj(i%, 3),1

            case else ' Other objects
                if g_anim_tick% mod ANIM_TICK_DIVIDER < HALF_ANIM_TICK_DIVIDER then offset%=TILE_SIZEx2
        end select

        ' Read or flip next frame's sprite
        select case obj_id%
            case 4
                sprite show safe sprite_index%, sprite(X, sprite_index%), sprite(Y, sprite_index%), sprite(L, sprite_index%), flip%
            case else
                sprite read sprite_index%, OBJ_TILE%(obj_id%,0)+offset%, OBJ_TILE%(obj_id%,1), OBJ_TILE%(obj_id%,2), OBJ_TILE%(obj_id%,3), OBJ_TILES_BUFFER
        end select
    next
end sub

'
' Move screen shots
sub move_shots()
    local i%
    for i%=0 to bound(g_shots())
        ' Checks weapon id
        if not g_shots(i%,0) then continue for
        ' Moves shot
        inc g_shots(i%,1),g_shots(i%,3)
        inc g_shots(i%,2),g_shots(i%,4)
        if g_shots(i%,2) < 0 then
            g_shots(i%,0)=0
            sprite hide safe i%+2
            sprite close i%+2
        else
            sprite next i%+2,g_shots(i%,1),g_shots(i%,2)
        end if
    next
end sub

'
' Move screen objects
sub move_and_process_objects()
    local i%, sprite_id%, obj_id%, screen_offset%, offset_y%

    for i%=0 to bound(g_obj())
        offset_y%=0
        obj_id%=g_obj(i%,0)

        ' Checks object id
        if not obj_id% then continue for

        screen_offset%=TILE_SIZE

        ' Moves object
        select case obj_id%
            case 2,3 ' Bat
                ' Calculate new X
                ' Simple bat
                if obj_id%=2 then
                    if g_obj(i%,1) > SCREEN_CENTER then
                        g_obj(i%,1)=SCREEN_WIDTH+SCREEN_CENTER*cos(g_obj(i%,4))
                    else
                        g_obj(i%,1)=(SCREEN_CENTER-TILE_SIZE)*cos(g_obj(i%,4))
                    end if
                else ' Bat wave
                    g_obj(i%,1)=(SCREEN_CENTER-TILE_SIZE)+(SCREEN_CENTER-TILE_SIZEx4)*cos(g_obj(i%,4))
                end if

                ' Increment angle by the speed
                inc g_obj(i%,4),OBJ_DATA(obj_id%,0)
                ' Increment Y
                inc g_obj(i%,2),OBJ_DATA(obj_id%,1)
                ' Compensates sprite's tile misalignment
                if g_anim_tick% mod ANIM_TICK_DIVIDER >= HALF_ANIM_TICK_DIVIDER then offset_y%=OBJ_TILE%(obj_id%,3)/2

            case 4 ' Knight
                ' Check if it is time to change direction
                if g_obj(i%,4)=0 and g_player(1)-g_obj(i%,2) < KNIGHT_CHANGE_DIRECT_DIST_PX then
                    g_obj(i%,4)=g_obj(i%,1) ' Save the last X position
                    g_obj(i%,5)=choice(g_obj(i%,1)>SCREEN_CENTER,-1,1) ' Horizontal movement direction
                else if g_obj(i%,5) and abs(g_obj(i%,4)-g_obj(i%,1)) > KNIGHT_MAX_HORIZONTAL_DIST_PX then
                    g_obj(i%,5)=0
                end if

                if g_obj(i%,5) then
                    inc g_obj(i%,1),OBJ_DATA(obj_id%,0)*g_obj(i%,5)
                else
                    inc g_obj(i%,2),OBJ_DATA(obj_id%,1)
                end if
            case 39 ' Shadow
                g_obj(i%,1)=g_obj(g_obj(i%,6),1)
                g_obj(i%,2)=g_obj(g_obj(i%,6),2)+g_obj(i%,4)
                screen_offset%=-TILE_SIZE

            case else
                inc g_obj(i%,1),OBJ_DATA(obj_id%,0)
                inc g_obj(i%,2),OBJ_DATA(obj_id%,1)
        end select

        ' Move or destroy the sprite if out of bounds
        sprite_id%=OBJ_INI_SPRITE_ID + i%
        if not sprite(e,sprite_id%) and g_obj(i%,2) <= SCREEN_HEIGHT then
            sprite next sprite_id%, g_obj(i%,1), g_obj(i%,2)+offset_y%
        else if obj_id% = 39 then ' Shadow
            delete_shadow(g_obj(i%,6))
        else
            g_obj(i%,0)=0
            sprite hide safe sprite_id%
            sprite close sprite_id%
        end if
    next
end sub

'
' Spawn a new object and initialize its state and sprite
sub spawn_object(obj_id%, x%, y%, map_data%)
    local sprite_id%, offset_x%, offset_y%, layer%=1, i%=get_free_object_slot()
    if i% < 0 then exit sub

    g_obj(i%,0)=obj_id%             ' Object Id
    g_obj(i%,1)=x%                  ' X
    g_obj(i%,2)=y%                  ' Y
    g_obj(i%,3)=OBJ_DATA(obj_id%,2) ' Life
    g_obj(i%,4)=OBJ_DATA(obj_id%,3) ' GPR 1
    g_obj(i%,5)=OBJ_DATA(obj_id%,4) ' GPR 2
    g_obj(i%,6)=-1                  ' Shadow index

    select case obj_id%
        case 2,3 ' Bat
            ' Calculate the correct bat angle
            if x% < SCREEN_CENTER then g_obj(i%,4)=g_obj(i%,4)+180
            ' Initialize bat spawning data
            if map_data% then
                enqueue_object_spawn(map_data%, obj_id%, x%, y%, choice(obj_id%=2, BAT_SPAWN_SPEED_MS, BAT_WAVE_SPAWN_SPEED_MS))
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

'
' Spawn a map block
sub spawn_block(x%, y%, type%)
    local i%, block_id%, row%, col%
    for i%=0 to bound(g_blocks())
        if g_blocks(i%, 0) = 0 then exit for
    next

    block_id%=31
    g_blocks(i%,0)=type% ' Block type
    g_blocks(i%,1)=0     ' Hits

    sprite read BLOCK_INI_SPRITE_ID + i%, OBJ_TILE%(block_id%,0), OBJ_TILE%(block_id%,1), OBJ_TILE%(block_id%,2), OBJ_TILE%(block_id%,3), OBJ_TILES_BUFFER
    sprite show safe BLOCK_INI_SPRITE_ID + i%, x%,0, 1
end sub

'
' Enqueues an object spawn
sub enqueue_object_spawn(spawn_count%, obj_id%, x%, y%, spawn_speed_ms)
    local i%
    for i%=0 to bound(g_spawn_queue())
        if g_spawn_queue(i%,0) then continue for

        g_spawn_queue(i%,0)=spawn_count% ' Spawn count
        g_spawn_queue(i%,1)=obj_id%      ' Object Id
        g_spawn_queue(i%,2)=x%           ' X
        g_spawn_queue(i%,3)=y%           ' Y
        g_spawn_queue(i%,4)=timer+spawn_speed_ms
        exit for
    next
end sub

'
' Create the object shadow
sub create_shadow(obj_index%, height%)
    local sprite_id%, i%=get_free_object_slot()
    if i% < 0 then exit sub

    g_obj(i%,0)=39                          ' Shadow id
    g_obj(i%,1)=g_obj(obj_index%,1)         ' X
    g_obj(i%,2)=g_obj(obj_index%,2)+height% ' Y
    g_obj(i%,4)=height%                     ' Height
    g_obj(i%,6)=obj_index%                  ' Shadow -> source object index
    g_obj(obj_index%,6)=i%                  ' Source object -> shadow index
    sprite_id%=OBJ_INI_SPRITE_ID + i%
    sprite read sprite_id%, OBJ_TILE%(39,0), OBJ_TILE%(39,1), OBJ_TILE%(39,2), OBJ_TILE%(39,3), OBJ_TILES_BUFFER
    sprite show safe sprite_id%, g_obj(i%,1),g_obj(i%,2), 3
end sub

'
' Find a free slot in the objects data array
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

'
' Create the shot sprite
sub fire()
    ' Cooldown
    if timer - g_shot_timer < g_player_shot_ms then exit sub

    local i%
    for i%=0 to choice(g_player(3)>5,2,1)
        ' Ignore used slots
        if g_shots(i%,0) then continue for

        ' Create the shot state
        g_shots(i%,0)=g_player(3)                   ' weapon
        g_shots(i%,1)=g_player(0)+OBJ_TILE%(40,2)/2 ' X
        g_shots(i%,2)=g_player(1)-OBJ_TILE%(40,3)-1 ' Y
        g_shots(i%,3)=0                             ' Speed X
        g_shots(i%,4)=-2.2                          ' Speed Y
        ' Create the shot sprite
        sprite read i%+2, OBJ_TILE%(40,0), OBJ_TILE%(40,1), OBJ_TILE%(40,2), OBJ_TILE%(40,3), OBJ_TILES_BUFFER
        sprite show safe i%+2, g_shots(i%,1),g_shots(i%,2), 1
        ' Play SFX
        if g_sound_on% then play effect SHOT_EFFECT
        ' Increment timer
        g_shot_timer=timer
        exit for
    next
end sub

'
' Process the player movement
sub move_player(direction%)
    local x=g_player(0), y=g_player(1)

    select case direction%
        case KB_LEFT
            inc g_player(0), -g_player(4)
            if map_collide(g_player()) then g_player(0)=x
            if g_player(0) < 0 then g_player(0)=SCREEN_WIDTH - TILE_SIZE * 2
            check_scroll_collision()
        case KB_RIGHT
            inc g_player(0), g_player(4)
            if map_collide(g_player()) then g_player(0)=x
            if g_player(0) > SCREEN_WIDTH - TILE_SIZE * 2 then g_player(0)=0
            check_scroll_collision()
        case KB_UP
            inc g_player(1), -g_player(4)
            if map_collide(g_player()) then g_player(1)=y
            if g_player(1) < TILE_SIZE * 6 then g_player(1)=TILE_SIZE * 6
            check_scroll_collision()
        case KB_DOWN
            inc g_player(1), g_player(4)
            if map_collide(g_player()) then g_player(1)=y
            if g_player(1) > SCREEN_HEIGHT then g_player(1)=SCREEN_HEIGHT
            check_scroll_collision()
    end select
end sub

'
' Initialize player state and sprite
sub init_player()
    g_player(0)=SCREEN_WIDTH/2-TILE_SIZE  ' x
    g_player(1)=SCREEN_HEIGHT-TILE_SIZE*3 ' y
    g_player(2)=0                         ' animation counter
    g_player(3)=1                         ' weapon
    g_player(4)=0.6                       ' speed

    sprite read #1, PLAYER_SKIN1_X_L,PLAYER_SKIN_Y, TILE_SIZEx2,TILE_SIZEx2, OBJ_TILES_BUFFER
    sprite show safe #1, g_player(0),g_player(1), 1,,1
end sub

'
' Check the player collision after map scroll
' Moves the player down in case of collision
sub check_scroll_collision()
    if map_collide(g_player()) then
        inc g_player(1),2
        'TODO: Implement player kill by crush
    end if
end sub

'
' Scrolls the map
sub scroll_map()
    local i%, sprite_id%

    ' Blocks scroll
    for i%=0 to bound(g_blocks())
        if g_blocks(i%,0)=0 then continue for
        sprite_id%=BLOCK_INI_SPRITE_ID + i%
        sprite next sprite_id%, sprite(X, sprite_id%), sprite(Y, sprite_id%) + 1
    next

    ' Sprite scroll 0,-1
    sprite scrollr 0,0,SCREEN_WIDTH,SCREEN_HEIGHT+TILE_SIZE*2,0,-1
    check_scroll_collision()
    inc g_tile_px%,1

    ' Draw the next map tile row
    if g_tile_px% = TILE_SIZE then
        g_tile_px%=0
        inc g_row%,-1
        ' Draw the next map row on the top of the screen buffer
        if g_row% >= 0 then
            draw_map_row_and_spawn_objects(g_row%)
        end if
    end if
end sub

'
' Process keyboard keys
sub process_kb()
    local kb1%=KeyDown(1), kb2%=KeyDown(2), kb3%=KeyDown(3)

    if not kb1% and not kb2% and not kb3% then
        g_kb_released%=true
        exit sub
    end if

    if kb1%=KB_LEFT or kb2%=KB_LEFT or kb3%=KB_LEFT then
        move_player(KB_LEFT)
    else if kb1%=KB_RIGHT or kb2%=KB_RIGHT or kb3%=KB_RIGHT then
        move_player(KB_RIGHT)
    end if

    if kb1%=KB_UP or kb2%=KB_UP or kb3%=KB_UP then
        move_player(KB_UP)
    else if kb1%=KB_DOWN or kb2%=KB_DOWN or kb3%=KB_DOWN then
        move_player(KB_DOWN)
    end if

    if g_kb_released% and (kb1%=KB_SPACE or kb2%=KB_SPACE or kb3%=KB_SPACE) then
        fire()
        g_kb_released%=false
    else if kb1%<>KB_SPACE and kb2%<>KB_SPACE and kb3%<>KB_SPACE then
        g_kb_released%=true
    end if
end sub

'
' Checks the player collision against solid tiles
function map_collide(player()) as integer
    local col%=player(0)\TILE_SIZE
    local row%=(player(1)-g_tile_px%)\TILE_SIZE+g_row%
    ' Check top left
    map_collide=g_map((row%+1)*MAP_COLS+col%) and &H80
    ' Check top right
    if not map_collide then map_collide=g_map((row%+1)*MAP_COLS+(col%+2)) >> 7 and 1
    ' Check bottom left
    if not map_collide then map_collide=g_map((row%+1)*MAP_COLS+col%) >> 7 and 1
    ' Check bottom right
    if not map_collide then map_collide=g_map((row%+1)*MAP_COLS+(col%+2)) >> 7 and 1
    ' TODO: Check horizontal wrapping collision
end function

'
' Initialize the game
sub init()
    ' screen mode
    font 1,1
    mode 7,12 ' 320x240
    'mode 4,12 ' debug mode

    ' init screen and tiles buffers
    page write SCREEN_BUFFER:cls

    page write MAP_TILES_BUFFER:cls
    load png MAP_TILESET_IMG

    page write OBJ_TILES_BUFFER:cls
    load png OBJ_TILESET_IMG

    ' clear screen
    page write 0: cls
    ' init game state variables
    g_tile_px%=0
end sub

'
' Initialize the given stage
sub init_stage(stage%)
    g_kb_released%=true
    load_map(stage%)
    init_map_tiles(stage%)
    initialize_screen_buffer()

    sprite interrupt check_collision

    page write 0: cls
    page write SCREEN_BUFFER
    init_player()

    if g_sound_on% then play modfile MUSIC$(stage%), 16000

    timer=0
end sub

'
' Draw map row to the top of the screen buffer
' and create objects sprites
sub draw_map_row_and_spawn_objects(row%)
    local tile%,col%,tx%,ty%,obj_id%,tile_data%,extra%

    page write SCREEN_BUFFER
    for col%=MAP_COLS_0 to 0 step -1
        ' Tile drawing
        tile_data%=g_map(row%*MAP_COLS+col%)
        tile%=(tile_data% AND &H7F) - 1
        tx%=(tile% mod TILES_COLS) * TILE_SIZE
        ty%=(tile% \ TILES_COLS) * TILE_SIZE
        blit tx%,ty%, col%*TILE_SIZE,0, TILE_SIZE,TILE_SIZE, MAP_TILES_BUFFER

        ' Create object
        obj_id%=(tile_data% AND &H1F00) >> 8
        if obj_id% then
            extra%=(tile_data% AND &HE000) >> 13
            select case obj_id%
                case 31 ' Block
                    spawn_block(col%*TILE_SIZE, 0, extra%)
                case else
                    spawn_object(obj_id%, col%*TILE_SIZE, 0, extra%)
            end select
        end if
    next
    page write 0
end sub

'
' Draw initial screen to the screen buffer
sub initialize_screen_buffer()
    local row%

    page write SCREEN_BUFFER
    for row%=MAP_ROWS_0 to MAP_ROWS_0 - (SCREEN_ROWS + 1) step -1
        draw_map_row_and_spawn_objects(row%)
        page scroll SCREEN_BUFFER,0,-TILE_SIZE
    next
    g_row%=row%+1
    page scroll SCREEN_BUFFER,0,TILE_SIZE
end sub

'
' Load the map to the map global array g_map
sub load_map(stage%)
    local i%=0, value%, solid_bit%
    local file_name$ = MAPS_DIR + "stage" + str$(stage%) + ".map"
    open file_name$ for input as #1
    do while not eof(1)
        value%=asc(input$(1, #1))
        solid_bit%=value% and &H80
        ' Recalculates the stage tiles position. 96 tiles per stage
        value%=(value% and &H7F) + (96 * (stage% - 1))
        g_map(i%)=((solid_bit% or value%) << 8) or asc(input$(1, #1))
        inc i%
    loop
    close #1
end sub

'
' Update the map tiles position moving the current stage tiles
' to the 0,0 buffer position. Each stage has 3 tiles rows.
sub init_map_tiles(stage%)
    page scroll MAP_TILES_BUFFER, 0, (stage% - 1) * 3 * TILE_SIZE
end sub
'
' Show the game intro
sub intro()
    local kb%
    do while kb% <> KB_SPACE
        kb%=KeyDown(1)
        if kb% = KB_ESC then mode 1:end
    loop
end sub

sub debug_print(msg$, offset%)
    page write 0
    print @(0,184+offset%) msg$
    page write SCREEN_BUFFER
end sub
