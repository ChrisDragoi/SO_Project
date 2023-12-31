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

void run_bash_counter(const char *file_content, char c)
{
    char command[1000];
    sprintf(command, "bash script.sh %c", c);

    // Open pipe to bash script input
    FILE *pipe = NULL;
    if ((pipe = popen(command, "w")) == NULL)
    {
        exit(-1);
    }

    // Feed file content to bash script
    fprintf(pipe, "%s", file_content);

    if (pclose(pipe) == -1)
    {
        exit(-1);
    }
}

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
    write(fd_out, "\n", 1);

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
        write(fd_out, "\n", 1);
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
        write(fd_out, "\n", 1);
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
    write(fd_out, "\n", 1);

    // utilizator id
    write(fd_out, "utilizator id: ", strlen("utilizator id: "));
    if ((write_b = write(fd_out, user_id, strlen(user_id))) == -1)
        wr_error();
    write(fd_out, "\n", 1);

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
    write(fd_out, "\n", 1);

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
    write(fd_out, "\n", 1);

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

void sentences(char *input_path, char character)
{
    int status;
    if (strstr(input_path, ".bmp") == NULL)
    {
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1)
        {
            perror("Error creating pipe:(  ");
            exit(-1);
        }

        pid_t pid_content = fork();
        if (pid_content == -1)
        {
            perror("Error at fork:(  ");
            exit(-1);
        }

        if (pid_content == 0)
        {
            close(pipe_fd[0]);
            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[1]);
            run_bash_counter(input_path, character);

            exit(0);
        }
        else
        {
            waitpid(pid_content, &status, 0);
            int nr = 0;
            close(pipe_fd[1]);
            read(pipe_fd[0], &nr, sizeof(int));
            close(pipe_fd[0]);
            printf("S-a încheiat procesul NUMARARE cu pid-ul %d și codul %d.\n", pid_content, WEXITSTATUS(status));
            printf("Au fost identificate in total %d propozitii corecte care contin caracterul %c.\n", nr, character);
        }
    }
}
void process_regular_file(char *input_path, char *output_path, char character)
{
    struct stat data;
    char dimensiune[20], user_id[20], time[20];
    int write_b;

    if (lstat(input_path, &data) == -1)
    {
        perror("Eroare la functia lstat:(    ");
        exit(-1);
    }
    // bmp
    if (strstr(input_path, ".bmp") != NULL)
    {
        process_bmp_file(input_path, output_path);
        return;
    }
    // propozitii cu pipe
    sentences(input_path, character);
    // procesare regular file
    int fd_out = creat(output_path, S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out == -1)
    {
        perror("Some error at creation of the statistic file:(  ");
        exit(-1);
    }

    // Nume fisier
    write(fd_out, "nume fisier: ", strlen("nume fisier: "));
    write(fd_out, input_path, strlen(input_path));
    write(fd_out, "\n", 1);

    // Dimensiune
    write(fd_out, "dimensiune: ", strlen("dimensiune: "));
    sprintf(dimensiune, "%ld", data.st_blksize);
    if ((write_b = write(fd_out, dimensiune, strlen(dimensiune))) == -1)
        perror("Some error at writing:(  ");
    write(fd_out, "\n", 1);

    // Utilizator ID
    write(fd_out, "utilizator id: ", strlen("utilizator id: "));
    sprintf(user_id, "%d", data.st_uid);
    if ((write_b = write(fd_out, user_id, strlen(user_id))) == -1)
        perror("Some error at writing:(  ");
    write(fd_out, "\n", 1);

    // Timpul ultimei modificari
    write(fd_out, "timpul ultimei modificari: ", strlen("timpul ultimei modificari: "));
    sprintf(time, "%ld", data.st_atime);
    if ((write_b = write(fd_out, time, strlen(time))) == -1)
        perror("Some error at writing:(  ");
    write(fd_out, "\n", 1);

    // Drepturi de acces
    write(fd_out, "drepturi de acces user: ", strlen("drepturi de acces user: "));
    write(fd_out, (data.st_mode & S_IRUSR) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWUSR) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXUSR) ? "X" : "-", 1);
    write(fd_out, "\n", 1);

    // Drepturi de acces grup
    write(fd_out, "drepturi de acces grup: ", strlen("drepturi de acces grup: "));
    write(fd_out, (data.st_mode & S_IRGRP) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWGRP) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXGRP) ? "X" : "-", 1);
    write(fd_out, "\n", 1);

    // Drepturi de acces altii
    write(fd_out, "drepturi de acces altii: ", strlen("drepturi de acces altii: "));
    write(fd_out, (data.st_mode & S_IROTH) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWOTH) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXOTH) ? "X" : "-", 1);
    write(fd_out, "\n", 1);

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
    write(fd_out, "\n", 1);

    // user
    write(fd_out, "identificatorul utilizatorului: ", strlen("identificatorul utilizatorului: "));
    char user_id[20];
    sprintf(user_id, "%d", data.st_uid);
    if (write(fd_out, user_id, strlen(user_id)) == -1)
    {
        perror("Some error at writing:(  ");
        exit(-1);
    }
    write(fd_out, "\n", 1);

    // Drepturi de acces user, grup, altii pentru director
    write(fd_out, "drepturi de acces user: ", strlen("drepturi de acces user: "));
    write(fd_out, (data.st_mode & S_IRUSR) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWUSR) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXUSR) ? "X" : "-", 1);
    write(fd_out, "\n", 1);

    write(fd_out, "drepturi de acces grup: ", strlen("drepturi de acces grup: "));
    write(fd_out, (data.st_mode & S_IRGRP) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWGRP) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXGRP) ? "X" : "-", 1);
    write(fd_out, "\n", 1);

    write(fd_out, "drepturi de acces altii: ", strlen("drepturi de acces altii: "));
    write(fd_out, (data.st_mode & S_IROTH) ? "R" : "-", 1);
    write(fd_out, (data.st_mode & S_IWOTH) ? "W" : "-", 1);
    write(fd_out, (data.st_mode & S_IXOTH) ? "X" : "-", 1);
    write(fd_out, "\n", 1);

    if (close(fd_out) == -1)
    {
        perror("Some error at closing:(  ");
        exit(-1);
    }
    exit(5);
}

