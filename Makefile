HEADER_SIZE = 65536

runtime:
	echo -n 0000 > ./runtime

check_runtime:
	@if [ "$(shell stat --printf="%s" ./runtime)" -gt "$(HEADER_SIZE)" ]; \
		then echo; echo "RUNTIME SIZE TOO LARGE"; echo; exit 1; \
	fi

header: check_runtime runtime
	dd if=/dev/zero of=./header bs=1 count=$(HEADER_SIZE)
	dd if=./runtime of=./header conv=notrunc

clean:
	rm -rf ./runtime ./header
