' Global variables
dim g_sound_on%=1
dim g_map(MAP_SIZE) as integer ' Map data
dim g_row%                     ' Current top map row. Zero is the bottom row.
dim g_prev_frame_timer         ' Previous frame timer value
dim g_delta_time               ' Frame delta time
dim g_tile_px%                 ' The rendered tile row pixel
dim g_kb_released%             ' Tracks kb keys releasing
dim g_stage%                   ' Current stage number
dim g_enemies_count%           ' Total enemies at the screen
' Timers
dim g_scroll_timer             ' Scroll timer
dim g_player_timer             ' Player timer
dim g_anim_timer                ' Objects timer
dim g_pshot_timer              ' Player shot timer
dim g_eshot_timer              ' Enemies shots timer
dim g_freeze_timer=-1          ' Enemies freeze timer
dim g_power_up_timer=-1        ' Power-up timer

' Player state: 0: x | 1: y | 2: animation counter | 3: weapon | 4: speed | 5: power up | 6: shield | 7: lives | 8: state
' Weapons:
'  0 - No shot
'  1 - Arrow
'  2 - Twin arrows
'  3 - Triple flames
'  4 - Boomerang
'  5 - Sword
'  6 - Fire arrow
'  7 - Twin arrows (level 2)
'  8 - Triple flames (level 2)
'  9 - Boomerang (level 2)
' 10 - Double Sword (level 2)
' 11 - Fire arrow (level 2)
' State:
'  0 - Playing
'  1 - Frozen
'  2 - Pause
'  3 - Moving to ending portal
'  4 - Dead
'  5 - Ready for next stage
dim g_player(8)
dim g_player_is_moving=false
dim g_player_shot_ms=SHOT_COOLDOWN_MS
dim g_score%=0
dim g_hiscore%=0

' Boss: 0: status | 1: body index | 2 to 7 - hit box indexes
' Status:
' 0 - Boss not spawned
' 1 - Waiting start
' 2 - Active
dim g_boss(7)

' Supports 13 shots at the same time: 0: id | 1: x | 2: y | 3: speed x | 4: speed y
' The three first slots are for the player shots, the other ones for enemies
dim g_shots(12,4)
const SHOTS_NUM%=bound(g_shots())
' Object data: 0: obj id | 1: x | 2: y | 3: life | 4: GPR 1 | 5: GPR 2 | 6: shadow
dim g_obj(20,6)
' Actions data queue: 0: num of executions | 1: action id | 2: GPR 1 | 3: GPR 2 | 4: GPR 3 | 5: next execution time
dim g_actions_queue(bound(g_obj()),5)
' Blocks: 0: type | 1: hits
dim g_blocks(9,1)
dim g_anim_tick%=0

dim g_cont_sfx$ ' Continuous SFX

' Sprites 1 = Player and 2 = Shield
' Initial sprite Id for shots. From 3 to 15
const SHOTS_INI_SPRITE_ID=3
' Initial sprite Id for objects. From 16 to 36
const OBJ_INI_SPRITE_ID=SHOTS_INI_SPRITE_ID+bound(g_shots()) + 1
' Initial sprite Id for blocks. From 37 to 46
const BLOCK_INI_SPRITE_ID=OBJ_INI_SPRITE_ID+bound(g_obj()) + 1
