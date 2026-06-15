FLAGS = -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o
PROJECT = Nonogram

.PHONY: all
all:
	gcc $(PROJECT).c $(FLAGS) $(PROJECT).out

debug:
	gcc $(PROJECT).c $(FLAGS) $(PROJECT).out -g

clean:
	rm -f $(PROJECT).out
