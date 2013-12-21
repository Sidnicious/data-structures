EVERYTHING = $(patsubst src/%.cpp,bin/%,$(wildcard src/*.cpp))
EVERYTHING += $(patsubst src/%.c,bin/%_c,$(wildcard src/*.c))

CXX=clang++
CXXFLAGS +=\
	-std=c++11 -stdlib=libc++\
	-Wall\
	-Wpedantic\
	-Werror

CC=clang
CCFLAGS +=\
	-std=c99\
	-Wall\
	-Wpedantic\
	-Werror

ifeq ($(DEBUG),1)
	CXXFLAGS += -g -DDEBUG 
	CCFLAGS += -g -DDEBUG
else
	CXXFLAGS += -Ofast
	CCFLAGS += -Ofast
endif

everything: $(EVERYTHING)

bin/%_c: src/%.c src/vector.h
	$(CC) $(CCFLAGS) -o $@ $<

bin/%: src/%.cpp src/*.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<
