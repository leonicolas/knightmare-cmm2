' Music table
dim MUSIC$(9)=("INTRO_MOD","MAJOU_MOD","KNIGHTMARE_MOD","MAJOU_MOD","MAJOU_MOD","KNIGHTMARE_MOD","MAJOU_MOD","","","")

sub play_song(song$)
    if not g_sound_on% then exit sub
    on error skip
    play stop
    select case song$
        case "BOSS_MOD"
            play modfile "music/boss.mod", 16000
        case "DEMON_MOD"
            play modfile "music/facing-the-demon.mod", 16000
        case "MAJOU_MOD"
            play modfile "music/majou-densetsu.mod", 16000
        case "KNIGHTMARE_MOD"
            play modfile "music/knightmare.mod", 16000
        case "SILENCE_MOD"
            play modfile "music/silence.mod", 16000
    end select
end sub

sub play_sfx(sfx$)
    if not g_sound_on% or sfx$ = "" then exit sub
    select case sfx$
        case "BOSS_HIT"
            play effect "sfx/boss_hit.wav"
        case "BOSS_KILL"
            play effect "sfx/boss_kill.wav"
        case "BOSS_SHIELD"
            play effect "sfx/boss_shield.wav"
        case "BLOCK_HIT"
            play effect "sfx/block_hit.wav"
        case "BLOCK_OPEN"
            play effect "sfx/block_open.wav"
        case "BOOMERANG"
            play effect "sfx/shot_boomerang.wav"
        case "CHRYSTAL"
            play effect "sfx/chrystal.wav"
        case "START_STAGE"
            play effect "sfx/start_stage.wav"
        case "ENEMY_KILL"
            play effect "sfx/enemy_kill.wav"
        case "FIRE_ARROW"
            play effect "sfx/shot_fire_arrow.wav"
        case "FLAME"
            play effect "sfx/shot_flame.wav"
        case "FREEZE_TICK"
            play effect "sfx/freeze_tick.wav"
        case "HELMET"
            play effect "sfx/helmet.wav"
        case "KILL_ALL_ENEMIES"
            play effect "sfx/kill_all_enemies.wav"
        case "NEW_LIFE"
            play effect "sfx/new_life.wav"
        case "PLAYER_DEATH"
            play effect "sfx/player_death.wav"
        case "POINTS"
            play effect "sfx/points.wav"
        case "POWER_UP_ENDING"
            play effect "sfx/power_up_ending.wav"
        case "POWER_UP_HIT"
            play effect "sfx/power_up_hit.wav"
        case "SHIELD"
            play effect "sfx/shield.wav"
        case "SHOT"
            play effect "sfx/shot.wav"
        case "SPLIT"
            play effect "sfx/split.wav"
        case "SKELETON_RESTORED"
            play effect "sfx/skeleton_restored.wav"
        case "SWORD"
            play effect "sfx/shot_sword.wav"
    end select
end sub
