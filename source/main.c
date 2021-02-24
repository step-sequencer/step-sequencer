// Includes
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "globals.h"
#include "libs/logger.h"
#include "libs/suart_midi.h"
#include "libs/midi.h"
#include "libs/poti.h"
#include "button.h"

uint8_t maxTimeTicks = MIN_TEMPO_BPM / 3;
uint8_t minTimeTicks = MAX_TEMPO_BPM / 3;

// Stores all parameters that represent the current state of the sequencer.
typedef struct
{
    // 0 if sequencer stopped, 1 if running
    uint8_t isRunning;
    // How many notes per time
    uint8_t tempoTicks;
    // Holds the number of ticks until next note can be sent
    uint8_t tempoTicksCounter;
    //Holds current the noteLengthTicks
    uint8_t noteLengthTicks;
    // Is the current release allready used to send a note
    uint8_t currentTickUsedNoteOn;
    // Is the current release allready used to send a noteOff event
    uint8_t currentTickUsedNoteOff;
    // Variable holds release counter when the start / stop button was pressed
    uint8_t startStopTriggeredReleaseCounter;
    // Variable holds release counter when button1 was pressed
    uint8_t button1TriggeredReleaseCounter;
    // Variable holds release counter when button2 was pressed
    uint8_t button2TriggeredReleaseCounter;
    // Variable holds release counter when button3 was pressed
    uint8_t button3TriggeredReleaseCounter;
    // Variable holds release counter when button4 was pressed
    uint8_t button4TriggeredReleaseCounter;
} SequencerState;

// Create a SequencerState
SequencerState sequencerState;

// Struct is used to hold all parameters regarding a Note.
typedef struct
{
    // A MidiNote Object from the midi.h library
    MidiNote midiNote;
    // Holds last MidiNote that was sent
    MidiNote lastSentMidiNote;
    // Flag that is 1 if noteOff was sent, 0 otherwise
    uint8_t noteOffSent;
    // Flag that is 0 if the note is deactivated / 1 if the note is activated
    uint8_t isActivated;
    // If counter is 0, then noteOff must be sent. This counter gets decreased by our interrupt.
    uint8_t noteOffCounter;
} SequencerNote;

// Create a SequencerNote for each note with preset values.
SequencerNote note1;
SequencerNote note2;
SequencerNote note3;
SequencerNote note4;

// Array of SequencerNotes which resembles the loop of our notes.
SequencerNote *sequencerNotes[4] = {&note1, &note2, &note3, &note4};
// currentNoteIndex is used to loop through the sequencerNotes array.
uint8_t currentNoteIndex = 0;

// Method inits the main timer that will be used to play the notes in a specific tempo
void init_base_timer()
{
    //disable interrupts
    cli();
    // Enable CTC mode
    TCCR1B |= (1 << WGM12);
    // Enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    // Set default compare match register value. The interrupt will so be triggered 20 times per second
    OCR1A = 780;
    // Set the prescaler to 1024. As timer1 with a prescaler of 1024 allowes a wide enough frequency range
    // (~ 1000Hz to 0,25Hz)
    TCCR1B |= (1 << CS12) | (1 << CS10);
    // Enable interrupts
    sei();
}

// Method sets ports for LEDs as output ports
void init_led_ports_mcu()
{
    LED_REGISTER = (1 << LED_START_STOP) | (1 << LED_1) | (1 << LED_2) | (1 << LED_3) | (1 << LED_4);
}

