#include <stdio.h>
#include <sys/stat.h>

#include "player.h"

struct stat file_stat;
SongBuffer song_data;

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s [song.nts]\n", *argv);
        return 1;
    }

    const char *song_filename = argv[1];
    if (stat(song_filename, &file_stat))
    {
        perror("File error");
        return 1;
    }

    int file_size = file_stat.st_size;
    songbuf_init(&song_data, file_size + (file_size >> 2));

    FILE *song_file = fopen(song_filename, "r");
    if (!song_file)
    {
        perror("File error");
        return 1;
    }

    int result = read_song(song_file, &song_data);
    fclose(song_file);
    if (result != 0)
        return 1;

    play_song(&song_data);

    songbuf_cleanup(&song_data);
    return 0;
}
