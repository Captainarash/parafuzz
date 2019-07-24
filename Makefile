CC=clang
CFLAGS=-O0 -framework IOKit -I .

all: test.c mach_test.c fuzz_interface.c
	$(CC) -o mach_test mach_test.c fuzz_interface.c $(CFLAGS)
	$(CC) -o fuzz_test test.c fuzz_interface.c $(CFLAGS)

mach_test: mach_test.c fuzz_interface.c
	$(CC) -o mach_test mach_test.c fuzz_interface.c $(CFLAGS)

fuzz_test: test.c fuzz_interface.c
	$(CC) -o fuzz_test test.c fuzz_interface.c $(CFLAGS)

fuzz_test_parallel: test_parallel.c fuzz_interface.c
	$(CC) -o fuzz_test_parallel test_parallel.c fuzz_interface.c $(CFLAGS)

race_mach: race_mach.c fuzz_interface.c
	$(CC) -o race_mach race_mach.c fuzz_interface.c $(CFLAGS)

AGDPClientControl:
	$(CC) -o AGDPClientControl_Crash AGDPClientControl_Crash.c fuzz_interface.c -framework IOKit -I .

clean:
	rm fuzz_test
	rm mach_test
