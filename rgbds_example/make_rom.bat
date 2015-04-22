..\mod2gbt\mod2gbt template.mod song

..\rgbasm -oinit.o init.asm
..\rgbasm -ogbt_player.o gbt_player.asm
..\rgbasm -ogbt_player_bank1.o gbt_player_bank1.asm
..\rgbasm -osong.o song.asm

..\rgblink -o gbt_test.gb -p 0xFF -m gbt_test.map -n gbt_test.sym init.o gbt_player.o gbt_player_bank1.o song.o

..\rgbfix -p 0xFF -v gbt_test.gb

del *.o

pause
