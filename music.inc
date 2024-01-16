' Music table
dim MUSIC$(9)=("INTRO_MOD", "MAJOU_MOD", "KNIGHTMARE_MOD", "", "", "", "", "", "", "")

sub play_song(song$)
    if not g_sound_on% then exit sub
    on error skip
    play stop
    select case song$
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
        case "BOSS_HIT_SFX"
            play effect "sfx/boss_hit.wav"
        case "BOSS_KILL_SFX"
            play effect "sfx/boss_kill.wav"
        case "BLOCK_HIT_SFX"
            play effect "sfx/block_hit.wav"
        case "BLOCK_OPEN_SFX"
            play effect "sfx/block_open.wav"
        case "BOOMERANG_SFX"
            play effect "sfx/shot_boomerang.wav"
        case "CHRYSTAL_SFX"
            play effect "sfx/chrystal.wav"
        case "START_STAGE_SFX"
            play effect "sfx/start_stage.wav"
        case "ENEMY_KILL_SFX"
            play effect "sfx/enemy_kill.wav"
        case "FIRE_ARROW_SFX"
            play effect "sfx/shot_fire_arrow.wav"
        case "FLAME_SFX"
            play effect "sfx/shot_flame.wav"
        case "FREEZE_TICK_SFX"
            play effect "sfx/freeze_tick.wav"
        case "KILL_ALL_ENEMIES_SFX"
            play effect "sfx/kill_all_enemies.wav"
        case "NEW_LIFE_SFX"
            play effect "sfx/new_life.wav"
        case "PLAYER_DEATH_SFX"
            play effect "sfx/player_death.wav"
        case "POINTS_SFX"
            play effect "sfx/points.wav"
        case "POWER_UP_ENDING"
            play effect "sfx/power_up_ending.wav"
        case "POWER_UP_HIT_SFX"
            play effect "sfx/power_up_hit.wav"
        case "SHIELD_SFX"
            play effect "sfx/shield.wav"
        case "SHOT_SFX"
            play effect "sfx/shot.wav"
        case "SWORD_SFX"
            play effect "sfx/shot_sword.wav"
    end select
end sub
