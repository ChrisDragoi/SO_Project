#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>

int total_lines_written = 0;

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

void process_bmp_file(char *input_path, char *output_path)
{
    int fd_bmp = open(input_path, O_RDONLY);
    if (fd_bmp == -1)
    {
        perror("Some error at opening the BMP file:(  ");
        exit(-1);
    }

    int fd_out = creat(output_path, S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out == -1)
    {
        perror("Some error at creation of the BMP statistics file:(  ");
        exit(-1);
    }

    int read_b, write_b;

    char signature[2];
    read_b = read(fd_bmp, signature, 2);
    if (read_b == -1)
        r_error();

    if (signature[0] != 'B' || signature[1] != 'M')
    {
        fprintf(stderr, "Not a valid BMP file\n");
        exit(-1);
    }

    // Nume fisier
    write(fd_out, "nume fisier: ", strlen("nume fisier: "));
    write(fd_out, input_path, strlen(input_path));
    write(fd_out, "\n", sizeof("\n"));

    int width, height;

    lseek(fd_bmp, 18, 0);
    read_b = read(fd_bmp, &width, 4);
    if (read_b == -1)
        r_error();
    else
    {
        write(fd_out, "latime: ", strlen("latime: "));
        char str_width[20];
        sprintf(str_width, "%d", width);
        if ((write_b = write(fd_out, str_width, strlen(str_width))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));
    }

    read_b = read(fd_bmp, &height, 4);
    if (read_b == -1)
        r_error();
    else
    {
        write(fd_out, "inaltime: ", strlen("inaltime: "));
        char str_height[20];
        sprintf(str_height, "%d", height);
        if ((write_b = write(fd_out, str_height, strlen(str_height))) == -1)
            wr_error();
        write(fd_out, "\n", sizeof("\n"));
    }
    // stat data
    struct stat data;
    char dimensiune[5], user_id[20], time[20], acces[10];
    if (lstat(input_path, &data) == -1)
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

    if (close(fd_bmp) == -1)
    {
        close_error();
    }
    if (close(fd_out) == -1)
    {
        close_error();
    }
    exit(9);
}

void process_regular_file(char *input_path, char *output_path)
{
    struct stat data;
    char dimensiune[20], user_id[20], time[20];
    int write_b;

    if (lstat(input_path, &data) == -1)
    {
        perror("Eroare la functia lstat:(    ");
        exit(-1);
    }
    if (strstr(input_path, ".bmp") != NULL)
    {
        process_bmp_file(input_path, output_path);
        return;
    }
    int fd_out = creat(output_path, S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out == -1)
    {
        perror("Some error at creation of the statistic file:(  ");
        exit(-1);
    }

    // Nume fisier
    write(fd_out, "nume fisier: ", strlen("nume fisier: "));
    write(fd_out, input_path, strlen(input_path));
    write(fd_out, "\n", sizeof("\n"));

    // Dimensiune
    write(fd_out, "dimensiune: ", strlen("dimensiune: "));
    sprintf(dimensiune, "%ld", data.st_blksize);
    if ((write_b = write(fd_out, dimensiune, strlen(dimensiune))) == -1)
        perror("Some error at writing:(  ");
    write(fd_out, "\n", sizeof("\n"));

    // Utilizator ID
    write(fd_out, "utilizator id: ", strlen("utilizator id: "));
    sprintf(user_id, "%d", data.st_uid);
    if ((write_b = write(fd_out, user_id, strlen(user_id))) == -1)
        perror("Some error at writing:(  ");
    write(fd_out, "\n", sizeof("\n"));

    // Timpul ultimei modificari
    write(fd_out, "timpul ultimei modificari: ", strlen("timpul ultimei modificari: "));
    sprintf(time, "%ld", data.st_atime);
    if ((write_b = write(fd_out, time, strlen(time))) == -1)
        perror("Some error at writing:(  ");
    write(fd_out, "\n", sizeof("\n"));

    // Drepturi de acces
    write(fd_out, "drepturi de acces user: ", strlen("drepturi de acces user: "));
    write(fd_out, (data.st_mode & S_IRUSR) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWUSR) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXUSR) ? "X" : "-", 1);
    write(fd_out, "\n", sizeof("\n"));

    // Drepturi de acces grup
    write(fd_out, "drepturi de acces grup: ", strlen("drepturi de acces grup: "));
    write(fd_out, (data.st_mode & S_IRGRP) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWGRP) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXGRP) ? "X" : "-", 1);
    write(fd_out, "\n", sizeof("\n"));

    // Drepturi de acces altii
    write(fd_out, "drepturi de acces altii: ", strlen("drepturi de acces altii: "));
    write(fd_out, (data.st_mode & S_IROTH) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWOTH) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXOTH) ? "X" : "-", 1);
    write(fd_out, "\n", sizeof("\n"));

    if (close(fd_out) == -1)
    {
        perror("Some error at closing:(  ");
        exit(-1);
    }
    exit(7);
}

void process_directory(char *dir_path, char *output_path)
{
    struct stat data;

    if (lstat(dir_path, &data) == -1)
    {
        perror("Eroare la functia lstat:(    ");
        exit(-1);
    }

    int fd_out = creat(output_path, S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out == -1)
    {
        perror("Some error at creation of the statistic file:(  ");
        exit(-1);
    }

    // Nume director
    write(fd_out, "nume director: ", strlen("nume director: "));
    write(fd_out, dir_path, strlen(dir_path));
    write(fd_out, "\n", sizeof("\n"));

    // user
    write(fd_out, "identificatorul utilizatorului: ", strlen("identificatorul utilizatorului: "));
    char user_id[20];
    sprintf(user_id, "%d", data.st_uid);
    if (write(fd_out, user_id, strlen(user_id)) == -1)
    {
        perror("Some error at writing:(  ");
        exit(-1);
    }
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

    if (close(fd_out) == -1)
    {
        perror("Some error at closing:(  ");
        exit(-1);
    }
    exit(5);
}

void process_file(char *input_path, char *output_path)
{
    struct stat data;
    if (lstat(input_path, &data) == -1)
    {
        perror("Eroare la functia lstat:(    ");
        exit(-1);
    }

    if (S_ISDIR(data.st_mode))
    {
        process_directory(input_path, output_path);
    }
    else if (S_ISREG(data.st_mode))
    {
        
        process_regular_file(input_path, output_path);
    }
}

void process_single_directory(char *input_path, char *output_directory)
{
    pid_t pid;
    int status;

    DIR *dir = opendir(input_path);
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
            pid = fork();

            if (pid == -1)
            {
                perror("Error at fork:(  ");
                exit(-1);
            }

            if (pid == 0)
            { // Proces fiu
                char file_path[257];
                char output_path[257];
                snprintf(file_path, 257, "%s/%s", input_path, entry->d_name);//fisier cu directorul in care se afla
                snprintf(output_path, 280, "%s/%s_statistica.txt", output_directory, entry->d_name);//fisier statistica
                process_file(file_path, output_path);
                exit(0); // End fiu
            }
            else
            {
                // Proces parinte
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                {
                    printf("Procesul copil %d scrie %d linii in fisier.\n", pid, WEXITSTATUS(status));
                    total_lines_written += WEXITSTATUS(status);
                }
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s input_directory output_directory\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *input_directory = argv[1];
    char *output_directory = argv[2];

    DIR *dir = opendir(input_directory);
    if (dir == NULL)
    {
        perror("Error opening input directory");
        exit(EXIT_FAILURE);
    }
    closedir(dir);

    process_single_directory(input_directory, output_directory);

    printf("Numarul total de linii este: %d\n", total_lines_written);

    return 0;
}
