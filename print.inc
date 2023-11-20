sub print_score(score%)
    print_text(SCREEN_ROWS+1,8-(len(str$(score%))),str$(score%))
end sub

sub print_hiscore(hiscore%)
    print_text(SCREEN_ROWS+1,17-(len(str$(hiscore%))),str$(hiscore%))
end sub

sub print_stage(stage%)
    print_text(SCREEN_ROWS+1,30-(len(str$(stage%))),str$(stage%))
end sub

sub print_lives(lives%)
    print_text(SCREEN_ROWS+1,23-(len(str$(lives%))),str$(lives%))
end sub

sub print_text(line%, col%, text$)
    local txt_len%=len(text$), i%, c%, y%=line%*TILE_SIZE

    page write 0
    for i%=1 to txt_len%
        c%=asc(mid$(text$, i%, 1))
        select case c%
            case 48 to 57
                blit NUMBERS_X+(c%-48)*TILE_SIZE,NUMBERS_Y, col%*TILE_SIZE+SCREEN_OFFSET,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
            case 65 to 90
                blit LETTERS_X+(c%-65)*TILE_SIZE,LETTERS_Y, col%*TILE_SIZE+SCREEN_OFFSET,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
            case 97 to 122
                blit LETTERS_X+(c%-97)*TILE_SIZE,LETTERS_Y, col%*TILE_SIZE+SCREEN_OFFSET,y%, TILE_SIZE, TILE_SIZE, OBJ_TILES_BUFFER
        end select
        inc col%
    next
    page write SCREEN_BUFFER
end sub