// Interrupt method of timer1. It is called in a fixed frequence and decreases the different counters.
ISR(TIMER1_COMPA_vect)
{
    sei();
    // Decrease tempoTicksCounter
    if (sequencerState.tempoTicksCounter > 0)
    {
        sequencerState.tempoTicksCounter--;
    }
    else
    {
        sequencerState.tempoTicksCounter = sequencerState.tempoTicks;
    }

    // Mark if current tick is not used yet to send a note on
    sequencerState.currentTickUsedNoteOn = 0;

    // If noteOffCounter is set, decrease it every tick
    if (note1.noteOffCounter > 0)
    {
        note1.noteOffCounter--;
    }
    if (note2.noteOffCounter > 0)
    {
        note2.noteOffCounter--;
    }
    if (note3.noteOffCounter > 0)
    {
        note3.noteOffCounter--;
    }
    if (note4.noteOffCounter > 0)
    {
        note4.noteOffCounter--;
    }

    // Free currentTickUsedNoteOff to be used by main loop
    sequencerState.currentTickUsedNoteOff = 0;

    // Decreasing the buttons release counter if it was triggered
    if (sequencerState.startStopTriggeredReleaseCounter > 0)
    {
        sequencerState.startStopTriggeredReleaseCounter--;
    }
    if (sequencerState.button1TriggeredReleaseCounter > 0)
    {
        sequencerState.button1TriggeredReleaseCounter--;
    }
    if (sequencerState.button2TriggeredReleaseCounter > 0)
    {
        sequencerState.button2TriggeredReleaseCounter--;
    }
    if (sequencerState.button3TriggeredReleaseCounter > 0)
    {
        sequencerState.button3TriggeredReleaseCounter--;
    }
    if (sequencerState.button4TriggeredReleaseCounter > 0)
    {
        sequencerState.button4TriggeredReleaseCounter--;
    }
}

// Method toggles the output pin power for a led output
void toggle_LED(int8_t led)
{
    LED_PORT ^= (1 << led);
}

// Method starts or stops the sequencer
void start_stop_triggered()
{
    if (sequencerState.isRunning)
    {
        // Shut all LEDs off
        LED_PORT &= ~((1 << LED_START_STOP) | (1 << LED_1) | (1 << LED_2) | (1 << LED_3) | (1 << LED_4));
        // Stop sequencer
        sequencerState.isRunning = 0;
        // Reset currentNoteIndex so it starts again with first note when restarting
        currentNoteIndex = 0;

        send_midi_note_off(&(note1.lastSentMidiNote));
        send_midi_note_off(&(note2.lastSentMidiNote));
        send_midi_note_off(&(note3.lastSentMidiNote));
        send_midi_note_off(&(note4.lastSentMidiNote));
    }
    else
    {
        // Power all LEDs on
        LED_PORT |= (1 << LED_START_STOP) | (1 << LED_1) | (1 << LED_2) | (1 << LED_3) | (1 << LED_4);

        // Reset the isActive flag of all notes to 1 so that all notes will be played when restarting the sequencer
        note1.isActivated = 1;
        note2.isActivated = 1;
        note3.isActivated = 1;
        note4.isActivated = 1;

        // Start sequencer
        sequencerState.isRunning = 1;
    }
}

// Method sets all notes that will be played acording to the given value of a poti (value from 0-1023)
void set_notes(uint16_t potiValue)
{
    // Parse potiValue to Midi Note range specified in globals.h. We substract 11 as the final note is added with 11.
    uint8_t parsedPotiValue = ((potiValue * (NOTE_NUMBER_RANGE - 11)) / 1024) + MIN_NOTE_NUMBER;

    // Set all notes
    note1.midiNote.number = parsedPotiValue;
    note2.midiNote.number = (parsedPotiValue + 4) % 128;
    note3.midiNote.number = (parsedPotiValue + 7) % 128;
    note4.midiNote.number = (parsedPotiValue + 11) % 128;
}

// Method sets velocity for all notes according to the given value of a poti (value from 0-1023)
void set_velocity(uint16_t potiValue)
{
    // Parse potiValue to Midi velocity range
    uint8_t velocity = potiValue / 8;

    // Set the velocity for all notes
    note1.midiNote.velocity = velocity;
    note2.midiNote.velocity = velocity;
    note3.midiNote.velocity = velocity;
    note4.midiNote.velocity = velocity;
}

// Method sets the length for all notes according to the given value of a poti (value from 0-1023)
void set_note_length(uint16_t potiValue)
{
    // Calculate noteOffCounter value. The poti Value is transformed to a value between 1 and the current tempo.
    sequencerState.noteLengthTicks = ((potiValue * (sequencerState.tempoTicks)) / 1024) + 1;
}

// Method sets the step sequencers tempo according to the given value of a poti (value from 0-1023)
void set_tempo(uint16_t potiValue)
{
    int8_t timerTicks = 1024 / (maxTimeTicks - 1);
    sequencerState.tempoTicks = maxTimeTicks - potiValue / timerTicks;
}

