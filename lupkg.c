#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define die(msg) fprintf(stderr, "%s\n", msg); exit(1)

extern const unsigned char _binary_header_start[];
extern const unsigned char _binary_header_end[];

extern const unsigned char _binary_app_run_start[];
extern const unsigned char _binary_app_run_end[];

int lupkg_build(int argc, char *argv[])
{
	FILE *pkg, *app_fs;
	DIR *build = opendir("./build/");
	size_t header_len = _binary_header_end - _binary_header_start;
	int c;

	if (build) {
		closedir(build);
		unlink("./build/app.fs");
	} else if (ENOENT == errno) {
		if(mkdir("./build/", S_IRWXU | S_IRWXG | S_IRWXO)) {
			die("Could not create build directory!");
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

	return 0;
}

int lupkg_init(int argc, char *argv[])
{
	DIR *app = opendir("./app/");
	size_t app_run_len = _binary_app_run_end - _binary_app_run_start;
	FILE *app_run;

	if (app) {
		closedir(app);
		die("The app directory already exists!");
	} else if (ENOENT == errno) {
		if(mkdir("./app/", S_IRWXU | S_IRWXG | S_IRWXO)) {
			die("Could not create app directory!");
		}
	}

	app_run = fopen("./app/app_run", "w");

	if (app_run == NULL) {
		die("Could not open app_run file!");
	}

	for (int i = 0; i < app_run_len; i++) {
		fputc(_binary_app_run_start[i], app_run);
	}

	fclose(app_run);

	if (chmod("./app/app_run", 0777)) {
		die("Could not make app_run executable!");
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int status = 0;

	if (argc < 2) {
		die("Usage: lupkg [build|init]");
	}

	if (!strcmp(argv[1], "build")) {
		status = lupkg_build(argc, argv);
	} else if (!strcmp(argv[1], "init")) {
		status = lupkg_init(argc, argv);
	} else {
		die("Usage: lupkg [build|init]");
	}

	printf("%s complete!\n", argv[1]);
	return status;
}
