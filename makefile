OPTIONS = -framework GLUT -framework OpenGL
DEFINES = -D GL_SILENCE_DEPRECATION

project1: project1.c myLib.o initShader.o stl_loader.o
	gcc -o project1 project1.c myLib.o initShader.o stl_loader.o $(OPTIONS) $(DEFINES)

project1_stl: project1_stl.c myLib.o initShader.o stl_loader.o
	gcc -o project1_stl project1_stl.c myLib.o initShader.o stl_loader.o $(OPTIONS) $(DEFINES)

myLib.o: myLib.c myLib.h
	gcc -c myLib.c $(DEFINES)

initShader.o: initShader.c initShader.h
	gcc -c initShader.c $(DEFINES)

stl_loader.o: stl_loader.c stl_loader.h myLib.h
	gcc -c stl_loader.c $(DEFINES)

clean:
	rm *.o
	rm project1
	rm project1_stl
