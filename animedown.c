#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "./config.h"

#ifdef DEBUG
static void
print_cmd(char **argv)
{
	fprintf(stderr, ">>>");
	while (*argv != NULL) {
		fprintf(stderr, " '%s'", *argv);
		argv++;
	}
	fprintf(stderr, "\n");
}
#endif

static void
cmd(char **argv)
{
	pid_t child = fork();

	if (child == 0) {
#ifdef DEBUG
		print_cmd(argv);
#endif
		if (execvp(argv[0], argv) < 0) {
			perror("Error: Cannot execute child proccess");
			exit(EXIT_FAILURE);
		}
	} else if (child > 0) {
		int wstatus;
		if (wait(&wstatus) < 0) {
			perror("Error: Could not wait for child");
			exit(EXIT_FAILURE);
		}
	} else {
		perror("Error: Could not fork the proccess");
		exit(EXIT_FAILURE);
	}
}

static void
cmdfd(char **argv, int fdin, int fdout)
{
	pid_t child = fork();

	if (child == 0) {
		if (fdin != STDIN_FILENO) {
			close(STDIN_FILENO);
			dup2(fdin, STDIN_FILENO);
		}
		if (fdout != STDOUT_FILENO) {
			close(STDOUT_FILENO);
			dup2(fdout, STDOUT_FILENO);
		}
#ifdef DEBUG
		print_cmd(argv);
#endif
		if (execvp(argv[0], argv) < 0) {
			perror("Error: Cannot execute child proccess");
			exit(EXIT_FAILURE);
		}
	} else if (child > 0) {
		int wstatus;
		if (wait(&wstatus) < 0) {
			perror("Error: Could not wait for child");
			exit(EXIT_FAILURE);
		}
	} else {
		perror("Error: Could not fork the proccess");
		exit(EXIT_FAILURE);
	}

}

static bool
cmd_pipeline(char ***pipeline)
{
	int p[2];
	int saved_in = STDIN_FILENO;
	int saved_out = STDOUT_FILENO;
	int prev_in = saved_in;

	for (size_t i = 0; pipeline[i] != NULL; i += 1) {
		if (pipe(p) == -1) {
			perror("Error: could not create pipe");
			return false;
		}

		if (pipeline[i + 1] != NULL) {
			cmdfd(pipeline[i], prev_in, p[1]);
		} else {
			cmdfd(pipeline[i], prev_in, saved_out);
			close(p[0]);
		}

		close(p[1]);
		prev_in = p[0];
	}

	return true;
}

static char *
get_cache_dir()
{
	if (getenv("XDG_CACHE_HOME")) {
		return getenv("XDG_CACHE_HOME");
	} else {
		const char *home = getenv("HOME");
		char *cache = calloc(sizeof(char), 127);

		if (strlen(home) > 120) {
			fprintf(stderr, "Could not get location of cache directory: buffer too small.\n");
			exit(EXIT_FAILURE);
		}

		strcat(cache, home);

		return strcat(cache, "/.cache");
	}
}

static bool
pmkdir(const char *const pathname)
{
	if (mkdir(pathname, 0755) < 0 && errno != EEXIST) {
		return false;
	}
	return true;
}

int
main(void)
{
	char *cache_dir = strcat(get_cache_dir(), "/animedown");
	/* char template[] = "/tmp/animedown.XXXXXX"; */
	/* char *temp_dir = mkdtemp(template); */

	/*
	if (temp_dir == NULL) {
		perror("Error: Could not create temporary directory");
		return EXIT_FAILURE;
	}
	*/

	if (!pmkdir(cache_dir)) {
		perror("Error: Could not create cache directory");
		return EXIT_FAILURE;
	}

	/*
	if (chdir(temp_dir) == -1) {
		perror("Error: Could not open temporary directory");
		return EXIT_FAILURE;
	}
	*/

	if (chdir(cache_dir) == -1) {
		perror("Error: Could not open temporary directory");
		return EXIT_FAILURE;
	}

	int saved_stdin = dup(STDIN_FILENO); (void)saved_stdin;
	int saved_stdout = dup(STDOUT_FILENO);

	char *cmd_curl[]  = {"curl", "-sfL", "-m", "15", "https://subsplease.org/rss/?t&r=720", NULL};
	char *cmd_sfeed[] = {"sfeed", NULL};
	char *cmd_grep[]  = {"grep", "-iv", "(batch)", NULL};

	char *cmd_awk[]   = {"awk", "-F\t", "{printf \"output=\\\"%s.torrent\\\"\\nurl=%s\\n\", $2, $3}", NULL};
	char *cmd_curl2[] = {"curl", "-Z", "-K", "-", NULL};

	char **pipeline1[] = {cmd_curl, cmd_sfeed, cmd_grep, NULL};
	char **pipeline2[] = {cmd_awk, cmd_curl2, NULL};

	int p[2];
	char *feed = "/tmp/feed.tsv";
	int fdfeed = open(feed, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if (pipe(p) == -1) {
		perror("Could not create pipe in main");
		exit(EXIT_FAILURE);
	}
	
	dup2(fdfeed, STDOUT_FILENO);
	if (!cmd_pipeline(pipeline1)) perror("Error: pipeline1 failed");

	dup2(fdfeed, STDIN_FILENO);
	dup2(p[1], STDOUT_FILENO);
	for (size_t i = 0; filter[i] != NULL; i += 1) {
		lseek(fdfeed, 0, SEEK_SET);
		cmd((char *[]){"grep", "-F", filter[i], NULL});
	}
	close(fdfeed);
	remove(feed);
	close(p[1]);

	dup2(p[0], STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	if (!cmd_pipeline(pipeline2)) perror("Error: pipeline2 failed");
	close(p[0]);

	/* cmd((char *[]){"rsync", "-rcu", ".", cache_dir,  NULL}); */

	cmd((char *[]){"fdfind", ".", cache_dir, "-tf", "-etorrent", "--changed-within", "15min", "-X", "cp", "-t", "/var/lib/transmission-daemon/watch/", "--", NULL});
	cmd((char *[]){"fdfind", ".", cache_dir, "-tf", "-etorrent", "--change-older-than", "2h", "-X", "rm", "--", NULL});

	/* cmd((char *[]){"rm", "-rf", temp_dir, NULL}); */
	free(cache_dir);
	return EXIT_SUCCESS;
}

