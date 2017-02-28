#define _XOPEN_SOURCE 500   /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

char *mkdtemp(char *template); /* Redeclaring to silence warning */

/* TMPHACK */
extern const unsigned char _binary_squashfuse_start[];
extern const unsigned char _binary_squashfuse_end[];

int mount_lupkg(char *srcpath, char *destpath)
{
	/*
	 * The header where the runtime is injected is 64K (i.e. 1000000 bytes).
	 * When mounting skip over these bytes to get to the file.
	 *
	 * TODO: use fuse to mount/umount
	 */

	int status;
	size_t size = strlen(srcpath) + strlen(destpath) + 100;
	char *cmd = malloc(size);
	char squashfuse_tmp[] = "/tmp/squashfuse.XXXXXX";
	size_t len = _binary_squashfuse_end - _binary_squashfuse_start;
	FILE *squashfuse;

	if (cmd == NULL) {
		return 1;
	}


	/* TMPHACK */
	mktemp(squashfuse_tmp);
	squashfuse = fopen(squashfuse_tmp, "w");
	for (int i = 0; i < len; i++) {
		fputc(_binary_squashfuse_start[i], squashfuse);
	}
	fclose(squashfuse);
	chmod(squashfuse_tmp, 0777);

	sprintf(cmd, "%s -o offset=1000000 %s %s",
		squashfuse_tmp, srcpath, destpath);

	status = system(cmd);
	free(cmd);

	unlink(squashfuse_tmp);
	return status;
}

int run_lupkg(char *lupkg_dir, char *argv[])
{
	int status;
	char cmd[25] = {0};

	sprintf(cmd, "%s/app_run", lupkg_dir);

	return execvp(cmd, argv);
}

int cleanup_lupkg(char *lupkg_dir)
{
	int status;
	char *cmd = malloc(strlen(lupkg_dir) + 15);

	if (cmd == NULL) {
		return 1;
	}

	sprintf(cmd, "fusermount -u %s", lupkg_dir);
	status = system(cmd);
	free(cmd);

	rmdir(lupkg_dir);

	return status;
}

int main(int argc, char* argv[])
{
	char *cmd;
	char lupkg_dir[] = "/tmp/.lupkg_XXXXXX";
	char srcpath[1024];
	int status = 0;
	pid_t pid;

	readlink("/proc/self/exe", srcpath, 1024);

	if (mkdtemp(lupkg_dir) == NULL) {
		fprintf(stderr, "Could not make package dir\n");
		return 1;
	}

	if ((status = mount_lupkg(srcpath, lupkg_dir))) {
		fprintf(stderr, "Could not mount package\n");
		cleanup_lupkg(lupkg_dir);
		return status;
	}

	pid = fork();

	if(pid == 0) {
		if ((status = run_lupkg(lupkg_dir, argv))) {
			fprintf(stderr, "Could not run package\n");
		}

		return status;
	}

	waitpid(pid, &status, 0);
	cleanup_lupkg(lupkg_dir);
	return status;
}
