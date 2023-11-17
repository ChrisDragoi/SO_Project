#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

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
    char str_width[20], str_height[20];

    // scriere in fisier nume
    write(fd_out, "nume fisier: ", strlen("nume fisier: "));
    write(fd_out, argv[1], strlen(argv[1]));
    write(fd_out, "\n", sizeof("\n"));

    // citeste si scrie latime
    read_b = read(fd_in, &width, 4);
    if (read_b == -1)
        r_error();
    else
    {
        write(fd_out, "latime: ", strlen("latime: "));
        sprintf(str_width, "%d", width);
        if ((write_b = write(fd_out, str_width, strlen(str_width))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));
    }
    // citeste si scrie inaltime
    read_b = read(fd_in, &height, 4);
    if (read_b == -1)
        r_error();
    else
    {
        write(fd_out, "inaltime: ", strlen("inaltime: "));
        sprintf(str_height, "%d", height);
        if ((write_b = write(fd_out, str_height, strlen(str_height))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));
    }

    // stat data
    struct stat data;
    char dimensiune[5], user_id[20], time[20], acces[10];
    if (stat(argv[1], &data) == -1)
    {
        perror("Eroare la functia stat:(    ");
        exit(-1);
    }
    sprintf(dimensiune, "%ld", data.st_blksize);
    sprintf(user_id, "%d", data.st_uid);
    sprintf(time, "%ld", data.st_atime);
    sprintf(acces, "%d", data.st_mode);

    // dimensiune
    write(fd_out, "dimensiune: ", strlen("dimensiune: "));
    if ((write_b = write(fd_out, dimensiune, strlen(dimensiune))) == -1)
        wr_error();
    write(fd_out, "\n", sizeof("\n"));

    // utilizator id
    write(fd_out, "utilizator id: ", strlen("utilizator id: "));
    if ((write_b = write(fd_out, user_id, strlen(user_id))) == -1)
        wr_error();
    write(fd_out, "\n", sizeof("\n"));

    // timpul ultimei modificari
    write(fd_out, "timpul ultimei modificari: ", strlen("timpul ultimei modificari: "));
    time_t last_modified_time = data.st_mtime;
    char *formatted_time = ctime(&last_modified_time);
    if (formatted_time[strlen(formatted_time) - 1] == '\n')
    {
        formatted_time[strlen(formatted_time) - 1] = '\0';
    }
    if ((write_b = write(fd_out, formatted_time, strlen(formatted_time))) == -1)
        wr_error();
    write(fd_out, "\n", sizeof("\n"));

    // drepturi de acces user
    write(fd_out, "drepturi de acces user: ", strlen("drepturi de acces user: "));
    if (data.st_mode & S_IRUSR)
    {
        write(fd_out, "R", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    if (data.st_mode & S_IWUSR)
    {
        write(fd_out, "W", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    if (data.st_mode & S_IXUSR)
    {
        write(fd_out, "X", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    write(fd_out, "\n", sizeof("\n"));

    // drepturi de acces grup
    write(fd_out, "drepturi de acces grup: ", strlen("drepturi de acces grup: "));
    if (data.st_mode & S_IRGRP)
    {
        write(fd_out, "R", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    if (data.st_mode & S_IWGRP)
    {
        write(fd_out, "W", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    if (data.st_mode & S_IXGRP)
    {
        write(fd_out, "X", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    write(fd_out, "\n", sizeof("\n"));

    // drepturi de acces altii
    write(fd_out, "drepturi de acces altii: ", strlen("drepturi de acces altii: "));
    if (data.st_mode & S_IROTH)
    {
        write(fd_out, "R", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    if (data.st_mode & S_IWOTH)
    {
        write(fd_out, "W", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    if (data.st_mode & S_IXOTH)
    {
        write(fd_out, "X", 1);
    }
    else
    {
        write(fd_out, "-", 1);
    }
    write(fd_out, "\n", sizeof("\n"));

    // inchidere fisiere
    closing_files(fd_in, fd_out);

    return 0;
}