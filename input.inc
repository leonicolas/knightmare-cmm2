sub process_input()
    static fire_down% = false
    if g_player(8) then exit sub
    g_player_is_moving=false
    local ctrl% = read_keyboard%()

    if not ctrl% then ctrl% = read_gamepad%()
    if not ctrl% then fire_down% = false : exit sub

    if ctrl% and CTRL_FIRE then
      if not fire_down% then
        fire()
        fire_down% = true
      endif
    else
      fire_down% = false
    endif

    if ctrl% and CTRL_LEFT then
        move_player(KB_LEFT)
    elseif ctrl% and CTRL_RIGHT then
        move_player(KB_RIGHT)
    endif
    if ctrl% and CTRL_UP then
        move_player(KB_UP)
    elseif ctrl% and CTRL_DOWN then
        move_player(KB_DOWN)
    endif
end sub

function read_keyboard%()
    if not keydown(0) then exit function
    local i%
    for i% = 1 to 3
        if not keydown(i%) then continue for
        select case keydown(i%)
        Case KB_UP
            read_keyboard% = read_keyboard% Or CTRL_UP
        Case KB_DOWN
            read_keyboard% = read_keyboard% Or CTRL_DOWN
        Case KB_LEFT
            read_keyboard% = read_keyboard% Or CTRL_LEFT
        Case KB_RIGHT
            read_keyboard% = read_keyboard% Or CTRL_RIGHT
        Case KB_SPACE
            read_keyboard% = read_keyboard% Or CTRL_FIRE
        End Select
    Next
end function

function read_gamepad%()
    if not g_gamepad_on% then exit function

    read_gamepad% = gamepad(B)
    if (read_gamepad% and BUTTON_B) then read_gamepad% = CTRL_FIRE

    ' No digital direction buttons down so check left-stick.
    if not read_gamepad% then
        select case gamepad(LX)
            case < 124: read_gamepad% = read_gamepad% Or CTRL_LEFT
            case > 132: read_gamepad% = read_gamepad% Or CTRL_RIGHT
        end select
        select case gamepad(LY)
            case < 124: read_gamepad% = read_gamepad% Or CTRL_UP
            case > 132: read_gamepad% = read_gamepad% Or CTRL_DOWN
        end select
    end if
end function
