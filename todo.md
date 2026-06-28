## Load and play
- Separate executable without sdl input
- Parse custom .nts format to midi data stream

## Record and save
- Enter to start and stop recording
- Record in a vector as a midi-like event stream
  - 0x8 [channel] [note] = off
  - 0x9 [channel] [note] = on
  - 0b11 [ms] = delay
- When stopped, SDL_MinimizeWindow to request a name from the terminal
- Empty means delete (confirm)
- Blank non-filename chars with underscore
- Save in midi format
- SDL_RaiseWindow to return to window
- Parse and play midi

## Synth upgrades
- Square wave duty cycles
- Graphical piano keyboard
- ADSR envelopes

## Soundfont wavetable synth
- Sample soundfont: FluidR3 GM
- Read metadata with sf2dump
- Work off TinySoundFont for parsing
