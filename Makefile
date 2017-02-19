HEADER_SIZE = 65536

all: lupkg

check_runtime: runtime
	@if [ "$(shell stat --printf="%s" ./runtime)" -gt "$(HEADER_SIZE)" ]; \
		then echo; echo "RUNTIME SIZE TOO LARGE"; echo; exit 1; \
	fi

header: check_runtime
	dd if=/dev/zero of=./header bs=1 count=$(HEADER_SIZE)
	dd if=./runtime of=./header conv=notrunc

header.o: header
	ld -r -b binary -o header.o header

run.o:
	ld -r -b binary -o run.o run

clean:
	@rm -rf ./runtime ./header *.o ./lupkg ./build ./app

lupkg: header.o run.o
	$(CC) -o lupkg header.o run.o lupkg.c

install:
	cp ./lupkg $(DESTDIR)/usr/bin/lupkg

test: clean lupkg
	./lupkg init
	./lupkg build
	sudo ./build/app.lupkg
