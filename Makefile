CC=gcc
CFLAGS=-O0 -framework QuartzCore -framework IOKit -framework ApplicationServices -I .
LDFLAGS=-L/usr/local/opt/openssl/lib
CPPFLAGS=-I/usr/local/opt/openssl/include

all: test.c mach_test.c fuzz_interface.c
	$(CC) -o mach_test mach_test.c fuzz_interface.c $(CFLAGS) -lcrypto -lssl $(LDFLAGS) $(CPPFLAGS)
	$(CC) -o fuzz_test test.c fuzz_interface.c $(CFLAGS) -lcrypto -lssl $(LDFLAGS) $(CPPFLAGS)

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
