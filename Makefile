EVERYTHING = $(patsubst %.cpp,%,$(wildcard *.cpp))

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

%_c: %.c vector.h
	$(CC) $(CCFLAGS) -o $@ $<

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
