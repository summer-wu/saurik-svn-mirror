CC := gcc

all: pledit

%: %.m
	$(CC) -std=c99 -Wall -Werror -o $@ $< -framework CoreFoundation -framework Foundation -lobjc

clean:
	rm -f pledit

.PHONY: all clean
