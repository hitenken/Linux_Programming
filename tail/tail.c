#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>

#define _GNU_SOURCE
#define DEFAULT_N_LINES 10

static void do_tail(FILE *f, long nlines);

static struct option longopts[] = {
	{"lines", required_argument, NULL, 'n'},
	{"help", no_argument, NULL, 'h'},
	{0,0,0,0}
};

int main(int argc, char *argv[]) {
	int opt;
	long nlines = DEFAULT_N_LINES;
	while ((opt = getopt_long(argc, argv, "n:", longopts, NULL )) != -1) {
		switch (opt){
			case 'n':
				nlines = atol(optarg);
				break;
			case 'h':
				fprintf(stdout, "Usage: %s [-n LINES] [FILE ...]\n",argv[0]);
				exit(0);
			case '?':
				fprintf(stderr, "Usage: %s [-n LINES] [FILE ...]\n",argv[0]);
				exit(1);
		}
	}
	if (optind ==  argc) {
		do_tail(stdin, nlines);
	}else {
		int i;

		for (i = optind; i < argc; i++) {
			FILE *f;

			f = fopen(argv[i], "r");
			if (!f) {
				perror(argv[i]);
				exit(1);
			}
			do_tail(f, nlines);
			fclose(f);
		}
	}
	exit(0);
}


#define MAX_LINE_LENGTH 1024
#define RING_BUFFER_INC(idx, nlines) do {\
	idx++; \
	if (idx >= nlines) { \
		idx = 0; \
	} \
} while(0) \

static void do_tail(FILE *f, long nlines) {
	long curr = 0;
	long n_read_lines = 0;
	char ring_buffer[nlines][MAX_LINE_LENGTH];

	if(nlines <= 0) return;
	while(fgets(ring_buffer[curr], MAX_LINE_LENGTH, f)) {
		RING_BUFFER_INC(curr, nlines);
		n_read_lines++;
	}
	
	if (n_read_lines < nlines) {
		curr = 0;
	}

	int n;
	for (n = nlines; n > 0 && n_read_lines; n--, n_read_lines--) {
		printf("%s", ring_buffer[curr]);
		RING_BUFFER_INC(curr, nlines);
	}
	
}
