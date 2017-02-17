#define _XOPEN_SOURCE 500   /* See feature_test_macros(7) */
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define RUN_PATH "/app/run"

char *mkdtemp(char *template); /* Redeclaring to silence warning */

int rm_r_callback (const char *fpath, const struct stat *sb, int tflag,
		   struct FTW *ftwbuf)
{
	int status = 0;

	switch(tflag) {
	case FTW_F:
	case FTW_SL:
		status = unlink(fpath);
		break;
	case FTW_DP:
		status = rmdir(fpath);
		break;
	default:
		status = 1;
	}

	if (status) {
		printf("Could not delete %s\n", fpath);
	}

	return status;
}

int rm_r(const char *dirpath)
{

	int flags =  FTW_DEPTH | FTW_MOUNT;
	return nftw(dirpath, rm_r_callback, 20, flags);
}

int extract_lupkg(char *srcpath, char *destpath)
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

	strcpy(cmd, "dd if=");
	strcat(cmd, srcpath);
	strcat(cmd, " skip=65536 bs=1 2> /dev/null | tar -xzf - -C ");
	strcat(cmd, destpath);

	/* extract the program */
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

int main(int argc, char* argv[])
{
	/*
	 * TODO:
	 * - Options to extract lupkg
	 * - Handle cleanup on kill
	 */

	char *cmd;
	char lupkg_dir[] = "/tmp/.lupkg_XXXXXX";
	int status = 0;

	if (mkdtemp(lupkg_dir) == NULL) {
		fprintf(stderr, "Could not make package dir\n");
		exit(1);
	}

	if ((status = extract_lupkg(argv[0], lupkg_dir))) {
		fprintf(stderr, "Could not extract package\n");
		goto cleanup_and_exit;
	}

	if ((status = run_lupkg(lupkg_dir))) {
		fprintf(stderr, "Could not run package\n");
		goto cleanup_and_exit;
	}

cleanup_and_exit:
	rm_r(lupkg_dir);
	return status;
}
