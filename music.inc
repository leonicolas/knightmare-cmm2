' Music table
dim MUSIC$(9)=("01-gamestart","02-bgm1","04-bgm2","06-prince_of_maresia","08-tavern_funk","10-another_level","12-xakt","14-positive_trek","16-last_hurdle","25-ending")
DIM BOSS_MUSIC$(8)=("","03-boss1","05-boss2","07-bat_rock","09-blind_panic","11-more_boss","13-under_his_eyes","15-boss_fronty_ii","17-manbow_reprise")

sub play_song(song$)
    if not g_sound_on then exit sub
    on error skip
    play stop
    select case song$
        case "STAGE_INTRO"
            play modfile ROOT_DIR + "music/" + MUSIC$(0) + ".mod", 44100
        case "SILENCE"
            play modfile ROOT_DIR + "music/26-silence.mod", 44100
        case "TITLE"
            play modfile ROOT_DIR + "music/24-title.mod", 44100
        case else
            play modfile ROOT_DIR + "music/" + song$ + ".mod", 44100
    end select
end sub

sub play_sfx(sfx$)
    if not g_sound_on or sfx$ = "" then exit sub
    select case sfx$
        case "BOSS_HIT"
            play effect ROOT_DIR + "sfx/boss_hit.wav"
        case "BOSS_KILL"
            play effect ROOT_DIR + "sfx/boss_kill.wav"
        case "BOSS_SHIELD"
            play effect ROOT_DIR + "sfx/boss_shield.wav"
        case "BLOCK_HIT"
            play effect ROOT_DIR + "sfx/block_hit.wav"
        case "BLOCK_OPEN"
            play effect ROOT_DIR + "sfx/block_open.wav"
        case "BOOMERANG"
            play effect ROOT_DIR + "sfx/shot_boomerang.wav"
        case "CHRYSTAL"
            play effect ROOT_DIR + "sfx/chrystal.wav"
        case "ENEMY_KILL"
            play effect ROOT_DIR + "sfx/enemy_kill.wav"
        case "FIRE_ARROW"
            play effect ROOT_DIR + "sfx/shot_fire_arrow.wav"
        case "FLAME"
            play effect ROOT_DIR + "sfx/shot_flame.wav"
        case "FREEZE_TICK"
            play effect ROOT_DIR + "sfx/freeze_tick.wav"
        case "HELMET"
            play effect ROOT_DIR + "sfx/helmet.wav"
        case "KILL_ALL_ENEMIES"
            play effect ROOT_DIR + "sfx/kill_all_enemies.wav"
        case "NEW_LIFE"
            play effect ROOT_DIR + "sfx/new_life.wav"
        case "PLAYER_DEATH"
            play effect ROOT_DIR + "sfx/player_death.wav"
        case "POINTS"
            play effect ROOT_DIR + "sfx/points.wav"
        case "POWER_UP_ENDING"
            play effect ROOT_DIR + "sfx/power_up_ending.wav"
        case "POWER_UP_HIT"
            play effect ROOT_DIR + "sfx/power_up_hit.wav"
        case "RED_DEATH"
            play effect ROOT_DIR + "sfx/red_death.wav"
        case "SHIELD"
            play effect ROOT_DIR + "sfx/shield.wav"
        case "SHOT"
            play effect ROOT_DIR + "sfx/shot.wav"
        case "SPLIT"
            play effect ROOT_DIR + "sfx/split.wav"
        case "SKELETON_RESTORED"
            play effect ROOT_DIR + "sfx/skeleton_restored.wav"
        case "SWORD"
            play effect ROOT_DIR + "sfx/shot_sword.wav"
        case "TERRAIN"
            play effect ROOT_DIR + "sfx/terrain.wav"
    end select
end sub

sub play_stage_song()
    play_song(MUSIC$(g_stage%))
end sub

sub play_boss_song()
    play_song(BOSS_MUSIC$(g_stage%))
end sub
