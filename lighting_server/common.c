#include "common.h"


int is_number(char * s)
{
    for (int i = 0; i < strlen (s); i++)
    {
        if (!isdigit (s[i])) return(0);
    }

    return(1);
}


int read_button_layout()
{
    FILE * f;
    long length;


    f = fopen ("buttons.json", "rb");
    if (f == NULL)
    {
		printf("file not found");
		return(-1);
    }


	fseek (f, 0, SEEK_END);
	length = ftell (f);
	if (length < 2)
	{
		printf("Nothing to read from file");
		return(-2);
	}


	fseek (f, 0, SEEK_SET);
	button_layout = malloc (length);
	if (button_layout)
	{
		fread (button_layout, 1, length, f);
	}
	fclose (f);


    return(1);
}