sub change_weapon(obj_ix%)
    if g_obj(obj_ix%, 0) <> 29 then exit sub

    select case g_obj(obj_ix%, 4)
        case 0 to 2 ' No weapon
            increment_score(1000)
            play_sfx("POINTS")
        case else ' Weapon chrystal
            increment_score(200)
            play_sfx("CHRYSTAL")
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
            play_sfx("POINTS")
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
    play_sfx("CHRYSTAL")
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
        play_sfx("NEW_LIFE")
    else
        play_sfx("PLAYER_DEATH")
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
