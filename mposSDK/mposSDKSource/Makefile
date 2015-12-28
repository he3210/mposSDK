CC=gcc
all:  cJSON.o  innerInterface.o ipaynowMPOSSDK.o iso8583.o log.o
	$(CC) -fpic -shared -o ./bin/libipaynowSDK.so cJSON.o  innerInterface.o ipaynowMPOSSDK.o iso8583.o log.o


cJSON.o : cJSON.c cJSON.h
	$(CC) -fPIC -std=c99 -c cJSON.c -lm
innerInterface.o : innerInterface.c innerInterface.h
	$(CC) -fPIC -std=c99 -c innerInterface.c
ipaynowMPOSSDK.o : ipaynowMPOSSDK.c ipaynowMPOSSDK.h
	$(CC) -fPIC -std=c99 -c ipaynowSDK.c
iso8583.o : iso8583.c iso8583.h
	$(CC) -fPIC -std=c99 -c iso8583.c
log.o : log.c log.h
	$(CC) -fPIC -std=c99 -c log.c

static : cJSON.o  innerInterface.o ipaynowMPOSSDK.o iso8583.o log.o
	ar rcs ./bin/libipaynowSDK.a cJSON.o  innerInterface.o ipaynowSDK.o iso8583.o log.o

clean:
	rm *.o
