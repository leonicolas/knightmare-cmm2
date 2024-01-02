sub show_stage_screen(stage%)
    page write 1: cls 0
    page write 0: cls 0
    print_status()
    print_stage(stage%, "STAGE ", true)
end sub
