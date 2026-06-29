#include "player.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

#include "synth.h"

typedef struct
{
    const char *name;
    int (*const handler)(const char *);
} Command;

#define BUFFER_SIZE 256
#define DEFAULT_OCTAVE 4
#define DEFAULT_TEMPO 120
#define TICKS_PER_QUARTER 96
#define TIMING_LAG_NS 2e6

const char COMMAND_MARKER = '*', *WHITESPACE = " \t\r\n";
const char NOTE_TONES[] = {9, 11, 0, 2, 4, 5, 7};

char file_buffer[BUFFER_SIZE];
NoteBuffer *curr_song;
signed char curr_voice, curr_octave;
int tempo;

/* Private function prototypes */
int set_up_voice(const char *line);
int handle_command(const char *command, const char *args);
int read_note(const char *note);
uint16_t parse_duration(char value);
void store_note(signed char note_number, uint16_t duration);
int handle_title(const char *title);
int handle_tempo(const char *value);

Command COMMAND_HANDLERS[] = {
    {"title", handle_title},
    {"tempo", handle_tempo}};
#define COMMAND_COUNT 2

int init_player()
{
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    if (SDL_Init(SDL_INIT_AUDIO))
    {
        printf("SDL error: %s\n", SDL_GetError());
        return 1;
    }

    init_synth();
    return 0;
}

int read_song(FILE *file, NoteBuffer *songbuf)
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
            else if (strcmp(token, "|")) /* Skip barlines */
                if (read_note(token))
                {
                    printf("Invalid note '%s'\n", token);
                    return 1;
                }
    }

    if (ferror(file))
    {
        perror("File error");
        return 1;
    }

    assert(feof(file));
    notebuf_reset(songbuf);
    return 0;
}

void player_cleanup()
{
    SDL_Quit();
    synth_cleanup();
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

int read_note(const char *note)
{
    if (!note || strlen(note) < 2)
        return 1;

    char tone, note_name = tolower(*note);
    signed char note_number;
    const char *strpos = note + 1;
    uint16_t duration;

    if (note_name == 'r')
        note_number = -1;
    else if (note_name >= 'a' && note_name < 'h')
    {
        tone = NOTE_TONES[note_name - 'a'];

        if (*strpos == 'b')
        {
            tone--;
            strpos++;
        }
        else if (*strpos == '#')
        {
            tone++;
            strpos++;
        }

        if (!*strpos)
            return 1;
        if (isdigit(*strpos))
            curr_octave = *strpos++ - '0';

        note_number = 12 + 12 * curr_octave + tone;
    }
    else
        return 1;

    if (!*strpos)
        return 1;
    duration = parse_duration(*strpos++);
    if (!duration)
        return 1;
    if (*strpos == '.')
    {
        duration += duration >> 1;
        strpos++;
    }

    if (*strpos)
        return 1;

    store_note(note_number, duration);
    return 0;
}

uint16_t parse_duration(char value)
{
    switch (value)
    {
    case 'w':
        return TICKS_PER_QUARTER << 2;
    case 'h':
        return TICKS_PER_QUARTER << 1;
    case 'q':
        return TICKS_PER_QUARTER;
    case 'e':
        return TICKS_PER_QUARTER >> 1;
    case 's':
        return TICKS_PER_QUARTER >> 2;
    case 't':
        return TICKS_PER_QUARTER >> 3;
    default:
        return 0;
    }
}

void store_note(signed char note_number, uint16_t duration) {}

int handle_title(const char *title)
{
    if (!title || !*title)
        return 1;
    printf("Playing %s\n", title);
    return 0;
}

int handle_tempo(const char *value)
{
    if (!value || !*value || strlen(value) > 3)
        return 1;

    char *end;
    tempo = (int)strtol(value, &end, 10);

    if (*end)
    {
        printf("Failed to parse tempo: '%s'\n", value);
        return 1;
    }

    return 0;
}
