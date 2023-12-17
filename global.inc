' Global variables

dim g_sound_on%=1
dim g_map(MAP_SIZE) as integer ' Map data
dim g_row%                     ' Current top map row. Zero is the bottom row.
dim g_prev_frame_timer         ' Previous frame timer value
dim g_delta_time               ' Frame delta time
dim g_tile_px%                 ' The rendered tile row pixel
dim g_kb_released%             ' Tracks kb keys releasing

' Timers
dim g_scroll_timer             ' Scroll timer
dim g_player_timer             ' Player timer
dim g_anim_timer                ' Objects timer
dim g_pshot_timer              ' Player shot timer
dim g_eshot_timer              ' Enemies shots timer
dim g_freeze_timer=-1          ' Enemies freeze timer
dim g_power_up_timer=-1        ' Power-up timer

' Player state: 0: x | 1: y | 2: animation counter | 3: weapon | 4: speed | 5: power up | 6: shield | 7: lives
dim g_player(7)
dim g_player_is_moving=false
dim g_player_shot_ms=SHOT_COOLDOWN_MS
dim g_score%=0
dim g_hiscore%=0

' Supports 12 shots at the same time: 0: id | 1: x | 2: y | 3: speed x | 4: speed y
' The three first slots are for the player shots, the other ones for enemies
' 0 - No shot
' 1 - Player Arrow
' 2 - Player Double arrow
' 3 - Player Fire
' 4 - Player Boomerang
' 5 - Player Sword
' 6 - Player Double arrow (level 2)
' 7 - Player Fire (level 2)
' 8 - Player Boomerang (level 2)
' 9 - Player Sword (level 2)
dim g_shots(12,4)
const SHOTS_NUM%=bound(g_shots())
' Object data: 0: obj id | 1: x | 2: y | 3: life | 4: GPR 1 | 5: GPR 2 | 6: shadow
dim g_obj(20,6)
' Actions data queue: 0: num of executions | 1: action id | 2: GPR 1 | 3: GPR 2 | 4: GPR 3 | 5: next execution time
dim g_actions_queue(bound(g_obj()),5)
' Blocks: 0: type | 1: hits
dim g_blocks(8,1)
dim g_anim_tick%=0

' Sprites 1 = Player and 2 = Shield
const SHOTS_INI_SPRITE_ID=3                                      ' Initial sprite Id for shots
const OBJ_INI_SPRITE_ID=SHOTS_INI_SPRITE_ID+bound(g_shots()) + 3 ' Initial sprite Id for objects
const BLOCK_INI_SPRITE_ID=OBJ_INI_SPRITE_ID+bound(g_obj()) + 1   ' Initial sprite Id for blocks
