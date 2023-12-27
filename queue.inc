
sub enqueue_action(exec_count%, action_id%, gpr1, gpr2, gpr3, next_exec_ms)
    local i%
    for i%=0 to bound(g_actions_queue())
        if g_actions_queue(i%,0) then continue for

        g_actions_queue(i%,0)=exec_count% ' Execution count
        g_actions_queue(i%,1)=action_id%  ' Action Id
        g_actions_queue(i%,2)=gpr1        ' GPR 1
        g_actions_queue(i%,3)=gpr2        ' GPR 2
        g_actions_queue(i%,4)=gpr3        ' GPR 3
        g_actions_queue(i%,5)=timer + next_exec_ms
        exit for
    next
end sub

sub process_actions_queue()
    local i%, time_ms, obj_id%

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

            case else ' Objects and enemies
                if obj_id% = 2 then
                    time_ms=BAT_SPAWN_SPEED_MS
                else if obj_id% = 3 then
                    time_ms=BAT_WAVE_SPAWN_SPEED_MS
                end if
                spawn_object(obj_id%, g_actions_queue(i%, 2), g_actions_queue(i%, 3))
        end select

        ' Decrement execution count
        inc g_actions_queue(i%, 0), -1
        g_actions_queue(i%, 5)=timer+time_ms
    next
end sub
