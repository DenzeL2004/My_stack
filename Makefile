all: mkdirectory build 

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

build:  main.o stack.o log_info.o generals.o
	g++ obj/main.o obj/stack.o obj/log_info.o obj/generals.o -o stack

main.o: main.cpp
	g++ main.cpp -c -o obj/main.o $(FLAGS)

stack.o: stack.cpp
	g++ stack.cpp -c -o obj/stack.o $(FLAGS)

log_info.o: log_info.cpp
	g++ log_info.cpp -c -o obj/log_info.o $(FLAGS)

generals.o: Generals_func/generals.cpp
	g++ Generals_func/generals.cpp -c -o obj/generals.o $(FLAGS)

.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj

cleanup:
	rm *.o stack