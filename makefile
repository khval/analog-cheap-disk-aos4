
incdir = -I./
incdir += -I../

objs += init.o EngineTimer.o loadpng.o comp.o iff.o part_intro.o part_main.o

all: $(objs)
		gcc  $(objs) demo.c  -lpng -lz -lm  -o demo.exe 

init.o:
		gcc  -Wall -O3  -c  init.c  -o init.o

%.o:	%.c
		gcc -c ${incdir} ${libdir} $(opts_inline) $< $(libs) -o $@

clean:
	delete $(objs) demo.exe

