COMPILER = gcc 
SOURCE_LIBS = -Ilib/
OSX_OPT = -Llib/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXinerama -lXi -lXcursor lib/libraylib.a
OSX_OUT = -o "bin/snake"
CFILES = src/*.c

all: 
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)
clean:
	rm ./bin/*
