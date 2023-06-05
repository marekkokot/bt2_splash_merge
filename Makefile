all: bt2_splash_merge

CC 	= g++
CLINK	= -lm -std=c++17 -static
CFLAGS	= -fPIC -Wall -O3 -m64 -std=c++17

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

bt2_splash_merge: bt2_splash_merge.o
	$(CC) -o $@ $^ $(CLINK)
	