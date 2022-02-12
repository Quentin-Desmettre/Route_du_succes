
INCLUDE = -I ./include/

FLAGS = -Wall -Wextra

all:
	g++ $(FLAGS) -o jam $(INCLUDE) src/*.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -g

run:
	$(MAKE) all
	./jam
