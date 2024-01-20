gcc -shared -o game.so -fPIC -O3 game_thr.c tt.c
gcc -o TEMP -fPIC -O3 game_thr.c tt.c