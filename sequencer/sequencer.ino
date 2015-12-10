const byte clock_tick      = 248;
const byte clock_start     = 250;
const byte clock_stop      = 252;
const byte clock_continue  = 251;
const byte note_on         = 144;
const byte note_off        = 128;
const int board_led        = 13;
const int midi_baud        = 31250;
const int midi_channel     = 0;
const int midi_channel_bis = 1;

const int hi_velocity      = 127;
const int lo_velocity      = 64;

int lo_note                 = 24;
int hi_note                 = 84;
int split_note              = 64;
int midi_clock_tick_counter = 0;
int sequence_step_counter   = 0;
int stopped                 = 1;
int clock_ppqn              = 6;
int sequence_length         = 8;
int selected_scale          = 0;

int notes[16] = {80, 24, 57, 30, 36, 24, 57, 30, 36, 24, 57, 30, 36, 24, 57, 48};
int velocities[16] = {64, 64, 64, 64, 64, 127, 64, 64, 64, 64, 127, 64, 64, 127, 64, 64};
// 0 = note, 1 = tie, 2 = slide, 3 = rest
int step_type[16] = {0, 0, 2, 0, 3, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};

// C-based
int harmonic_scale[128] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127};
int major_scale[128] = {0,0,2,3,3,5,5,7,7,8,10,10,12,12,14,15,15,17,17,19,19,20,22,22,24,24,26,27,27,29,29,31,31,32,34,34,36,36,38,39,39,41,41,43,43,44,46,46,48,48,50,51,51,53,53,55,55,56,58,58,60,60,62,63,63,65,65,67,67,68,70,70,72,72,74,75,75,77,77,79,79,80,82,82,84,84,86,87,87,89,89,91,91,92,94,94,96,96,98,99,99,101,101,103,103,104,106,106,108,108,110,111,111,113,113,115,115,116,118,118,120,120,122,123,123,125,125,127};
int minor_scale[128] = {0,0,2,2,4,5,5,7,7,9,9,11,12,12,14,14,16,17,17,19,19,21,21,23,24,24,26,26,28,29,29,31,31,33,33,35,36,36,38,38,40,41,41,43,43,45,45,47,48,48,50,50,52,53,53,55,55,57,57,59,60,60,62,62,64,65,65,67,67,69,69,71,72,72,74,74,76,77,77,79,79,81,81,83,84,84,86,86,88,89,89,91,91,93,93,95,96,96,98,98,100,101,101,103,103,105,105,107,108,108,110,110,112,113,113,115,115,117,117,119,120,120,122,122,124,125,125,127};
int hirajoshi_scale[128] = {0,0,2,3,3,3,7,7,8,8,8,12,12,12,14,15,15,15,19,19,20,20,20,24,24,24,26,27,27,27,31,31,32,32,32,36,36,36,38,39,39,39,43,43,44,44,44,48,48,48,50,51,51,51,55,55,56,56,56,60,60,60,62,63,63,63,67,67,68,68,68,72,72,72,74,75,75,75,79,79,80,80,80,84,84,84,86,87,87,87,91,91,92,92,92,96,96,96,98,99,99,99,103,103,104,104,104,108,108,108,110,111,111,111,115,115,116,116,116,120,120,120,122,123,123,123,127,127};
int hiwato_scale[128] = {0,1,1,1,5,5,6,6,6,10,10,10,12,13,13,13,17,17,18,18,18,22,22,22,24,25,25,25,29,29,30,30,30,34,34,34,36,37,37,37,41,41,42,42,42,46,46,46,48,49,49,49,53,53,54,54,54,58,58,58,60,61,61,61,65,65,66,66,66,70,70,70,72,73,73,73,77,77,78,78,78,82,82,82,84,85,85,85,89,89,90,90,90,94,94,94,96,97,97,97,101,101,102,102,102,106,106,106,108,109,109,109,113,113,114,114,114,118,118,118,120,121,121,121,125,125,126,126};

void noteOn(int pitch, int velocity) {
    int pitch_in_range = map(pitch, 0, 127, lo_note, hi_note);
    if(pitch_in_range > split_note){
        Serial3.write(144 + midi_channel_bis);
    }else{
        Serial3.write(144 + midi_channel);
    }
    switch(selected_scale){
        case 1: 
            Serial3.write(major_scale[pitch_in_range]);
            break;
        case 2:
            Serial3.write(minor_scale[pitch_in_range]);
            break;
        case 3:
            Serial3.write(hirajoshi_scale[pitch_in_range]);
            break;            
        case 4:
            Serial3.write(hiwato_scale[pitch_in_range]);
            break;                        
        default:
            Serial3.write(pitch_in_range);      
            break;
    }
    Serial3.write(velocity);
}

