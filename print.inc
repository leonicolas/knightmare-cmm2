sub print_score(score%)
    print_text(SCREEN_ROWS+1, 8-(len(str$(score%))), str$(score%), SCREEN_OFFSET)
end sub

sub print_hiscore(hiscore%)
    print_text(SCREEN_ROWS+1, 17-(len(str$(hiscore%))), str$(hiscore%), SCREEN_OFFSET)
end sub

sub print_stage(stage%)
    print_text(SCREEN_ROWS+1, 30-(len(str$(stage%))), str$(stage%), SCREEN_OFFSET)
end sub

sub print_lives(lives%)
    print_text(SCREEN_ROWS+1, 23-(len(str$(lives%))), str$(lives%), SCREEN_OFFSET)
end sub

sub print_freeze_timer(t)
    local lsh%=fix(t)
    local rsh%=fix((t - lsh%) * 60)
    local l$=right$("0"+str$(lsh%),2)
    local r$=right$("0"+str$(rsh%),2)
    print_text(1, 24, l$+":"+r$, SCREEN_OFFSET, 1)
end sub

sub clear_freeze_timer()
    clear_text(1, 24, 5, SCREEN_OFFSET, 1)
end sub

sub print_power_up_timer(t)
    local txt$=right$("0"+str$(fix(t)),2)
    print_text(1, 4, txt$, SCREEN_OFFSET, 1)
end sub

sub clear_power_up_timer()
    clear_text(1, 4, 2, SCREEN_OFFSET, 1)
end sub

sub clear_text(line%, col%, len%, offset%, buffer%)
    print_text(line%, col%, string$(len%,127), offset%, buffer%)
end sub

sub print_text(line%, col%, text$, offset%, buffer%)
    local txt_len%=len(text$), i%, c%, y%=line%*TILE_SIZE

    page write buffer%
    for i%=1 to txt_len%
        c%=asc(mid$(text$, i%, 1))
        select case c%
            case 32
                blit SYMBOLS_X,SYMBOLS_Y, col%*TILE_SIZE+offset%,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
            case 48 to 58
                blit NUMBERS_X+(c%-48)*TILE_SIZE,NUMBERS_Y, col%*TILE_SIZE+offset%,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
            case 65 to 90
                blit LETTERS_X+(c%-65)*TILE_SIZE,LETTERS_Y, col%*TILE_SIZE+offset%,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
            case 97 to 122
                blit LETTERS_X+(c%-97)*TILE_SIZE,LETTERS_Y, col%*TILE_SIZE+offset%,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
            case 127
                blit TRANSPARENT_BLOCK_X,TRANSPARENT_BLOCK_Y, col%*TILE_SIZE+offset%,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
        end select
        inc col%
    next
    page write SCREEN_BUFFER
end sub
