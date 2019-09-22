#include "common.h"


int is_number(char * s)
{
    for (int i = 0; i < strlen (s); i++)
    {
        if (!isdigit (s[i])) return(0);
    }

    return(1);
}


char * get_file_content(char * fp, char * opt)
{
	FILE * f;
	long length;
	char * content;


	f = fopen (fp, opt);
	if (f == NULL)
	{
		perror("	file not found");
		return(NULL);
	}


	fseek (f, 0, SEEK_END);
	length = ftell (f);
	if (length < 2)
	{
		perror("	File has no content");
		fclose(f);
		return(NULL);
	}


	fseek (f, 0, SEEK_SET);
	content = malloc (length);
	if (content)
	{
		fread (content, 1, length, f);
	}
	fclose(f);


	return content;
}