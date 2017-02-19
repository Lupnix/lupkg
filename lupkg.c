#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>


#define die(msg) fprintf(stderr, "%s\n", msg); exit(1)

extern const unsigned char _binary_header_start[];
extern const unsigned char _binary_header_end[];

void lupkg_build(int argc, char *argv[])
{
	FILE *pkg, *app_fs;
	DIR *build = opendir("./build/");
	size_t header_len = _binary_header_end - _binary_header_start;
	int c;

	if (build) {
		closedir(build);
	} else if (ENOENT == errno) {
		if(mkdir("./build/", S_IRWXU | S_IRWXG | S_IRWXO)) {
			die("Could not create build dir!");
		}
	}

	if (system("mksquashfs ./app/ ./build/app.fs > /dev/null")) {
		die("Could not create app file system!");
	}


	pkg = fopen("./build/app.lupkg", "w");

	if (pkg == NULL) {
		die("Could not open package!");
	}

	for (int i = 0; i < header_len; i++) {
		fputc(_binary_header_start[i], pkg);
	}

	app_fs = fopen("./build/app.fs", "r");

	if (app_fs == NULL) {
		fclose(pkg);
		die("Could not open app file system!");

	}

	while ((c = fgetc(app_fs)) != EOF) {
		fputc(c, pkg);
	}

	fclose(pkg);
	fclose(app_fs);

	if (chmod("./build/app.lupkg", 0777)) {
		die("Could not make package executable!");
	}
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		die("Usage: lupkg [build]");
	}

	if (!strcmp(argv[1], "build")) {
		lupkg_build(argc, argv);
	} else {
		die("Usage: lupkg [build]");
	}

	return 0;
}