void process_file(char *input_path, char *output_path, char character)

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
        process_regular_file(input_path, output_path, character);
    }
}

void conversion(char *input_path)
{
    if (strstr(input_path, ".bmp") == NULL)
    {
        fprintf(stderr, "Not a BMP file: %s\n", input_path);
        exit(-1);
    }
    int fd_bmp = open(input_path, O_RDWR);
    if (fd_bmp == -1)
    {
        perror("Some error at opening the BMP file:(  ");
        exit(-1);
    }
    int width, height;

    lseek(fd_bmp, 18, 0);
    int read_b = read(fd_bmp, &width, 4);
    if (read_b == -1)
        r_error();

    read_b = read(fd_bmp, &height, 4);
    if (read_b == -1)
        r_error();

    lseek(fd_bmp, 54, SEEK_SET);
    int p_gri;
    unsigned char rgb[3];
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            read_b = read(fd_bmp, rgb, 3);
            p_gri = 0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2];
            lseek(fd_bmp, -3, SEEK_CUR);
            if (write(fd_bmp, &p_gri, 1) == -1)
                wr_error();
            if (write(fd_bmp, &p_gri, 1) == -1)
                wr_error();
            if (write(fd_bmp, &p_gri, 1) == -1)
                wr_error();
        }
    }

    if (close(fd_bmp) == -1)
    {
        close_error();
    }
}

void process_single_directory(char *input_path, char *output_directory, char character)
{
    pid_t pid, pid_img;
    int status;

    DIR *dir = opendir(input_path);
    if (dir == NULL)
    {
        perror("Some error opening directory:(  ");
        exit(-1);
    }
    struct stat data;
    if (lstat(input_path, &data) == -1)
    {
        perror("Eroare la functia lstat:(    ");
        exit(-1);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue; 
        }
        if (strstr(entry->d_name, ".bmp") != NULL)
        {
            pid_img = fork();
            if (pid_img == -1)
            {
                perror("Error at fork:(  ");
                exit(-1);
            }
            if (pid_img == 0)
            {
                char file_path[257];
                snprintf(file_path, 257, "%s/%s", input_path, entry->d_name); // fisier cu directorul in care se afla
                conversion(file_path);
                exit(0);
            }
            else
            {
                // Proces parinte
                waitpid(pid_img, &status, 0);
                if (WIFEXITED(status))
                {
                    printf("S-a incheiat procesul CONVERSIE cu pid-ul %d si codul %d.\n", pid, WEXITSTATUS(status));
                    total_lines_written += WEXITSTATUS(status);
                }
            }
        }
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
            snprintf(file_path, 257, "%s/%s", input_path, entry->d_name);                        // fisier cu directorul in care se afla
            snprintf(output_path, 280, "%s/%s_statistica.txt", output_directory, entry->d_name); // fisier statistica
            process_file(file_path, output_path, character);

            exit(0); // End fiu
        }
        else
        {
            // Proces parinte
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                printf("S-a incheiat procesul cu pid-ul %d si codul %d.\n", pid, WEXITSTATUS(status));
                total_lines_written += WEXITSTATUS(status);
            }
        }
    }

    closedir(dir);
    printf("S-au scris in total %d linii.\n", total_lines_written);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s input_directory output_directory character\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *input_directory = argv[1];
    char *output_directory = argv[2];
    char character = argv[3][0];

    process_single_directory(input_directory, output_directory, character);

    return 0;
}
