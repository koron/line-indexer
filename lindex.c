#include <stdio.h>

static off64_t my_fgetpos(FILE *file)
{
    fpos_t pos;
    if (fgetpos(file, &pos) != 0)
        return -1LL;
    else
        return (off64_t)pos;
}

static int index_line2(FILE *infile, FILE *outfile)
{
    off64_t off = 0;
    while (1)
    {
        switch (fgetc(infile))
        {
            case EOF:
                if (feof(infile) != 0)
                {
                    if (off != my_fgetpos(infile))
                        fprintf(outfile, "%lld\n", off);
                    return 0;
                }
                else
                    return 1;
            case '\n':
                fprintf(outfile, "%lld\n", off);
                off = my_fgetpos(infile);
                if (off < 0)
                    return 1;
        }
    }
}

static int index_line(char *targetPath, char *indexPath)
{
    FILE *infile = fopen(targetPath, "rb");
    FILE *outfile = fopen(indexPath, "wb");
    int retval = 1;
    if (infile != NULL && outfile != NULL)
        retval = index_line2(infile, outfile);
    if (outfile != NULL)
        fclose(outfile);
    if (infile != NULL)
        fclose(infile);
    if (retval != 0)
        unlink(indexPath);
    return retval;
}

int main(int argc, char **argv)
{
    if (argc >= 3)
        return index_line(argv[1], argv[2]);
    else
        return 1;
}
