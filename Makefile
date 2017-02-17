HEADER_SIZE = 65536

check_runtime: runtime
	@if [ "$(shell stat --printf="%s" ./runtime)" -gt "$(HEADER_SIZE)" ]; \
		then echo; echo "RUNTIME SIZE TOO LARGE"; echo; exit 1; \
	fi

header: check_runtime
	dd if=/dev/zero of=./header bs=1 count=$(HEADER_SIZE)
	dd if=./runtime of=./header conv=notrunc

test.tar.gz:
	tar -czf test.tar.gz ./app

test.lupkg: header test.tar.gz
	cat ./header > test.lupkg
	cat ./test.tar.gz >> test.lupkg
	chmod +x ./test.lupkg

clean:
	rm -rf ./runtime ./header ./test.tar.gz ./test.lupkg

test: test.lupkg
	./test.lupkg
