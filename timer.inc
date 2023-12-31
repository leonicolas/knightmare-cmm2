
sub process_freeze_timer()
    local fraction%=fix((g_freeze_timer - fix(g_freeze_timer)) * 100)
    print_freeze_timer(g_freeze_timer)
    if fraction% = 0 or (g_freeze_timer < 4 and fraction% = 50) then play_sfx("FREEZE_TICK_SFX")

    inc g_freeze_timer, -1 * g_delta_time
    if g_freeze_timer < 0 then
        clear_freeze_timer()
    end if
end sub

sub process_power_up_timer()
    print_power_up_timer(g_power_up_timer)
    inc g_power_up_timer, -0.025
    if g_power_up_timer < 11 and fix(g_power_up_timer*100) = fix(g_power_up_timer)*100 then
        play_sfx("POWER_UP_ENDING")
    end if
    if g_power_up_timer < 0 then
        ' Checks if the player has a shield
        g_player(5)=choice(g_player(6) > 0, 1, 0)
        clear_power_up_timer()
    end if
end sub
