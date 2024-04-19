sub show_stage_screen(stage%)
    page write 1: cls 0
    page write 0: cls 0
    print_status()
    print_stage(stage%, "STAGE ")
end sub

sub show_game_over_screen(stage%)
    page write 1: cls 0
    page write 0: cls 0
    print_centralized("GAME OVER")
end sub
