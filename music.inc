' Music table
dim MUSIC$(8)=("INTRO","MAJOU","KNIGHTMARE","MAJOU","MAJOU","KNIGHTMARE","MAJOU","MAJOU","KNIGHTMARE")

sub play_song(song$)
    if not g_sound_on then exit sub
    on error skip
    play stop
    select case song$
        case "STAGE_INTRO"
            play modfile ROOT_DIR + "music/stage-intro.mod", 44100
        case "BOSS"
            play modfile ROOT_DIR + "music/boss.mod", 44100
        case "DEMON"
            play modfile ROOT_DIR + "music/facing-the-demon.mod", 44100
        case "ENDING"
            play modfile ROOT_DIR + "music/ending.mod", 44100
        case "MAJOU"
            play modfile ROOT_DIR + "music/majou-densetsu.mod", 44100
        case "KNIGHTMARE"
            play modfile ROOT_DIR + "music/knightmare.mod", 44100
        case "SILENCE"
            play modfile ROOT_DIR + "music/silence.mod", 44100
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
    if g_stage% < 9 then
        play_song(MUSIC$(g_stage%))
    end if
end sub

sub play_boss_song()
    select case g_stage%
        case 1,3,4,5,7,8
            play_song("DEMON")
        case 2,6
            play_song("BOSS")
    end select
end sub
