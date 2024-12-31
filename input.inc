sub process_kb()
    if g_player(8) then exit sub

    local kb1%=KeyDown(1), kb2%=KeyDown(2), kb3%=KeyDown(3)

    g_player_is_moving=false

    if not kb1% and not kb2% and not kb3% then
        g_kb_released%=true
        exit sub
    end if

    if g_kb_released% and (kb1%=KB_SPACE or kb2%=KB_SPACE or kb3%=KB_SPACE) then
        fire()
        g_kb_released%=false
    else if kb1%<>KB_SPACE and kb2%<>KB_SPACE and kb3%<>KB_SPACE then
        g_kb_released%=true
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
end sub

sub process_gamepad()
    if g_player(8) or not g_gamepad% then exit sub

    local dig_pos=gamepad(LY)
    if g_gamepad% and 128 or (dig_pos >= 0 and dig_pos < 124) then move_player(KB_UP)
    if g_gamepad% and 32  or dig_pos > 130 then move_player(KB_DOWN)

    dig_pos=gamepad(LX)
    if g_gamepad% and 256 or (dig_pos >= 0 and dig_pos < 124) then move_player(KB_LEFT)
    if g_gamepad% and 64  or dig_pos > 130 then move_player(KB_RIGHT)

    if g_gamepad% and 8192 then
        if g_gp_released% then fire()
        g_gp_released%=false
    else if not (g_gamepad% and 8192) then
        g_gp_released%=true
    end if
end sub

sub handle_gamepad()
    g_gamepad%=gamepad(B)
end sub
