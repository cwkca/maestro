#include "player.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "synth.h"

typedef struct
{
    const char *name;
    int (*const handler)(const char *);
} Command;

#define delay_ns 1e7
#define BUFFER_SIZE 256
#define DEFAULT_OCTAVE 4
#define DEFAULT_TEMPO 120
const char COMMAND_MARKER = '*', *WHITESPACE = " \t\r\n";

char file_buffer[BUFFER_SIZE];
SongBuffer *curr_song;
int curr_voice, curr_octave, tempo;

/* Temp */
int note_count = 0;

/* Private function prototypes */
int set_up_voice(const char *line);
int handle_command(const char *command, const char *args);
int read_note(const char *token);
int handle_title(const char *title);
int handle_tempo(const char *value);
void sleep_test();

Command COMMAND_HANDLERS[] = {
    {"title", handle_title},
    {"tempo", handle_tempo}};
#define COMMAND_COUNT 2

int read_song(FILE *file, SongBuffer *songbuf)
{
    int line_number;
    char *eol, *token, *command, *args;

    curr_song = songbuf;
    curr_voice = 0;
    curr_octave = DEFAULT_OCTAVE;
    tempo = DEFAULT_TEMPO;

    for (line_number = 0; fgets(file_buffer, BUFFER_SIZE, file); line_number++)
    {
        eol = strchr(file_buffer, 0);
        assert(eol > file_buffer);
        if (*--eol != '\n')
        {
            /* Todo: handle long lines */
            printf("Line %d is too long\n", line_number);
            return 1;
        }

        /* Trim trailing whitespace */
        while (strchr(WHITESPACE, *eol))
            *eol-- = 0;

        /* Voice headers must be on their own line */
        if (*file_buffer == 'v')
        {
            if (set_up_voice(file_buffer))
                return 1;
            continue;
        }

        for (token = strtok(file_buffer, WHITESPACE); token;
             token = strtok(NULL, WHITESPACE))
            if (*token == COMMAND_MARKER)
            {
                command = token + 1;
                args = strchr(token, 0) + 1;

                if (args > eol)
                {
                    /* Todo: support commands without args */
                    printf("Command %s on line %d is missing arguments\n", command, line_number);
                    return 1;
                }

                if (handle_command(command, args))
                    return 1;

                /* Don't continue to tokenize this line */
                break;
            }
            else if (read_note(token))
                return 1;
    }

    if (!feof(file))
    {
        perror("File error");
        return 1;
    }

    printf("Read %d notes\n", note_count);

    songbuf_reset(songbuf);
    return 0;
}

void play_song(SongBuffer *songbuf)
{
}

/*
 * Private functions
 */

int set_up_voice(const char *line)
{
    printf("Found voice header: '%s'\n", line);
    return 0;
}

int handle_command(const char *command, const char *args)
{
    char i;
    for (i = 0; i < COMMAND_COUNT; i++)
        if (!strcmp(COMMAND_HANDLERS[i].name, command))
            return COMMAND_HANDLERS[i].handler(args);

    printf("Unrecognized command '%s'\n", command);
    return 1;
}

int read_note(const char *token)
{
    if (strcmp(token, "|"))
        note_count++;
    return 0;
}

int handle_title(const char *title)
{
    if (!title || !*title)
        return 1;
    printf("Playing %s\n", title);
    return 0;
}

int handle_tempo(const char *value)
{
    if (!value || !*value)
        return 1;

    char *end;
    tempo = (int)strtol(value, &end, 10);

    if (*end)
    {
        printf("Failed to parse tempo: '%s'\n", value);
        return 1;
    }

    printf("Set tempo to %d\n", tempo);
    return 0;
}

void sleep_test()
{
    int i, sec;
    long nsec, interval, error_ns;
    struct timespec start, end, delay = {0};
    delay.tv_nsec = delay_ns;

    for (i = 0; i < 100; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        nanosleep(&delay, NULL);
        clock_gettime(CLOCK_MONOTONIC, &end);

        sec = end.tv_sec - start.tv_sec;
        nsec = end.tv_nsec - start.tv_nsec;
        interval = 1e9 * sec + nsec;

        error_ns = interval - delay_ns;
        delay.tv_nsec -= error_ns >> 2;
        printf("Overslept by %ld usec. New delay %ld usec.\n",
               error_ns / 1000, delay.tv_nsec / 1000);
    }
}
