# CC?=gcc

# all: test1.c test2.c test3.c
# 	$(CC) -o test1 test1.c
# 	$(CC) -o test2 test2.c
# 	$(CC) -o test3 test3.c
# 	./test3

all: clean compile

NUM_TEST = 9

compile:
	for i in $(shell seq 5 $(NUM_TEST)) ; do \
		$(CC) -o "test$$i" "test$$i.c"; \
	done

clean:
	for i in $(shell seq 5 $(NUM_TEST)) ; do \
		rm -rf "test$$i"; \
	done