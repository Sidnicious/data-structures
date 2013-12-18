EVERYTHING = $(patsubst src/%.cpp,bin/%,$(wildcard src/*.cpp))
EVERYTHING += $(patsubst src/%.c,bin/%_c,$(wildcard src/*.c))

CXX=clang++
CXXFLAGS +=\
	-std=c++11 -stdlib=libc++\
	-DDEBUG\
	-g\
	-Wall\
	-Wpedantic\
	-Werror

CC=clang
CCFLAGS +=\
	-std=c99\
	-DDEBUG\
	-g\
	-Wall\
	-Wpedantic\
	-Werror

everything: $(EVERYTHING)

bin/%_c: src/%.c src/vector.h
	$(CC) $(CCFLAGS) -o $@ $<

bin/%: src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
