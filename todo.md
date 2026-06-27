## Synth keyboard
- Support multiple voices

## Record and playback
- Enter to start and stop recording
- Record in a vector as a midi-like event stream
  - 0x8 [channel] [note] = off
  - 0x9 [channel] [note] = on
  - 0b11 [ms] = delay
- When stopped, SDL_MinimizeWindow to request a name from the terminal
- Empty means delete (confirm)
- SDL_RaiseWindow to return to window
- Space to play. If multiple, list names in terminal and choose by number.

## Save and load songs
- Midi files
- Instruments: Square (81), Saw (82), Triangle (83)
- Blank non-filename chars with underscore
- Unsupported instrument: warn, default to square wave

## Synth upgrades
- ADSR envelopes
- Support additional voices

## Soundfont wavetable synth
- Sample soundfont: FluidR3 GM
- Read metadata with sf2dump
- Work off TinySoundFont for parsing
