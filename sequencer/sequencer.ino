const byte clock_tick      = 248;
const byte clock_start     = 250;
const byte clock_stop      = 252;
const byte clock_continue  = 251;
const int clock_ppqn       = 6;
const byte note_on         = 144;
const byte note_off        = 128;
const int board_led        = 13;
const int midi_baud        = 31250;
const int button_1         = 2;
const int button_2         = 3;
const int button_3         = 4;
const int midi_channel     = 0; // todo: make it configurable
const int midi_channel_bis = 1;

int midi_clock_tick_counter = 0;
int sequence_step_counter = 0;
int stopped = 1;
int sequence_length = 8;
int split_note = 64;

int notes[16] = {80, 24, 57, 30, 36, 24, 57, 30, 36, 24, 57, 30, 36, 24, 57, 48};
int velocities[16] = {64, 64, 64, 64, 64, 127, 64, 64, 64, 64, 127, 64, 64, 127, 64, 64};
int slides[16] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};

void noteOn(int channel, int pitch, int velocity) {
    Serial.write(144 + channel);
    Serial.write(pitch);
    Serial.write(velocity);
}

void reset_step_counter(){
    sequence_step_counter = 0;
}

void reset_tick_counter(){
    midi_clock_tick_counter = 0;
}

void all_notes_off(int channel){
    Serial.write(176 + channel);
    Serial.write(123);
    Serial.write(0);
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
    sequence_step_counter++;
    if(sequence_step_counter >= sequence_length){
        sequence_step_counter = 0;
    }
}

void set_sequence_length(){
    sequence_length = map(analogRead(1), 0, 1023, 1, 16);
}

void set_split_note(){
    split_note = map(analogRead(0), 0, 1023, 24, 90);
}

void randomize_step(int step_number){
    notes[step_number] = random(24, 90);
    slides[step_number] = random(1);
    velocities[step_number] = random(64, 127);
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
    shift_array_left(slides);
}

void shift_sequence_right(){
    shift_array_right(notes);
    shift_array_right(velocities);
    shift_array_right(slides);
}

void setup(){
    pinMode(button_1,INPUT);
    pinMode(button_2,INPUT);
    pinMode(button_3,INPUT);
    digitalWrite(button_1, HIGH);
    digitalWrite(button_2, HIGH);
    digitalWrite(button_3, HIGH);
    Serial.begin(midi_baud);
    reset_sequencer();
    for(int i=0;i<16;i++){
        randomize_step(i);
    }
}

void loop(){
    if(Serial.available() > 0){
        byte midi_byte = Serial.read();
        if(midi_byte == clock_stop){
            Serial.write(midi_byte);
            reset_sequencer();
        }
        if(midi_byte == clock_start || midi_byte == clock_continue){
            Serial.write(midi_byte);
            stopped = 0;
        }
        if(midi_byte == clock_tick){
            Serial.write(midi_byte);
            if(!stopped){
                if(midi_clock_tick_counter == 0){
                    set_sequence_length();
                    set_split_note();
                    if(digitalRead(button_1) == LOW){
                        shift_sequence_left();
                    }
                    if(digitalRead(button_3) == LOW){
                        shift_sequence_right();
                    }
                    if(digitalRead(button_2) == LOW){
                        randomize_step(sequence_step_counter);
                    }
                    if(!slides[sequence_step_counter]){
                        all_notes_off(midi_channel);
                        all_notes_off(midi_channel_bis);
                    }
                    if(notes[sequence_step_counter] > split_note){
                        noteOn(midi_channel_bis, notes[sequence_step_counter], velocities[sequence_step_counter]);
                    }else{
                        noteOn(midi_channel, notes[sequence_step_counter], velocities[sequence_step_counter]);
                    }
                    step_sequence();
                }
                step_tick();
            }
        }
    }
}
