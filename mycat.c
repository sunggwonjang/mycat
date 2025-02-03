#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_INFO		60
#define ANSI_YELLOW		"\033[1;33m"
#define ANSI_GREEN		"\033[0;32m"
#define ANSI_DEFAULT	"\033[0m"

#define TRUE			1
#define FALSE			0


int nextCharOffset (unsigned char ch)
{
	// for performance, just check whether it is ASCII(7bit) value or not.
	// if it is, return the length as 1.
	if (!(ch & 0x80)) {
		return 1;
	}

	unsigned char mask = 0xFE;
	int count = 7;
	do {
		ch = (ch & mask);
		mask = (mask << 1);
		count --;
	} while (ch != mask);

	return count;
}

void tabtospace (const char* src, char* dest, int nTab)
{
	int bytePerChar;
	int column = 0, extra, i;

	while (*src != '\0')
	{
		bytePerChar = nextCharOffset (*src);
		if (bytePerChar == 1)
		{
			if (*src == '\t')
			{
				extra = nTab - (column % nTab);
				if (extra == 0)
					extra = nTab;
				for (i = 0; i < extra; i++)
				{
					*dest++ = ' ';
					column++;
				}
			}
			else
			{
				*dest++ = *src;
				column++;
			}
		}
		else
		{
			memcpy (dest, src, bytePerChar);
			dest += bytePerChar;
			column += 2;
		}
		src += bytePerChar;
	}
	*dest = '\0';
}

int filecat (const char* filename, int isLine, int isHead, int isTab, int nTab)
{
	char buffer[4096];
	char rebuff[4096*8];
	char *pRead;
	int nLine = 1, i;
	FILE *fp;

	fp = fopen (filename, "rt");
	if (fp == NULL)
	{
		printf ("%s - No such file or directory\n", filename);
		return -1;
	}

	if (isHead != FALSE)
		printf (ANSI_GREEN "FILENAME : %s\n" ANSI_DEFAULT, filename);

	do
	{
		pRead = fgets (buffer, sizeof(buffer) - 1, fp);
		if (pRead != NULL)
		{
			if (isLine != FALSE)
				printf (ANSI_YELLOW "%6d" ANSI_DEFAULT ": ", nLine);

			if (isTab == FALSE)
			{
				printf ("%s", buffer);
			}
			else
			{
				tabtospace (buffer, rebuff, nTab);
				printf ("%s", rebuff);
			}

			nLine++;
		}
	} while (pRead != NULL);

	fclose (fp);
	return 0;
}

void usage (const char* cmd)
{
	printf ("Usage) %s [-t count] [-l] [-h] filename\n", cmd);
	printf ("  -h      : show filename\n");
	printf ("  -l      : show line number\n");
	printf ("  -t size : change tab size (1-8)\n");
	exit (EXIT_FAILURE);
}

int main (int argc, char* argv[])
{
	int isLine = FALSE, isTab = FALSE, isHead = FALSE;
	int nTab = 0, opt, i;

	const char* env = getenv ("TAB");
	if (env != NULL)
	{
		i = atoi (env);
		if (i > 0)
		{
			nTab = i;
			isTab = TRUE;
		}
	}

	while ((opt = getopt (argc, argv, "hlt:")) != -1) {
		switch (opt) {
			case 'h' :
				isHead = TRUE;
				break;
			case 'l' :
				isLine = TRUE;
				break;
			case 't' :
				nTab = atoi (optarg);
				if (nTab > 0)
				{
					isTab = TRUE;
					break;
				}
			default :
				usage (argv[0]);
		}
	}

	if (optind >= argc)
		usage (argv[0]);

	for (i = optind; i < argc; i++)
	{
		filecat (argv[i], isLine, isHead, isTab, nTab);
		puts ("");
	}

	return 0;
}