int main()
{
    init_led_ports_mcu();
    init_button_ports_mcu();
    init_poti_adc();
    init_midi(CH1);
    init_base_timer();

    uint16_t resultPotiFrquency;
    uint16_t resultPotiVelocity;
    uint16_t resultPotiNoteLength;
    uint16_t resultPotiTempo;

    while (1)
    {
        if (button_pressed(BUTTON_START_STOP, 0) && sequencerState.startStopTriggeredReleaseCounter < 1)
        {
            start_stop_triggered();
            sequencerState.startStopTriggeredReleaseCounter = 6;
        }

        if (button_pressed(BUTTON_1, 1) && sequencerState.isRunning && sequencerState.button1TriggeredReleaseCounter < 1)
        {
            // Flip isActivate from 1 to 0 or from 0 to 1;
            note1.isActivated ^= 1;
            toggle_LED(LED_1);
            sequencerState.button1TriggeredReleaseCounter = 6;
        }

        if (button_pressed(BUTTON_2, 1) && sequencerState.isRunning && sequencerState.button2TriggeredReleaseCounter < 1)
        {
            note2.isActivated ^= 1;
            toggle_LED(LED_2);
            sequencerState.button2TriggeredReleaseCounter = 6;
        }

        if (button_pressed(BUTTON_3, 1) && sequencerState.isRunning && sequencerState.button3TriggeredReleaseCounter < 1)
        {
            note3.isActivated ^= 1;
            toggle_LED(LED_3);
            sequencerState.button3TriggeredReleaseCounter = 6;
        }

        if (button_pressed(BUTTON_4, 1) && sequencerState.isRunning && sequencerState.button4TriggeredReleaseCounter < 1)
        {
            note4.isActivated ^= 1;
            toggle_LED(LED_4);
            sequencerState.button4TriggeredReleaseCounter = 6;
        }

        resultPotiFrquency = get_poti_average(POTI_FREQUENCY);
        set_notes(resultPotiFrquency);
        resultPotiVelocity = get_poti_average(POTI_VELOCITY);
        set_velocity(resultPotiVelocity);
        resultPotiNoteLength = get_poti_average(POTI_NOTE_LENGTH);
        set_note_length(resultPotiNoteLength);
        resultPotiTempo = get_poti_average(POTI_TEMPO);
        set_tempo(resultPotiTempo);

        if (sequencerState.isRunning)
        {
            // Check if we have to send note off message.
            if (note1.noteOffCounter == 0 && !note1.noteOffSent)
            {
                send_midi_note_off(&note1.lastSentMidiNote);
                note1.noteOffSent = 1;
            }
            if (note2.noteOffCounter == 0 && !note2.noteOffSent)
            {
                send_midi_note_off(&note2.lastSentMidiNote);
                note2.noteOffSent = 1;
            }
            if (note3.noteOffCounter == 0 && !note3.noteOffSent)
            {
                send_midi_note_off(&note3.lastSentMidiNote);
                note3.noteOffSent = 1;
            }
            if (note4.noteOffCounter == 0 && !note4.noteOffSent)
            {
                send_midi_note_off(&note4.lastSentMidiNote);
                note4.noteOffSent = 1;
            }
            if (sequencerState.tempoTicksCounter == 0 && !sequencerState.currentTickUsedNoteOn)
            {
                if (sequencerNotes[currentNoteIndex]->isActivated)
                {
                    // Set the current note as last note
                    sequencerNotes[currentNoteIndex]->lastSentMidiNote = sequencerNotes[currentNoteIndex]->midiNote;
                    // Send the current note via midi
                    send_midi_note_on(&(sequencerNotes[currentNoteIndex]->lastSentMidiNote));
                    // Set noteOffCounter for that note
                    sequencerNotes[currentNoteIndex]->noteOffCounter = sequencerState.noteLengthTicks;
                    // Set noteOffSent flag
                    sequencerNotes[currentNoteIndex]->noteOffSent = 0;
                }
                // Mark that the current tick is allready used to send a note on via midi
                sequencerState.currentTickUsedNoteOn = 1;
                // Update currentNoteIndex to the next note
                currentNoteIndex++;
                currentNoteIndex %= (sizeof sequencerNotes / sizeof sequencerNotes[0]);
            }
        }
    }
}
