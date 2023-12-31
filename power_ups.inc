sub change_weapon(obj_ix%)
    if g_obj(obj_ix%, 0) <> 29 then exit sub

    select case g_obj(obj_ix%, 4)
        case 0 to 2 ' No weapon
            increment_score(1000)
            play_sfx("POINTS_SFX")
        case else ' Weapon chrystal
            increment_score(200)
            play_sfx("CHRYSTAL_SFX")
            local weapon% = g_obj(obj_ix%, 4) - 1
            if weapon% = 5 then increase_player_speed()
            g_player(3)=weapon% + choice(g_player(3)=weapon% or g_player(3)=weapon%+5, 5, 0)
    end select
end sub

sub power_up(obj_ix%)
    if g_obj(obj_ix%, 0) <> 30 then exit sub
    select case g_obj(obj_ix%, 4)
        case 0 to 2 ' Black pill
            increment_score(1000)
            play_sfx("POINTS_SFX")
            exit sub
        case 3 ' Dark blue pill - speed
            increase_player_speed()
        case 4 ' Blue pill - shield
            enqueue_action(1, 32)
        case 5 ' White pill - invisibility
            g_player(5)=2
            g_power_up_timer=POWER_UP_TIME
        case 6 ' Red pill - invincibility
            g_player(5)=4
            g_power_up_timer=POWER_UP_TIME
    end select

    increment_score(200)
    play_sfx("CHRYSTAL_SFX")
end sub

function is_chrystal(obj_sprite_id%) as integer
    local obj_id%=g_obj(obj_sprite_id% - OBJ_INI_SPRITE_ID, 0)
    is_chrystal=(obj_id%=29 or obj_id%=30)
end function

sub increase_player_speed()
    if g_player(4) < PLAYER_MAX_SPEED then inc g_player(4), PLAYER_SPEED_INC
end sub

sub increment_score(points%)
    local i%=g_score%\NEW_LIFE_POINTS
    inc g_score%, points%
    print_score(g_score%)

    if g_score%\NEW_LIFE_POINTS > i% then
        update_life(1)
    end if

    if g_score% > g_hiscore% then
        g_hiscore% = g_score%
        print_hiscore(g_hiscore%)
    end if
end sub

sub update_life(value%)
    if value% >= 0 then
        play_sfx("NEW_LIFE_SFX")
    else
        play_sfx("PLAYER_DEATH_SFX")
    end if
    inc g_player(7), choice(value%, value%, 1)
    print_lives(g_player(7))
end sub

function is_super_weapon() as integer
    select case g_player(3)
        case 3,4,6,8,9,11
            is_super_weapon = true
    end select
end function

function hit_block(sprite_id%) as integer
    local i%=sprite_id% - BLOCK_INI_SPRITE_ID
    local max_hits%=block_max_hits(i%)

    if g_blocks(i%,1) = max_hits% then exit function

    ' Increment hits
    inc g_blocks(i%,1), 1
    hit_block = true

    g_cont_sfx$=""
    if g_blocks(i%,1) < max_hits% then
        play_sfx("BLOCK_HIT_SFX")
    else if g_blocks(i%,1) = max_hits% then
        play_sfx("BLOCK_OPEN_SFX")
    end if

    if g_blocks(i%,1)=1 or g_blocks(i%,1)=max_hits% then
        ' Spawn new block tile
        enqueue_action(1, 31, i%)
    end if
end function

sub destroy_block(block_ix%)
    local sprite_id%=BLOCK_INI_SPRITE_ID + block_ix%
    g_blocks(block_ix%,0)=0
    sprite hide safe sprite_id%
    sprite close sprite_id%
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
