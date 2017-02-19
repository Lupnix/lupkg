HEADER_SIZE = 65536

check_runtime: runtime
	@if [ "$(shell stat --printf="%s" ./runtime)" -gt "$(HEADER_SIZE)" ]; \
		then echo; echo "RUNTIME SIZE TOO LARGE"; echo; exit 1; \
	fi

header: check_runtime
	dd if=/dev/zero of=./header bs=1 count=$(HEADER_SIZE)
	dd if=./runtime of=./header conv=notrunc

app.sfs:
	mksquashfs ./app ./app.sfs

app.lupkg: header app.sfs
	cat ./header > app.lupkg
	cat ./app.sfs >> app.lupkg
	chmod +x ./app.lupkg

clean:
	rm -rf ./runtime ./header ./app.sfs ./app.lupkg

test: app.lupkg
	sudo ./app.lupkg
