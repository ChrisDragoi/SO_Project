#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void wr_error()
{
    perror("Some error at writing:(  ");
    exit(-1);
}
void r_error()
{
    perror("Some error at reading:(  ");
    exit(-1);
}

void close_error()
{
    perror("Some error at closing:(  ");
    exit(-1);
}

void closing_files(int in, int out)
{
    if (close(in) == -1)
    {
        close_error();
    }
    if (close(out) == -1)
    {
        close_error();
    }
}
int main(int argc, char *argv[])
{
    // verificare argumente linie de comanda
    if (argc != 2)
    {
        perror("Some error that is probably caused by entering incorrect arguments for runing the program:(  \nYou need to have ony one aargument that reads a image file.");
        exit(-1);
    }

    // deschidere fisier poza
    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1)
    {
        perror("Some error at opening the file:(  ");
        exit(-1);
    }

    // creare fisier de statistica
    int fd_out = creat("stat.txt", S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out == -1)
    {
        perror("Some error at creation of the statistic file:(  ");
        exit(-1);
    }

    // cursor, date de scris in fisierul statistica
    lseek(fd_in, 18, 0);

    int width, height;
    int read_b, write_b;

    // scriere in fisier
    write(fd_out, "nume fisier: ", sizeof("nume fisier: "));
    write(fd_out, argv[1], sizeof(argv[1]));
    write(fd_out,"\n",sizeof("\n"));
    read_b = read(fd_in, &width, 4);
    if (read_b == -1)
        r_error();
    else
    {
        write(fd_out, "latime: ", sizeof("latime: "));
        if ((write_b = write(fd_out, &width, 1)) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));
    }
    read_b = read(fd_in, &width, 4);
    if (read_b == -1)
        r_error();
    else
    {
        write(fd_out, "inaltime: ", sizeof("inaltime: "));
        if ((write_b = write(fd_out, &height, 1)) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));
    }

    // inchidere fisiere
    closing_files(fd_in, fd_out);

    return 0;
}