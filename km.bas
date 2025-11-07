option explicit
option base 0
option angle degrees
option default float

If Mm.Device$ = "MMB4L" Then Option Simulate "MMB4W"

#include "constants.inc"
#include "global.inc"
#include "screen.inc"
#include "boss.inc"
#include "collision.inc"
#include "init.inc"
#include "input.inc"
#include "map.inc"
#include "music.inc"
#include "objects.inc"
#include "power_ups.inc"
#include "print.inc"
#include "queue.inc"
#include "timer.inc"

Option Break 4
On Key 3, end_game()

init_game()

If Mm.Info(Device X) = "MMB4L" Then
    Graphics Title 0, "Knightmare for MMBasic, v0.1.0"
    Graphics Interrupt 0, on_window_event
EndIf

show_intro()
show_menu()

sub start_game()
    local first_stage%=true
    init_player(2)
    g_row%=MAP_ROWS_0
    g_score% = 0

    ' Dev variables
    ' g_stage%=8     ' Stage
    ' g_row%=0       ' Position into the map
    ' g_player(3)=11 ' Best weapon
    ' g_player(6)=30 ' Shield with support to 30 hits
    ' End Dev variables
    do
        timer=0
        show_stage_screen(g_stage%)
        select case g_player(8)
            case 5 ' Start stage
                do while g_player(8) = 5
                    if timer > 4000 then
                        if g_stage% = 9 then
                            g_player(5)=0 ' No power-ups for final cutscene
                            g_player(6)=0 ' No shield for final cutscene
                            g_player(8)=7 ' Final cutscene state
                        else
                            g_player(8)=0
                        end if
                    end if
                loop
            case 6 ' Player died
                if g_player(7) < 0 then
                    game_over()
                    exit do
                end if
                calculate_start_row()
                do while g_player(8) = 6
                    if timer > 2000 then g_player(8)=0
                loop
            case > 10 ' Game finished
                play stop
                exit do
        end select
        first_stage%=false
        run_stage()

        ' Next stage
        if g_player(8) = 5 then
            inc g_stage%
            g_row%=choice(g_stage%=9,0,MAP_ROWS_0)
        end if
    loop
end sub

Sub on_window_event(window_id%, event_id%)
  If event_id% = WINDOW_EVENT_CLOSE Then end_game()
End Sub

Sub end_game()
  Option Break 3
  If InStr(Mm.CmdLine$, "--shell") Then sys.run_shell()
  End
End Sub

Function sys.HOME$()
  Select Case Mm.Info$(Device X)
    Case "MMB4L"
      sys.HOME$ = Mm.Info$(EnvVar "HOME")
    Case "MMBasic for Windows"
      sys.HOME$ = Mm.Info$(EnvVar "HOMEDIR") + Mm.Info$(EnvVar "HOMEPATH")
    Case Else
      sys.HOME$ = "A:"
  End Select
End Function

' Runs the program specified by the '.mmbasic-shell' file.
'
' @param  fnbr%  File number to use for accessing the file, if unspecified/0 then uses #9.
Sub sys.run_shell(fnbr%)
  Const prog$ = sys.read_shell_file$(1, fnbr%)
  If prog$ <> "" Then Run prog$
End Sub

' Reads the '.mmbasic-shell' file.
'
' @param  exist%  If 1 then ERROR if file does not exist, otherwise return empty string.
' @param  fnbr%   File number to use for accessing the file, if unspecified/0 then uses #9.
' @return         The contents of the first line of the file.
Function sys.read_shell_file$(exist%, fnbr%)
  Const f$ = sys.HOME$() + "/.mmbasic-shell"
  If Not exist% And Not Mm.Info(Exists f$) Then Exit Function
  Const _fnbr% = Choice(fnbr%, fnbr%, 9)
  Open f$ For Input As _fnbr%
  Line Input #_fnbr%, sys.read_shell_file$
  Close _fnbr%
End Function

sub run_stage()
    local on_top%
    init_stage()

    do
        ' Game tick
        if timer - g_prev_frame_timer < GAME_TICK_MS then continue do
        g_delta_time=(timer-g_prev_frame_timer)/1000
        g_prev_frame_timer=timer
        inc g_timer
        'print_debug("FPS: "+str$(1/g_delta_time))
        page write SPRITES_BUFFER

        ' Scrolls the map
        if g_freeze_timer < 0 and g_row% >= -1 and g_timer mod 16 = 0 then scroll_map()

        ' Process keyboard and game pad
        process_input()

        ' Auto move player
        if g_player(8)=3 then auto_move_player_to_portal()

        ' Process animations
        if g_timer mod 6 = 0 then
            inc g_anim_tick%
            animate_player()
            if g_boss(0)>1 then animate_boss()
            if g_player(8)>=20 then fade_map(20)
            animate_shots()
            animate_objects()
            ' Clean state
            g_fire%=false
        end if

        ' Process enemies shots
        if g_timer mod 250 = 0 then
            if g_stage% > 1 or g_row% < 125 then enemies_fire()
        end if
        ' Process boss shots
        if g_boss(0) > 1 then boss_fire()

        ' Move sprites
        move_shots()
        move_and_process_objects()
        if g_boss(0) > 0 then move_boss()

        ' Spawn enqueued objects
        process_actions_queue()
        sprite move
        ' Move player and shield ensuring always on top
        on_top%=choice(g_player(8) = 3,0,1)
        sprite show safe 1, g_player(0), g_player(1),1,,on_top%
        if g_player(5) = 1 and g_player(6) > 0 then sprite show safe 2, g_player(0), g_player(1)-TILE_SIZE,1,,on_top%

        ' Map and sprites rendering
        page write 0
        blit 0,TILE_SIZEx2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SCREEN_BUFFER
        page write 1
        blit 0,TILE_SIZEx2, SCREEN_OFFSET,0, SCREEN_WIDTH,SCREEN_HEIGHT, SPRITES_BUFFER
        page write SPRITES_BUFFER

        ' Power up timers
        if g_freeze_timer >= 0 then process_freeze_timer()
        if g_power_up_timer >= 0 then process_power_up_timer()

        ' Check player status
        if g_player(8)>4 and g_player(8)<7 or g_player(8)>160 then exit do
    loop

    ' Close all sprites and free memory
    destroy_all()
end sub
