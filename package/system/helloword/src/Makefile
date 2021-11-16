C_FILE = helloword.c \
			mytool1.c \
			wifi/wifi_info.c

all: helloword

helloword:
	$(CC) $(CFLAGS) -Wall $(C_FILE) -o $@ $^
#	$(CC) $(CFLAGS) -Wall helloword.c mytool1.c -o $@ $^

clean:
	rm -f helloword