void reset_step_counter(){
    sequence_step_counter = 0;
}

void reset_tick_counter(){
    midi_clock_tick_counter = 0;
}

void all_notes_off(int channel){
    Serial3.write(176 + channel);
    Serial3.write(123);
    Serial3.write(0);
}

void reset_sequencer(){
    reset_step_counter();
    reset_tick_counter();
    for(int i=0;i<16;i++){
        all_notes_off(i);
    }
    stopped = 1;
}

void step_tick(){
    midi_clock_tick_counter++;
    if(midi_clock_tick_counter == clock_ppqn){
        midi_clock_tick_counter = 0;
    }
}



void step_sequence(){
    // 0 = note, 1 = tie, 2 = slide, 3 = rest
    switch(step_type[sequence_step_counter]){
        case 0:
            // stop previous notes, play note
            all_notes_off(midi_channel);
            all_notes_off(midi_channel_bis);
            noteOn(notes[sequence_step_counter], velocities[sequence_step_counter]);
            break;
        case 1:
            // do nothing
            break;
        case 2:
            // don't stop previous notes, play note
            noteOn(notes[sequence_step_counter], velocities[sequence_step_counter]);
            break;
        case 3:
            // stop notes
            all_notes_off(midi_channel);
            all_notes_off(midi_channel_bis);
            break;
    }
    sequence_step_counter++;
    if(sequence_step_counter >= sequence_length){
        sequence_step_counter = 0;
    }
}

void gather_settings(){
    lo_note = map(analogRead(0), 0, 1023, 0, 127);
    hi_note = map(analogRead(1), 0, 1023, 0, 127);
    split_note = map(analogRead(2), 0, 1023, 0, 127);
    selected_scale = map(analogRead(3), 0, 1023, 0, 4);
    sequence_length = map(analogRead(4), 0, 1023, 1, 16);
    int clock_divide = map(analogRead(5), 0, 1023, 0, 3);
    switch(clock_divide){
        case 0:
            clock_ppqn = 24;
            break;
        case 1:
            clock_ppqn = 12;
            break;
        case 2:
            clock_ppqn = 6;
            break;
        case 3:
            clock_ppqn = 3;
            break;
        default:
            clock_ppqn = 6;
    }
}

void randomize_step(int step_number){
    notes[step_number] = random(lo_note, hi_note +1);
    velocities[step_number] = random(lo_velocity, hi_velocity + 1);
    step_type[step_number] = random(4);
}

void shift_array_left(int a[16]){
    int temp_val = a[0];
    for(int i=1;i<15;i++){
        a[i-1] = a[i]; 
    }
    a[15] = temp_val;
}

void shift_array_right(int a[16]){
    int temp_val = a[15];
    for(int i=15;i>0;i--){
        a[i] = a[i-1];
    }
    a[0] = temp_val;
}

void shift_sequence_left(){
    shift_array_left(notes);
    shift_array_left(velocities);
    shift_array_left(step_type);
}

void shift_sequence_right(){
    shift_array_right(notes);
    shift_array_right(velocities);
    shift_array_right(step_type);
}

void setup(){
    Serial3.begin(midi_baud);
    randomSeed(analogRead(0));
    reset_sequencer();
    for(int i=0;i<16;i++){
        randomize_step(i);
    }
}

void loop(){
    if(Serial3.available() > 0){
        byte midi_byte = Serial3.read();
        if(midi_byte == clock_stop){
            Serial3.write(midi_byte);
            reset_sequencer();
        }
        if(midi_byte == clock_start || midi_byte == clock_continue){
            Serial3.write(midi_byte);
            stopped = 0;
        }
        if(midi_byte == clock_tick){
            Serial3.write(midi_byte);
            if(!stopped){
                if(midi_clock_tick_counter == 0){
                    gather_settings();
                    if(map(analogRead(7), 0, 1023, 0, 1) == 1){
                        randomize_step(sequence_step_counter);
                    }
                    step_sequence();
                }
                step_tick();
            }
        }
    }
}
