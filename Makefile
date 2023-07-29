mymalloc: mymalloc.c
	gcc -g -o  mymalloc mymalloc.c -fsanitize=address -Wall -Werror -Wvla

driver: driver.c mymalloc.c mymalloc.h
	gcc -g -o driver driver.c -fsanitize=address -Wall -Werror -Wvla
	
clean:
	rm -f mymalloc driver