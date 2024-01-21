
sub enqueue_action(exec_count%, action_id%, gpr1, gpr2, gpr3, delay_ms)
    local i%
    for i%=0 to bound(g_actions_queue())
        if g_actions_queue(i%,0) then continue for

        g_actions_queue(i%,0)=exec_count% ' Execution count
        g_actions_queue(i%,1)=action_id%  ' Action Id
        g_actions_queue(i%,2)=gpr1        ' GPR 1
        g_actions_queue(i%,3)=gpr2        ' GPR 2
        g_actions_queue(i%,4)=gpr3        ' GPR 3
        g_actions_queue(i%,5)=timer + delay_ms
        g_actions_queue(i%,6)=delay_ms
        exit for
    next
end sub

sub process_actions_queue()
    local i%, obj_id%, color%

    ' Process enqueued actions
    for i%=0 to bound(g_actions_queue())
        if g_actions_queue(i%, 0) = 0 then continue for
        if timer < g_actions_queue(i%, 5) then continue for

        obj_id%=g_actions_queue(i%, 1)

        select case obj_id%
            case 31 ' Blocks
                replace_block(g_actions_queue(i%, 2))

            case 32 ' Shield
                spawn_shield()

            case 100 ' Boss death blinks the screen
                page write 0
                cls choice(g_actions_queue(i%, 0) mod 2, 0, rgb(gray))
                page write SPRITES_BUFFER
                if g_actions_queue(i%, 0) = 1 then destroy_boss()

            case 101 ' Move player to portal
                move_player_to_portal()

            case else ' Objects and enemies
                spawn_object(obj_id%, g_actions_queue(i%, 2), g_actions_queue(i%, 3), g_actions_queue(i%,4))

        end select

        ' Decrement execution count
        inc g_actions_queue(i%, 0), -1
        g_actions_queue(i%, 5)=timer+g_actions_queue(i%,6)
    next
end sub

sub clear_queue()
    local i%
    for i%=0 to bound(g_actions_queue())
        g_actions_queue(i%, 0) = 0
    next
end sub
