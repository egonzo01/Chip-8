chip:
	gcc -o chip8 main.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2

test: 
	gcc -o test test.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2

clean:
	rm -rf chip8
