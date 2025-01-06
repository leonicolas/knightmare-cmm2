Sub process_input()
    Static fire_down% = false
    if g_player(8) then exit sub
    g_player_is_moving=false
    Local ctrl% = read_keyboard%()
    If Not ctrl% Then ctrl% = read_gamepad%()
    If Not ctrl% Then fire_down% = false : Exit Sub

    If ctrl% And CTRL_FIRE Then
      If Not fire_down% Then
        fire()
        fire_down% = true
      EndIf
    Else
      fire_down% = false
    EndIf

    If ctrl% And CTRL_LEFT Then
        move_player(KB_LEFT)
    ElseIf ctrl% And CTRL_RIGHT Then
        move_player(KB_RIGHT)
    EndIf
    If ctrl% And CTRL_UP Then
        move_player(KB_UP)
    ElseIf ctrl% And CTRL_DOWN Then
        move_player(KB_DOWN)
    EndIf
End Sub

Function read_keyboard%()
  If Not KeyDown(0) Then Exit Function
  Local i%
  For i% = 1 To 3
    Select Case KeyDown(i%)
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
End Function

Function read_gamepad%()
  if not g_gamepad_on% then exit function

  read_gamepad% = gamepad(B)
  if not read_gamepad% Then exit function

  If Not &h1E0 Then
    ' No digital direction buttons down so check left-stick.
    Select Case gamepad(LX)
      Case < 124: read_gamepad% = read_gamepad% Or CTRL_LEFT
      Case > 132: read_gamepad% = read_gamepad% Or CTRL_RIGHT
    End Select
    Select Case gamepad(LY)
      Case < 124: read_gamepad% = read_gamepad% Or CTRL_UP
      Case > 132: read_gamepad% = read_gamepad% Or CTRL_DOWN
    End Select
  EndIf
End Function
