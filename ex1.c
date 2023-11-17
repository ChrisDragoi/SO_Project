#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

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

void process_file(char *file_path, int fd_out)
{
    struct stat data;
    char dimensiune[20], user_id[20], time[20];
    int write_b;

    if (lstat(file_path, &data) == -1)
    {
        perror("Eroare la functia lstat:(    ");
        exit(-1);
    }

    // Nume fisier sau director
    if (S_ISDIR(data.st_mode))
    {
        write(fd_out, "nume director: ", strlen("nume director: "));
        write(fd_out, file_path, strlen(file_path));
        write(fd_out, "\n", sizeof("\n"));

        // Identificatorul utilizatorului
        write(fd_out, "identificatorul utilizatorului: ", strlen("identificatorul utilizatorului: "));
        sprintf(user_id, "%d", data.st_uid);
        if ((write_b = write(fd_out, user_id, strlen(user_id))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));

        // Drepturi de acces user, grup, altii pentru director
        write(fd_out, "drepturi de acces user: ", strlen("drepturi de acces user: "));
        write(fd_out, (data.st_mode & S_IRUSR) ? "R" : "-", 1);
        write(fd_out, (data.st_mode & S_IWUSR) ? "W" : "-", 1);
        write(fd_out, (data.st_mode & S_IXUSR) ? "X" : "-", 1);
        write(fd_out, "\n", sizeof("\n"));

        write(fd_out, "drepturi de acces grup: ", strlen("drepturi de acces grup: "));
        write(fd_out, (data.st_mode & S_IRGRP) ? "R" : "-", 1);
        write(fd_out, (data.st_mode & S_IWGRP) ? "W" : "-", 1);
        write(fd_out, (data.st_mode & S_IXGRP) ? "X" : "-", 1);
        write(fd_out, "\n", sizeof("\n"));

        write(fd_out, "drepturi de acces altii: ", strlen("drepturi de acces altii: "));
        write(fd_out, (data.st_mode & S_IROTH) ? "R" : "-", 1);
        write(fd_out, (data.st_mode & S_IWOTH) ? "W" : "-", 1);
        write(fd_out, (data.st_mode & S_IXOTH) ? "X" : "-", 1);
        write(fd_out, "\n", sizeof("\n"));
    }
    else if (S_ISLNK(data.st_mode))
    {
        char link_target[256];
        ssize_t link_size = readlink(file_path, link_target, sizeof(link_target) - 1);
        if (link_size == -1)
        {
            perror("Error reading symbolic link target");
            exit(-1);
        }
        link_target[link_size] = '\0';

        write(fd_out, "nume legatura: ", strlen("nume legatura: "));
        write(fd_out, file_path, strlen(file_path));
        write(fd_out, "\n", sizeof("\n"));

        // Dimensiune legatura
        write(fd_out, "dimensiune legatura: ", strlen("dimensiune legatura: "));
        sprintf(dimensiune, "%ld", data.st_size);
        if ((write_b = write(fd_out, dimensiune, strlen(dimensiune))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));

        // Dimensiune fisier target
        write(fd_out, "dimensiune fisier dimensiunea fisierului target: ", strlen("dimensiune fisier dimensiunea fisierului target: "));
        sprintf(dimensiune, "%ld", data.st_blksize);
        if ((write_b = write(fd_out, dimensiune, strlen(dimensiune))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));

        // Drepturi de acces user, grup, altii pentru legatura simbolica
        write(fd_out, "drepturi de acces user legatura: ", strlen("drepturi de acces user legatura: "));
        write(fd_out, (data.st_mode & S_IRUSR) ? "R" : "-", 1);
        write(fd_out, (data.st_mode & S_IWUSR) ? "W" : "-", 1);
        write(fd_out, (data.st_mode & S_IXUSR) ? "X" : "-", 1);
        write(fd_out, "\n", sizeof("\n"));

        write(fd_out, "drepturi de acces grup legatura: ", strlen("drepturi de acces grup legatura: "));
        write(fd_out, (data.st_mode & S_IRGRP) ? "R" : "-", 1);
        write(fd_out, (data.st_mode & S_IWGRP) ? "W" : "-", 1);
        write(fd_out, (data.st_mode & S_IXGRP) ? "X" : "-", 1);
        write(fd_out, "\n", sizeof("\n"));

        write(fd_out, "drepturi de acces altii legatura: ", strlen("drepturi de acces altii legatura: "));
        write(fd_out, (data.st_mode & S_IROTH) ? "R" : "-", 1);
        write(fd_out, (data.st_mode & S_IWOTH) ? "W" : "-", 1);
        write(fd_out, (data.st_mode & S_IXOTH) ? "X" : "-", 1);
        write(fd_out, "\n", sizeof("\n"));
    }

    else if (S_ISREG(data.st_mode))
    {
        write(fd_out, "nume fisier: ", strlen("nume fisier: "));
        write(fd_out, file_path, strlen(file_path));
        write(fd_out, "\n", sizeof("\n"));

        // citeste si scrie latime
        write(fd_out, "latime: ", strlen("latime: "));
        write(fd_out, "N/A", strlen("N/A"));
        write(fd_out, "\n", sizeof("\n"));

        // citeste si scrie inaltime
        write(fd_out, "inaltime: ", strlen("inaltime: "));
        write(fd_out, "N/A", strlen("N/A"));
        write(fd_out, "\n", sizeof("\n"));

        // dimensiune
        write(fd_out, "dimensiune: ", strlen("dimensiune: "));
        sprintf(dimensiune, "%ld", data.st_blksize);
        if ((write_b = write(fd_out, dimensiune, strlen(dimensiune))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));

        // utilizator id
        write(fd_out, "utilizator id: ", strlen("utilizator id: "));
        sprintf(user_id, "%d", data.st_uid);
        if ((write_b = write(fd_out, user_id, strlen(user_id))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));

        // timpul ultimei modificari
        write(fd_out, "timpul ultimei modificari: ", strlen("timpul ultimei modificari: "));
        sprintf(time, "%ld", data.st_atime);
        if ((write_b = write(fd_out, time, strlen(time))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));

        // drepturi de acces
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

        // Drepturi de acces grup
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

        // Drepturi de acces altii
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

    // deschidere fisier de statistica
    int fd_out = creat("statistica.txt", S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out == -1)
    {
        perror("Some error at creation of the statistic file:(  ");
        exit(-1);
    }

    // deschidere director
    DIR *dir = opendir(argv[1]);
    if (dir == NULL)
    {
        perror("Some error opening directory:(  ");
        exit(-1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char file_path[257];
            snprintf(file_path, sizeof(file_path), "%s/%s", argv[1], entry->d_name);
            process_file(file_path, fd_out);
            write(fd_out, "\n", 1);
        }
    }

    // inchidere
    if (closedir(dir) == -1)
    {
        perror("Some error closing directory:(  ");
        exit(-1);
    }
    if (close(fd_out) == -1)
    {
        perror("Some error at closing:(  ");
        exit(-1);
    }

    return 0;
}
