#define _XOPEN_SOURCE 500   /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define RUN_PATH "/run"

char *mkdtemp(char *template); /* Redeclaring to silence warning */

int mount_lupkg(char *srcpath, char *destpath)
{
	/*
	 * The header where the runtime is injected is 64K (i.e. 65536 bytes).
	 * When extracing skip over these bytes to get to the file.
	 */
	int status;
	size_t size = strlen(srcpath) + strlen(destpath) + 100;
	char *cmd = malloc(size);

	if (cmd == NULL) {
		return 1;
	}

	strcpy(cmd, "mount ");
	strcat(cmd, srcpath);
	strcat(cmd, " ");
	strcat(cmd, destpath);
	strcat(cmd, " -t squashfs -o ro,loop,offset=65536");

	/* mount the program */
	status = system(cmd);
	free(cmd);
	return status;
}

int run_lupkg(char *lupkg_dir)
{
	int status;
	char *run_path = RUN_PATH;
	char *cmd = malloc(strlen(lupkg_dir) + strlen(run_path) + 1);

	if (cmd == NULL) {
		return 1;
	}

	strcpy(cmd, lupkg_dir);
	strcat(cmd, run_path);
	status = system(cmd);
	free(cmd);

	return status;
}

int cleanup_lupkg(char *lupkg_dir)
{
	int status;
	char *cmd = malloc(strlen(lupkg_dir) + 15);

	if (cmd == NULL) {
		return 1;
	}

	strcpy(cmd, "umount -f ");
	strcat(cmd, lupkg_dir);
	status = system(cmd);
	free(cmd);
	rmdir(lupkg_dir);

	return status;
}

int main(int argc, char* argv[])
{
	/*
	 * TODO:
	 * - Options to mount lupkg
	 * - Handle cleanup on kill
	 */

	char *cmd;
	char lupkg_dir[] = "/tmp/.lupkg_XXXXXX";
	int status = 0;

	if (mkdtemp(lupkg_dir) == NULL) {
		fprintf(stderr, "Could not make package dir\n");
		exit(1);
	}

	if ((status = mount_lupkg(argv[0], lupkg_dir))) {
		fprintf(stderr, "Could not mount package\n");
		goto cleanup_and_exit;
	}

	if ((status = run_lupkg(lupkg_dir))) {
		fprintf(stderr, "Could not run package\n");
		goto cleanup_and_exit;
	}

cleanup_and_exit:
	cleanup_lupkg(lupkg_dir);
	return status;
}
