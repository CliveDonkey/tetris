$CC = clang
$LINK =-L  -lGL -lm -lX11 -lpthread -lXrandr -lXi -ldl -lglfw
$BUILD =-o3 -g3 -Wall -c

program: program.o glad.o shader.o stb_lib.o glmath.o
	clang -o build/program build/program.o build/shader.o build/stb_lib.o build/glad.o build/glmath.o $($LINK)

program.o: src/program.c src/stb_image.h
	clang $($BUILD) -o build/program.o src/program.c

shader.o: src/shader.c
	clang $($BUILD) -o build/shader.o src/shader.c

stb_lib.o: src/stb_lib.c
	clang $($BUILD) -o build/stb_lib.o src/stb_lib.c
	
glad.o: src/glad/glad.c
	clang $($BUILD) src/glad/glad.c -o build/glad.o

glmath.o: src/glmath/glmath.c
	clang $($BUILD) -o build/glmath.o src/glmath/glmath.c

clean:
	rm build/program.o build/shader.o build/glad.o build/program build/stb_lib.o build/glmath.o
