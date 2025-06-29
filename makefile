# 高速化のためのコンパイラオプション（警告抑制と最適化のバランス）
CFLAGS = -O3 -march=native -ffast-math -funroll-loops -finline-functions -flto 
LDFLAGS = -lncursesw -lm -flto

main : main.o title.o game.o get_key.o player.o boss.o update.o
	gcc $(CFLAGS) -o main main.o title.o object.o display.o matrix.o game.o get_key.o player.o boss.o update.o $(LDFLAGS)
main.o : main.c ./title/title.h
	gcc $(CFLAGS) -c main.c
title.o : ./title/title.c ./title/title.h display.o
	gcc $(CFLAGS) -c ./title/title.c
game.o : ./game/game.c ./game/game.h object.o display.o matrix.o
	gcc $(CFLAGS) -c ./game/game.c
get_key.o : ./game/get_key.c ./game/game.h
	gcc $(CFLAGS) -c ./game/get_key.c
player.o : ./game/player.c ./game/game.h
	gcc $(CFLAGS) -c ./game/player.c
boss.o : ./game/boss.c ./game/game.h
	gcc $(CFLAGS) -c ./game/boss.c
update.o : ./game/update.c ./game/game.h
	gcc $(CFLAGS) -c ./game/update.c
object.o : ./object/object.c ./object/object.h matrix.o
	gcc $(CFLAGS) -c ./object/object.c
display.o : ./display/display.c ./display/display.h
	gcc $(CFLAGS) -c ./display/display.c
matrix.o : ./matrix/matrix.c ./matrix/matrix.h
	gcc -c ./matrix/matrix.c

clean :
	rm -f ./*.o
touch :