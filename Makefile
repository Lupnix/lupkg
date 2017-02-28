HEADER_SIZE = 1000000

all: lupkg

squashfuse:
	# Make sure that you have squashfuse built and in your path
	cp `which $@` ./$@

squashfuse.o: squashfuse
	ld -r -b binary -o squashfuse.o squashfuse

runtime: squashfuse.o
	$(CC) -o runtime squashfuse.o runtime.c

check_runtime: runtime
	@if [ "$(shell stat --printf="%s" ./runtime)" -gt "$(HEADER_SIZE)" ]; \
		then echo; echo "RUNTIME SIZE TOO LARGE"; echo; exit 1; \
	fi

header: check_runtime
	dd if=/dev/zero of=./header bs=1 count=$(HEADER_SIZE)
	dd if=./runtime of=./header conv=notrunc

header.o: header
	ld -r -b binary -o header.o header

app_run.o:
	ld -r -b binary -o app_run.o app_run

clean:
	@rm -rf ./runtime ./header *.o ./lupkg ./build ./app ./squashfuse

lupkg: header.o app_run.o
	$(CC) -o lupkg header.o app_run.o lupkg.c

install:
	cp ./lupkg $(DESTDIR)/usr/bin/lupkg

test: clean lupkg
	./lupkg init
	./lupkg build
	./build/app.lupkg
