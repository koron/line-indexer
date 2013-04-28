#include <stdio.h>

static int has_lf(const char *p)
{
    while (1)
    {
        switch (*p++)
        {
            case '\0':
                return 0;
            case '\n':
                return 1;
        }
    }
}

static void skip_to_next(FILE *file)
{
    while (1)
    {
        switch (fgetc(file))
        {
            case EOF:
            case '\n':
                return;
        }
    }
}

static fpos_t read_fpos(FILE *idx)
{
    fpos_t pos = -1LL;
    char buf[100];
    char *p;

    /* read from idx file. */
    p = fgets(buf, sizeof(buf), idx);
    if (p == NULL)
        return pos;

    /* extract 1st column as pos. */
    if (sscanf(p, "%lld", &pos) != 1)
        pos = -1LL;

    /* skip to next line. */
    if (has_lf(p) == 0)
        skip_to_next(idx);

    return pos;
}

static void copy_line(FILE *src, FILE *out)
{
    while (1)
    {
        int ch = fgetc(src);
        switch (ch)
        {
            case EOF:
                return;

            case '\n':
                fputc(ch, out);
                return;

            default:
                fputc(ch, out);
                break;
        }
    }
}

static int line_get2(FILE *src, FILE *idx, FILE *out)
{
    while (1)
    {
        fpos_t pos = read_fpos(idx);
        printf("pos=%lld\n", pos);
        if (pos < 0)
        {
            if (feof(idx) != 0)
                return 0;
            else if (ferror(idx) != 0)
                return 1;
            else
                continue;
        }

        if (fsetpos(src, &pos) == 0)
            copy_line(src, out);
    }
}

static int line_get(const char *src, const char *idx, const char *out)
{
    FILE *srcfile = fopen(src, "rb");
    FILE *idxfile = fopen(idx, "rb");
    FILE *outfile = fopen(out, "wb");
    int retval = 1;

    if (srcfile != NULL && idxfile != NULL && outfile != NULL)
        retval = line_get2(srcfile, idxfile, outfile);

    if (outfile != NULL)
        fclose(outfile);
    if (idxfile != NULL)
        fclose(idxfile);
    if (outfile != NULL)
        fclose(outfile);
    if (retval != 0)
        unlink(out);

    return retval;
}

int main(int argc, char **argv)
{
    if (argc >= 4)
        return line_get(argv[1], argv[2], argv[3]);
    else
        return 1;
}
