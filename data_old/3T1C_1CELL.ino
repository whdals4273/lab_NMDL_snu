/*
- Optimized for single cell measurement without selection

by "Minseung Kang"
*/


/*  PIN_MAP_INFO

    int DFF1 = 31; //PA7 = D
    int DS = 42; //PA19
    int VH = 43; //PA20

    int FB = 22; //PB26
    int CR = 52; //PB21
    int CON = 53; //PB14
    int HL_CHOP = 2; //PB25

    int WL_0 = 25; //PD0
    int WL_1 = 26; //PD1
    int WL_2 = 27; //PD2
    int WL_3 = 28; //PD3
    int WL_4 = 29; //PD6

    int N1_0 = 51; //PC12
    int N1_1 = 50; //PC13
    int N1_2 = 49; //PC14
    int N1_3 = 48; //PC15
    int N1_4 = 47; //PC16

    int N3_0 = 46; //PC17
    int N3_1 = 45; //PC18
    int N3_2 = 44; //PC19
    int N3_3 = 41; //PC9
    int N3_4 = 40; //PC8

    int N2_0 = 39; //PC7
    int N2_1 = 38; //PC6
    int N2_2 = 37; //PC5
    int N2_3 = 36; //PC4
    int N2_4 = 35; //PC3

    int N4_0 = 34; //PC2
    int N4_1 = 33; //PC1
    int N4_2 = 7; //PC23
    int N4_3 = 6; //PC24
    int N4_4 = 5; //PC25

    int ADC_0 = A0;
    int ADC_1 = A1;
    int ADC_2 = A2;
    int ADC_3 = A3;
    int ADC_4 = A4;
*/

#include <ctype.h>
#include <stdlib.h>
#include <math.h>

const int VAR_NUM           = 18;   // Number of total serial input variables

const int WL_READ_PINLIST[] = {1      , 1 <<  1, 1 <<  2, 1 <<  3, 1 <<  6};
const int N1_PINLIST[]      = {1 << 12, 1 << 13, 1 << 14, 1 << 15, 1 << 16};
const int N2_PINLIST[]      = {1 <<  7, 1 <<  6, 1 <<  5, 1 <<  4, 1 <<  3};
const int N3_PINLIST[]      = {1 << 17, 1 << 18, 1 << 19, 1 <<  9, 1 <<  8};
const int N4_PINLIST[]      = {1 <<  2, 1 <<  1, 1 << 23, 1 << 24, 1 << 25};

void setup() {
    pinMode(53, OUTPUT);
    pinMode(52, OUTPUT);
    pinMode(51, OUTPUT);
    pinMode(50, OUTPUT);
    pinMode(49, OUTPUT);
    pinMode(48, OUTPUT);
    pinMode(47, OUTPUT);
    pinMode(46, OUTPUT);
    pinMode(45, OUTPUT);
    pinMode(44, OUTPUT);
    pinMode(43, OUTPUT);
    pinMode(42, OUTPUT);
    pinMode(41, OUTPUT);
    pinMode(40, OUTPUT);
    pinMode(39, OUTPUT);
    pinMode(38, OUTPUT);
    pinMode(37, OUTPUT);
    pinMode(36, OUTPUT);
    pinMode(35, OUTPUT);
    pinMode(34, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(31, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(22, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    ADC->ADC_MR     |= 0x80;    // Mode FREERUN
    ADC->ADC_CR     = 2;        // Start converter
    ADC->ADC_CHER   = 0xF8;     // Enabling All channels
    Serial.begin(115200);

    //randomSeed(millis()); // Use the time elapsed since the Arduino was powered on to set the seed for the random number generator
}

void loop() {
    if (Serial.available() > 0) {
        String Input_string = Serial.readString();
        /*Input Serial Format: (total 18 parameters)
        (scenario),(half_select),
        (set_num),(update_num),
        (pre_en),(pulse_width),(post_en),(zero_time),
        (read_time),(read_period),(read_set_time),(read_delay),
        (extra1),(extra2),(extra3),(extra4),(extra5),(extra6)
        Example input: PD,F,10,200,1,1,1,1,10,10,1,1,1000,10,20,3,0,0
        */
        String scenario, half_select;
        String set_num_str, update_num_str;
        String pre_en_str, pulse_width_str, post_en_str, zero_time_str;
        String read_time_str, read_period_str, read_set_time_str, read_delay_str;
        //String remainder_str;
        String extra1_str, extra2_str, extra3_str, extra4_str, extra5_str, extra6_str;
        
        int cycle_num       = 1;
        int minutes         = 60000;
        int index[VAR_NUM]  = {0, };
        /*==================================================================
        Input serial to variables
        ==================================================================*/ 
        index[0] = Input_string.indexOf(","); // find commas
        for (int i = 0; i < VAR_NUM - 1; i++) {
            index[i + 1] = Input_string.indexOf(",", index[i] + 1);
        }

        scenario            = Input_string.substring(0, index[0]);
        half_select         = Input_string.substring(index[0] + 1, index[1]);
        scenario.toUpperCase();
        half_select.toUpperCase();
        bool select_bool    = false;
        if (half_select == "T") {
            select_bool     = true;
        }

        set_num_str         = Input_string.substring(index[1] + 1, index[2]);
        update_num_str      = Input_string.substring(index[2] + 1, index[3]);

        pre_en_str          = Input_string.substring(index[3] + 1, index[4]);
        pulse_width_str     = Input_string.substring(index[4] + 1, index[5]);
        post_en_str         = Input_string.substring(index[5] + 1, index[6]);
        zero_time_str       = Input_string.substring(index[6] + 1, index[7]);

        read_time_str       = Input_string.substring(index[7] + 1, index[8]);
        read_period_str     = Input_string.substring(index[8] + 1, index[9]);
        read_set_time_str   = Input_string.substring(index[9] + 1, index[10]);
        read_delay_str      = Input_string.substring(index[10] + 1, index[11]);

        extra1_str          = Input_string.substring(index[11] + 1, index[12]);
        extra2_str          = Input_string.substring(index[12] + 1, index[13]);
        extra3_str          = Input_string.substring(index[13] + 1, index[14]);
        extra4_str          = Input_string.substring(index[14] + 1, index[15]);
        extra5_str          = Input_string.substring(index[15] + 1, index[16]);
        extra6_str          = Input_string.substring(index[16] + 1, index[17]);

        int set_num         = set_num_str.toInt();
        int update_num      = update_num_str.toInt();

        int pre_en          = pre_en_str.toInt();
        int pulse_width     = pulse_width_str.toInt();
        int post_en         = post_en_str.toInt();
        int zero_time       = zero_time_str.toInt();

        int read_time       = read_time_str.toInt();
        int read_period     = read_period_str.toInt();
        int read_set_time   = read_set_time_str.toInt();
        int read_delay      = read_delay_str.toInt();

        double extra1       = extra1_str.toDouble();
        double extra2       = extra2_str.toDouble();
        double extra3       = extra3_str.toDouble();
        double extra4       = extra4_str.toDouble();
        double extra5       = extra5_str.toDouble();
        double extra6       = extra6_str.toDouble();

////////////////////////////////////////////////////////////////////////////////
        if (select_bool) {  // bitcell operation with selection scheme
            int boost_bool  = false;
            // turn capacitor bottom electrode voltage low or high during read operation
            if (int(extra6)) {
                boost_bool  = true;
            }

            if (scenario == "P") {          // potentiation only
            /*
                Only performs potentiation of (update_num) steps
                Ignores given set number (only one set)
            */

                // read initial weight
                char strBuf_init[50] = {'\0'};
                int tempADC     = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                sprintf(strBuf_init, "0, %d", tempADC);
                Serial.println(strBuf_init);

                // start potentiation loop
                for (int i = 0; i < update_num; i++) {
                    potentiation_sel(pre_en, pulse_width, post_en, zero_time);
                    if ((i + 1) % read_period == 0) {
                        tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                        char strBuf[50] = {'\0'};
                        sprintf(strBuf, "%d, %d", i + 1, tempADC);
                        Serial.println(strBuf);
                    }
                }

                delay(50);
                Serial.println("EOD");
            }
            else if (scenario == "D") {         // depression only
            /*
                Only performs potentiation of (update_num) steps
                Ignores given set number (only one set)
            */
                // read initial weight
                char strBuf_init[50] = {'\0'};
                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                sprintf(strBuf_init, "0, %d", tempADC);
                Serial.println(strBuf_init);

                // start depression loop
                for (int i = 0; i < update_num; i++) {
                    depression_sel(pre_en, pulse_width, post_en, zero_time);
                    if ((i + 1) % read_period == 0) {
                        tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                        char strBuf[50] = {'\0'};
                        sprintf(strBuf, "%d, %d", i + 1, tempADC);
                        Serial.println(strBuf);
                    }
                }

                delay(50);
                Serial.println("EOD");
            }
            else if (scenario == "GPD") {       // gnd-potentiation-depression
            /*
                Potentiation-Depression for (set) times
                First start with connecting storage cap to GND
            */

                // GND storage capacitor
                gnd_all_nosel(50);
                // read initial weight
                char strBuf_init[50] = {'\0'};
                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                sprintf(strBuf_init, "0, %d", tempADC);
                Serial.println(strBuf_init);

                // Start P-D loop
                for (int i = 0; i < set_num; i++) {
                    // potentiation x(update_num) times
                    for (int j = 0; j < update_num; j++) {
                        potentiation_sel(pre_en, pulse_width, post_en, zero_time);
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                            char strBuf[50] = {'\0'};
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    // depression x(update_num) times
                    for (int j = 0; j < update_num; j++) {
                        depression_sel(pre_en, pulse_width, post_en, zero_time);
                        if ((j + 1) % read_period == 0) {
                            tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                            char strBuf[50] = {'\0'};
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }

                delay(50);
                Serial.println("EOD");
            }
            else if (scenario == "GPDF") {       // gnd-potentiation-depression (short pulse)
            /*
                Potentiation-Depression for (set) times with short pulses ~200 ns
                First start with connecting storage cap to GND
            */

                // GND storage capacitor
                gnd_all_nosel(50);
                // read initial weight
                char strBuf_init[50] = {'\0'};
                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                sprintf(strBuf_init, "0, %d", tempADC);
                Serial.println(strBuf_init);

                // Start P-D loop
                for (int i = 0; i < set_num; i++) {
                    // potentiation x(update_num) times
                    for (int j = 0; j < update_num; j++) {
                        potentiation_sel_fast();
                        if ((j + 1) % read_period == 0) {
                            tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                            char strBuf[50] = {'\0'};
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    // depression x(update_num) times
                    for (int j = 0; j < update_num; j++) {
                        depression_sel_fast();
                        if ((j + 1) % read_period == 0) {
                            tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                            char strBuf[50] = {'\0'};
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }

                delay(50);
                Serial.println("EOD");                
            }
            else if (scenario == "HS") {
                /*
                    Half select test
                    extra1: mode select
                            0: all test
                            1: selected cell (WWL and WBL signal)
                            2: WWL signal only
                            3: WBL signal only
                    extra2: potentiation pulse width
                    extra3: depression pulse width
                    extra4: potentiation step number
                    extra5: depression step number
                */
                // GND weight
                gnd_all_nosel(50);
                // read initial weight
                char strBuf_init[50] = {'\0'};
                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                sprintf(strBuf_init, "0, %d", tempADC);
                Serial.println(strBuf_init);

                if (int(extra1) == 1 || int(extra1) == 0) {
                    // phase 1: WWL and WBL signal coinciding
                    // Start P-D loop
                    for (int i = 0; i < set_num; i++) {
                        // potentiation x(extra4) times
                        for (int j = 0; j < int(extra4); j++) {
                            potentiation_sel(pre_en, int(extra2), post_en, zero_time);
                            if ((j + 1) % read_period == 0) {
                                tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                char strBuf[50] = {'\0'};
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        // depression x(extra5) times
                        for (int j = 0; j < int(extra5); j++) {
                            depression_sel(pre_en, int(extra3), post_en, zero_time); //extra3
                            if ((j + 1) % read_period == 0) {
                                tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                char strBuf[50] = {'\0'};
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                }
                gnd_all_nosel(50);
                if (int(extra1) == 2 || int(extra1) == 0) {
                    // phase 2: WWL signal only
                    // Start P-D loop (WWL only)
                    for (int i = 0; i < set_num; i++) {
                        // potentiation x(extra4) times
                        for (int j = 0; j < int(extra4); j++) {
                            PIOC->PIO_SODR  = N1_PINLIST[0];
                            delayMicroseconds(int(extra2));
                            PIOC->PIO_CODR  = N1_PINLIST[0];
                            delayMicroseconds(zero_time);

                            if ((j + 1) % read_period == 0) {
                                tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                char strBuf[50] = {'\0'};
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        // depression x(extra5) times
                        for (int j = 0; j < int(extra5); j++) {
                            PIOC->PIO_SODR  = N2_PINLIST[0];
                            delayMicroseconds(int(extra3));
                            PIOC->PIO_CODR  = N2_PINLIST[0];
                            delayMicroseconds(zero_time);

                            if ((j + 1) % read_period == 0) {
                                tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                char strBuf[50] = {'\0'};
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                }
                gnd_all_nosel(50);
                if (int(extra1) == 3 || int(extra1) == 0) {
                    // phase 3: WBL signal only
                    // Start P-D loop (WBL only)
                    for (int i = 0; i < set_num; i++) {
                        // potentiation x(extra4) times
                        for (int j = 0; j < int(extra4); j++) {
                            PIOC->PIO_SODR  = N4_PINLIST[0];
                            delayMicroseconds(pre_en);
                            delayMicroseconds(int(extra2));
                            delayMicroseconds(post_en);
                            PIOC->PIO_CODR  = N4_PINLIST[0];
                            delayMicroseconds(zero_time);

                            if ((j + 1) % read_period == 0) {
                                tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                char strBuf[50] = {'\0'};
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        // depression x(extra5) times
                        for (int j = 0; j < int(extra5); j++) {
                            PIOC->PIO_SODR  = N4_PINLIST[0];
                            delayMicroseconds(pre_en);
                            delayMicroseconds(int(extra2));
                            delayMicroseconds(post_en);
                            PIOC->PIO_CODR  = N4_PINLIST[0];
                            delayMicroseconds(zero_time);
                            
                            if ((j + 1) % read_period == 0) {
                                tempADC = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                char strBuf[50] = {'\0'};
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                }

                delay(50);
                Serial.println("EOD");
            }
            else if (scenario == "DSTB") {
                /*
                    Weight disturbance test
                    extra1: mode select
                            1: WWLP signal disturb
                            2: WWLD signal disturb
                            3: WBL signal disturb
                    extra2: number of disturb signals between read operation
                            *read disturb should be tested with retention scenario
                    extra3: number of potentiation pulses for initial weight set
                    extra4: pulse width of disturb signal
                */
                // GND weight
                gnd_all_nosel(50);
                // read initial weight
                char strBuf_init[50] = {'\0'};
                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                sprintf(strBuf_init, "0, %d", tempADC);
                Serial.println(strBuf_init);

                // set starting weight
                for (int i = 0; i < int(extra3); i++) {
                    potentiation_sel(pre_en, pulse_width, post_en, zero_time);
                    if ((i + 1) % read_period == 0) {
                        char strBuf[50] = {'\0'};
                        int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                        sprintf(strBuf, "0, %d", tempADC);
                        Serial.println(strBuf);
                    }
                }
                char strBuf_start[50] = {'\0'};
                int tempADC_start     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                sprintf(strBuf_start, "0, %d", tempADC_start);
                Serial.println(strBuf_start);

                if (int(extra1) == 1) {
                    // WWLP disturb test
                    for (int i = 0; i < set_num; i++) {
                        for (int j = 0; j < int(extra2); j++) {
                            PIOC->PIO_SODR  = N1_PINLIST[0];
                            delayMicroseconds(int(extra4));
                            PIOC->PIO_CODR  = N1_PINLIST[0];
                            delayMicroseconds(zero_time);
                            if ((j + 1) % read_period == 0) {
                                char strBuf[50] = {'\0'};
                                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }                        
                    }
                }
                else if (int(extra1) == 2) {
                    // WWLD disturb test
                    for (int i = 0; i < set_num; i++) {
                        for (int j = 0; j < int(extra2); j++) {
                            PIOC->PIO_SODR  = N2_PINLIST[0];
                            delayMicroseconds(int(extra4));
                            PIOC->PIO_CODR  = N2_PINLIST[0];
                            delayMicroseconds(zero_time);
                            if ((j + 1) % read_period == 0) {
                                char strBuf[50] = {'\0'};
                                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                }
                else if (int(extra1) == 3) {
                    // WBL disturb test
                    for (int i = 0; i < set_num; i++) {
                        for (int j = 0; j < int(extra2); j++) {
                            PIOC->PIO_SODR  = N4_PINLIST[0];
                            delayMicroseconds(int(extra4));
                            PIOC->PIO_CODR  = N4_PINLIST[0];
                            delayMicroseconds(zero_time);
                            if ((j + 1) % read_period == 0) {
                                char strBuf[50] = {'\0'};
                                int tempADC     = tempADC = readADC_sel(read_time, read_set_time, read_delay, boost_bool);
                                sprintf(strBuf, "%d, %d", i + 1, tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                }
                
                delay(50);
                Serial.println("EOD");
            }
            else {
                gnd_all_nosel(50);
                Serial.println("Default Scenario");
            }
        }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
        else {              // bitcell operation without selection scheme
            if (scenario == "P") {          // potentiation only
            /*
                Only performs potentiation of (update_num) steps
                Ignores given set number (only one set)
            */
                // set VBE (if connected) to GND
                PIOC->PIO_SODR  = N4_PINLIST[0];        
                delayMicroseconds(10);
                // read initial weight
                char strBuf_zero[50];
                int tempADC     = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_zero, "0, %d", tempADC);
                Serial.println(strBuf_zero);

                for (int i = 0; i < update_num; i++) {  // potentiation loop
                    potentiation_nosel(pulse_width, zero_time);
                    if ((i + 1) % read_period == 0) {   // read if remainder zero
                        tempADC = readADC(read_time, read_set_time, read_delay);
                        char strBuf[50];
                        sprintf(strBuf, "%d, %d", i + 1, tempADC);
                        Serial.println(strBuf);
                    }
                }

                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "D") {     // depression only
            /*
                Only performs depression of (update_num) steps
                Ignores given set number (only one set)
            */
                // set VBE (if connected) to GND
                PIOC->PIO_SODR  = N4_PINLIST[0];        
                delayMicroseconds(10);
                // read initial weight
                char strBuf_zero[50];
                int tempADC     = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_zero, "0, %d", tempADC);
                Serial.println(strBuf_zero);

                for (int i = 0; i < update_num; i++) {  // potentiation loop
                    depression_nosel(pulse_width, zero_time);
                    if ((i + 1) % read_period == 0) {   // read if remainder zero
                        tempADC = readADC(read_time, read_set_time, read_delay);
                        char strBuf[50];
                        sprintf(strBuf, "%d, %d", i + 1, tempADC);
                        Serial.println(strBuf);
                    }
                }
                
                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "PD") {    // potentiation-depression
            /*
                Potentiation-Depression for (set) times
            */
                // set VBE (if connected) to GND
                PIOC->PIO_SODR  = N4_PINLIST[0];        
                delayMicroseconds(10);
                // read initial weight
                char strBuf_zero[50];
                int tempADC     = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_zero, "0, %d", tempADC);
                Serial.println(strBuf_zero);

                for (int i = 0; i < set_num; i++) {
                    // Potentiation x(update_num)
                    for (int j = 0; j < update_num; j++) {
                        potentiation_nosel(pulse_width, zero_time);
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    // Depression x(update_num)
                    for (int j = 0; j < update_num; j++) {
                        depression_nosel(pulse_width, zero_time);
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }
                
                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "GPD") {   // gnd-potentiation-depression
            /*
                Potentiation-Depression for (set) times
                First start with connecting storage cap to GND
            */
                // GND weight
                gnd_all_nosel(20);
                // set VBE (if connected) to GND
                PIOC->PIO_SODR  = N4_PINLIST[0];        
                delayMicroseconds(10);
                // read initial weight
                char strBuf_zero[50];
                int tempADC     = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_zero, "0, %d", tempADC);
                Serial.println(strBuf_zero);

                for (int i = 0; i < set_num; i++) {
                    // Potentiation x(update_num)
                    for (int j = 0; j < update_num; j++) {
                        potentiation_nosel(pulse_width, zero_time);
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    // Depression x(update_num)
                    for (int j = 0; j < update_num; j++) {
                        depression_nosel(pulse_width, zero_time);
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }
                
                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "GPDF") {  // gnd-potentiation-depression (short pulse)
            /*
                Potentiation-Depression for (set) times
                Utilizes fast pulse (~100 ns)
                First start with connecting storage cap to GND
            */
                // GND weight
                gnd_all_nosel(20);
                // set VBE (if connected) to GND
                PIOC->PIO_SODR  = N4_PINLIST[0];        
                delayMicroseconds(10);
                // read initial weight
                char strBuf_zero[50];
                int tempADC     = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_zero, "0, %d", tempADC);
                Serial.println(strBuf_zero);

                for (int i = 0; i < set_num; i++) {
                    // Potentiation x(update_num)
                    for (int j = 0; j < update_num; j++) {
                        potentiation_fast_nosel();
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    // Depression x(update_num)
                    for (int j = 0; j < update_num; j++) {
                        depression_fast_nosel();
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }
                
                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "RET") {   // retention test
                /*
                    Retention test: initial charging -> periodic reading
                    extra1: number of pulses for initial capacitor charging
                    extra2: number of sampling timings
                    extra3: interval between sampling timings (in minutes)
                    extra4: number of sampling at a single timing
                */

                // GND weight
                gnd_all_nosel(20);
                // set VBE (if connected) to GND
                PIOC->PIO_SODR   = N4_PINLIST[0];
                delayMicroseconds(10);
                // read initial weight
                char strBuf_zero[50];
                int tempADC     = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_zero, "0, %d", tempADC);
                Serial.println(strBuf_zero);   
                
                // Initial storage capacitor charging
                for (int i = 0; i < int(extra1); i++) {
                    potentiation_nosel(pulse_width, zero_time);   // extra1
                }
                char strBuf_high[50];
                tempADC         = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_high, "0, %d", tempADC);
                Serial.println(strBuf_high); 
                
                // Periodic reading
                for (int i = 0; i < int(extra2); i++) {
                    // read every (extra3) minutes
                    // read (extra4) times every sample timing
                    delay(minutes * extra3);
                    for (int j = 0; j < int(extra4); j++) {
                        tempADC = readADC(read_time, read_set_time, read_delay);
                        char strBuf[50];    // is it okay to keep re-stating strBuf?
                        sprintf(strBuf, "%d, %d", i + 1, tempADC);
                        Serial.println(strBuf);
                    }
                }
                
                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "PD_DIFF") {
                /*
                 * different P/D pulse number and widths (200, 300, 400, 500 ns)
                 * extra1: potentiation pulse width (2, 3, 4, or 5)
                 * extra2: depression pulse width (2, 3, 4, or 5)
                 * extra3: number of potentiation pulses
                 * extra4: number of depression pulses
                */
                // GND weight
                gnd_all_nosel(20);
                // set VBE (if connected) to GND
                PIOC->PIO_SODR  = N4_PINLIST[0];        
                delayMicroseconds(10);
                // read initial weight
                char strBuf_zero[50];
                int tempADC     = readADC(read_time, read_set_time, read_delay);
                sprintf(strBuf_zero, "0, %d", tempADC);
                Serial.println(strBuf_zero);

                for (int i = 0; i < set_num; i++) {
                    // Potentiation x(update_num)
                    for (int j = 0; j < int(extra3); j++) {
                        if (int(extra1) == 3) {
                            fast_pulse_300(true);
                        }
                        else if (int(extra1) == 4) {
                            fast_pulse_400(true);
                        }
                        else if (int(extra1) == 5) {
                            fast_pulse_500(true);
                        }
                        else {
                            fast_pulse_200(true);
                        }
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    // Depression x(update_num)
                    for (int j = 0; j < int(extra4); j++) {
                        if (int(extra2) == 3) {
                            fast_pulse_300(false);
                        }
                        else if (int(extra2) == 4) {
                            fast_pulse_400(false);
                        }
                        else if (int(extra2) == 5) {
                            fast_pulse_500(false);
                        }
                        else {
                            fast_pulse_200(false);
                        }
                        if ((j + 1) % read_period == 0) {
                            tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }
                
                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "CE") {   // cycling endurance test
                /*
                    Cycling endurance test (currently goes up to 1E10 cycles)
                    Add more if needed
                    extra1: number of total cycles in power of 10
                */
                // GND weight
                gnd_all_nosel(10);
                //////////////////////////////////////////////////////////
                if (extra1 > 0) {   // cycle 1 - 10
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "0, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "0, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 1) {   // cycle 11 - 100
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "1, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "1, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                    
                    for (int i = 0; i < 80; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 2) {   // cycle 101 - 1,000
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "2, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "2, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    for (int i = 0; i < 890; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 3) {   // cycle 1,001 - 10,000
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "3, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "3, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    for (int i = 0; i < 8990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 4) {   // cycle 10,001 - 100,000 (1E5)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "4, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "4, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    for (int i = 0; i < 89990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 5) {   // cycle 100,001 - 1,000,000 (1E6)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "5, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "5, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    for (int i = 0; i < 899990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 6) {   // cycle 1,000,001 - 5,000,000 (5E6)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 5E6 cycle");
                    for (int i = 0; i < 3999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 6) {   // cycle 5,000,001 - 10,000,000 (1E7)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                    
                    Serial.println("Checkpoint-going up to 1E7 cycle");
                    for (int i = 0; i < 4999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 7) {   // cycle 10,000,001 - 50,000,000 (5E7)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 5E7 cycle");
                    for (int i = 0; i < 39999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 7) {   // cycle 50,000,001 - 100,000,000 (1E8)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 1E8 cycle");
                    for (int i = 0; i < 49999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 8) {   //  cycle 100,000,001 - 500,000,000 (5E8)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 5E8 cycle");
                    for (int i = 0; i < 399999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 8) {   //  cycle 500,000,001 - 1,000,000,000 (1E9)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 1E9 cycle");
                    for (int i = 0; i < 499999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 9) {   //  cycle 1,000,000,001 - 5,000,000,000 (5E9)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 5E9 cycle");
                    for (int i = 0; i < 3999999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 9) {   //  cycle 5,000,000,001 - 10,000,000,000 (1E10)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_nosel(pulse_width, zero_time);
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                    
                    Serial.println("Checkpoint-going up to 1E10 cycle");
                    for (int i = 0; i < 4999999990; i++) {
                        potentiation_cycle_nosel(pulse_width, update_num);
                        depression_cycle_nosel(pulse_width, update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                //  add more if more cyclic stress is required
                //////////////////////////////////////////////////////////
                for (int i = 0; i < 10; i++) {  // Final read section after all cycle
                    for (int j = 0; j < update_num; j++) {
                        potentiation_nosel(pulse_width, zero_time);
                        if ((j + 1) % read_period == 0) {
                            int tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", int(extra1), tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    for (int j = 0; j < update_num; j++) {
                        depression_nosel(pulse_width, zero_time);
                        if ((j + 1) % read_period == 0) {
                            int tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", int(extra1), tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }
                delay(50);
                Serial.println("EOD");
            }
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
            else if (scenario == "CEF") {  // cycling endurance test (short pulse)
                /*
                    Cycling endurance test (currently goes up to 1E10 cycles)
                    Add more if needed
                    extra1: number of total cycles in power of 10
                */
                // GND weight
                gnd_all_nosel(20);
                //////////////////////////////////////////////////////////
                if (extra1 > 0) {   // cycle 1 - 10
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "0, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "0, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 1) {   // cycle 11 - 100
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "1, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "1, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    for (int i = 0; i < 80; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 2) {   // cycle 101 - 1,000
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "2, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "2, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                    
                    for (int i = 0; i < 890; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 3) {   // cycle 1,001 - 10,000
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "3, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "3, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    for (int i = 0; i < 8990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 4) {   // cycle 10,001 - 100,000 (1E5)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "4, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "4, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                    
                    for (int i = 0; i < 89990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 5) {   // cycle 100,001 - 1,000,000 (1E6)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "5, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "5, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    for (int i = 0; i < 899990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 6) {   // cycle 1,000,001 - 5,000,000 (5E6)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 5E6 cycle");
                    for (int i = 0; i < 3999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 6) {   // cycle 5,000,001 - 10,000,000 (1E7)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "6, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                    
                    Serial.println("Checkpoint-going up to 1E7 cycle");
                    for (int i = 0; i < 4999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 7) {   // cycle 10,000,001 - 50,000,000 (5E7)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                    
                    Serial.println("Checkpoint-going up to 5E7 cycle");
                    for (int i = 0; i < 39999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 7) {   // cycle 50,000,001 - 100,000,000 (1E8)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "7, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 1E8 cycle");
                    for (int i = 0; i < 49999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 8) {   //  cycle 100,000,001 - 500,000,000 (5E8)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 5E8 cycle");
                    for (int i = 0; i < 399999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 8) {   //  cycle 500,000,001 - 1,000,000,000 (1E9)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "8, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 1E9 cycle");
                    for (int i = 0; i < 499999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 9) {   //  cycle 1,000,000,001 - 5,000,000,000 (5E9)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 5E9 cycle");
                    for (int i = 0; i < 3999999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                if (extra1 > 9) {   //  cycle 5,000,000,001 - 10,000,000,000 (1E10)
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < update_num; j++) {
                            potentiation_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                        for (int j = 0; j < update_num; j++) {
                            depression_fast_nosel();
                            if ((j + 1) % read_period == 0) {
                                int tempADC = readADC(read_time, read_set_time, read_delay);
                                char strBuf[50];
                                sprintf(strBuf, "9, %d", tempADC);
                                Serial.println(strBuf);
                            }
                        }
                    }
                
                    Serial.println("Checkpoint-going up to 1E10 cycle");
                    for (int i = 0; i < 4999999990; i++) {
                        potentiation_cycle_fast_nosel(update_num);
                        depression_cycle_fast_nosel(update_num);
                    }
                }
                //////////////////////////////////////////////////////////
                //  add more if more cyclic stress is required
                //////////////////////////////////////////////////////////
                for (int i = 0; i < 10; i++) {  // Final read section after all cycle
                    for (int j = 0; j < update_num; j++) {
                        potentiation_fast_nosel();
                        if ((j + 1) % read_period == 0) {
                            int tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", int(extra1), tempADC);
                            Serial.println(strBuf);
                        }
                    }
                    for (int j = 0; j < update_num; j++) {
                        depression_fast_nosel();
                        if ((j + 1) % read_period == 0) {
                            int tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", int(extra1), tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }
                delay(50);
                Serial.println("EOD");
            }

            else if (scenario == "SYMM_CHECK") {
                /*
                 * extra1: send to symmetry point mode
                            0: Alternating Potentiation-Depression
                            1: N1 and N2 simultaneous ON
                 * extra2: initial potentiation pulse number
                 * extra3: potentiation/depression batch pulse number
                */
                if (extra3 < 1) { // extra3 value guard
                    extra3 = 1.0;
                }
                // initiallize to GND
                gnd_all_nosel(20);
                // initial potentiation to reach starting point
                for (int i = 0; i < int(extra2); i++) {
                    potentiation_nosel(pulse_width, zero_time);
                }
                // initial read
                int tempADC = readADC(read_time, read_set_time, read_delay);
                char strBuf[50];
                sprintf(strBuf, "%d, %d", 0, tempADC);
                Serial.println(strBuf);

                if (int(extra1) == 0) { // alternating potentiation-depression symmetry
                    for (int i = 0; i < update_num; i++) {
                        // batched update (PDPD vs PPPDDDPPPDDD...)
                        for (int j = 0; j < int(extra3); j++) {
                            potentiation_nosel(pulse_width, zero_time);
                        }
                        for (int j = 0; j < int(extra3); j++) {
                            depression_nosel(pulse_width, zero_time);
                        }
                        // periodic read
                        if ((i + 1) % read_period == 0) {
                            int tempADC = readADC(read_time, read_set_time, read_delay);
                            char strBuf[50];
                            sprintf(strBuf, "%d, %d", i + 1, tempADC);
                            Serial.println(strBuf);
                        }
                    }
                }
                else if (int(extra1) == 1) {
                    // N1 & N2 (WWLP & WWLD) ON
                    PIOC->PIO_SODR = N1_PINLIST[0] | N2_PINLIST[0];   
                    delayMicroseconds(update_num);
                    PIOC->PIO_CODR = N1_PINLIST[0] | N2_PINLIST[0];

                    // SP read
                    int tempADC = readADC(read_time, read_set_time, read_delay);
                    char strBuf[50];
                    sprintf(strBuf, "%d, %d", 0, tempADC);
                    Serial.println(strBuf);
                }
                delay(50);
                Serial.println("EOD");
            }
        }
    }
}

/*======================================================================
========================================================================
======================================================================*/

void gnd_all_nosel(int GND_time) {
    /*
        GND storage capacitor by connecting both nodes
        to GND for (GND_time) ms
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    delayMicroseconds(10);
    // Turn on N2 pin
    PIOC->PIO_SODR  = N2_PINLIST[0];
    delay(GND_time);
    // Turn off N2 pin & disable weight update
    PIOC->PIO_CODR  = N2_PINLIST[0];
    delayMicroseconds(1);
    PIOC->PIO_CODR  = N4_PINLIST[0];
}

void potentiation_nosel(int pulse_width, int zero_time) {
    /*
        Single potentiation event
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    delayMicroseconds(1);
    // Turn on N1 pin
    PIOC->PIO_SODR  = N1_PINLIST[0];
    delayMicroseconds(pulse_width);
    // Turn off N1 pin & disable weight update
    PIOC->PIO_CODR  = N1_PINLIST[0];
    delayMicroseconds(1);
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // zero time delay
    delayMicroseconds(zero_time);
}

void potentiation_fast_nosel() {
    /*
        Single short-pulsed potentiation event
        Forced CPU sleep for delaying
        target pulse width ~200 ns
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // Turn on N1 pin
    PIOC->PIO_SODR  = N1_PINLIST[0];
    // force CPU off
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // Turn off N1 pin & disable weight update
    PIOC->PIO_CODR  = N1_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // zero time delay
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
}

void potentiation_cycle_nosel(int pulse_width, int repeat_number) {
    /*
        Burst potentiation event x(repeat_number)
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    delayMicroseconds(1);
    for (int i = 0; i < repeat_number; i++) {
        // Turn on N1 pin
        PIOC->PIO_SODR  = N1_PINLIST[0];
        delayMicroseconds(pulse_width);
        // Turn off N1 pin
        PIOC->PIO_CODR  = N1_PINLIST[0];
        delayMicroseconds(1);
    }
    // Disable weight update
    PIOC->PIO_CODR  = N4_PINLIST[0];
}

void potentiation_cycle_fast_nosel(int repeat_number) {
    /*
        Burst potentiation event x(repeat_number)
        short-pulsed depression burst (target ~200 ns)
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    for (int i = 0; i < repeat_number; i++) {
        // Turn on N1 pin
        PIOC->PIO_SODR  = N1_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // Turn off N1 pin
        PIOC->PIO_CODR  = N1_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
    // Disable weight update
    PIOC->PIO_CODR  = N4_PINLIST[0];
}

void depression_nosel(int pulse_width, int zero_time) {
    /*
        Single depression event
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    delayMicroseconds(1);
    // Turn on N2 pin
    PIOC->PIO_SODR  = N2_PINLIST[0];
    delayMicroseconds(pulse_width);
    // Turn off N2 pin & disable weight update
    PIOC->PIO_CODR  = N2_PINLIST[0];
    delayMicroseconds(1);
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // zero time delay
    delayMicroseconds(zero_time);
}

void depression_fast_nosel() {
    /*
        Single short-pulsed depression event
        Forced CPU sleep for delaying
        target pulse width ~200 ns
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // Turn on N2 pin
    PIOC->PIO_SODR  = N2_PINLIST[0];
    // force CPU off
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // Turn off N2 pin & disable weight update
    PIOC->PIO_CODR  = N2_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    PIOC->PIO_CODR  = N4_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
}

void depression_cycle_nosel(int pulse_width, int repeat_number) {
    /*
        Burst depression event x(repeat_number)
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    delayMicroseconds(1);
    for (int i = 0; i < repeat_number; i++) {
        // Turn on N2 pin
        PIOC->PIO_SODR  = N2_PINLIST[0];
        delayMicroseconds(pulse_width);
        // Turn off N2 pin
        PIOC->PIO_CODR  = N2_PINLIST[0];
        delayMicroseconds(1);
    }
    // Disable weight update
    PIOC->PIO_CODR  = N4_PINLIST[0];
}

void depression_cycle_fast_nosel(int repeat_number) {
    /*
        Burst depression event x(repeat_number)
        short-pulsed depression burst (target ~200 ns)
        Only works when using (0, 0) address
    */
    // Enable weight update
    PIOC->PIO_SODR  = N4_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    for (int i = 0; i < repeat_number; i++) {
        // Turn on N2 pin
        PIOC->PIO_SODR  = N2_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // Turn off N2 pin
        PIOC->PIO_CODR  = N2_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
    // Disable weight update
    PIOC->PIO_CODR  = N4_PINLIST[0];
}

int readADC(int read_time, int read_set_time, int read_delay) {
    int ADC_val     = 1024;
    
    PIOB->PIO_CODR  = 1 << 26;
    PIOB->PIO_CODR  = 1 << 25; // digitalWrite(HL_CHOP,LOW)

    delayMicroseconds(read_set_time);

    PIOB->PIO_CODR = 1 << 14; //CON
    PIOA->PIO_SODR = 1 << 19; //DS
    PIOB->PIO_SODR = 1 << 21; //CR

    for (int i = 0; i < read_time; i++) {
        PIOD->PIO_SODR = WL_READ_PINLIST[0] ; //WL SET
        PIOA->PIO_SODR = 1 << 7 ; //DFF1 CLK HIGH
        PIOA->PIO_CODR = 1 << 7 ; //DFF1 CLK LOW  --> DFF clocking needed while writing same WL?
        PIOD->PIO_CODR = WL_READ_PINLIST[0] ; //WL clear
        delayMicroseconds(1000);  //  1 ms delay as unit read time
    }

    for (int i = 0; i < read_delay; i++) {
        PIOA->PIO_SODR = 1 << 7; //DFF1 CLK HIGH
        PIOA->PIO_CODR = 1 << 7; //DFF1 CLK LOW
        PIOD->PIO_CODR = WL_READ_PINLIST[0]; //WL clear
        delayMicroseconds(10);  //
    }

    PIOA->PIO_SODR  = 1 << 7 ; //DFF1 CLK HIGH
    PIOA->PIO_CODR  = 1 << 7 ; //DFF1 CLK LOW
    PIOA->PIO_CODR  = 1 << 19; //DS ON
    PIOB->PIO_SODR  = 1 << 14; //CON OFF

    ADC_val         = ADC->ADC_CDR[7];// read data on A0

    PIOB->PIO_CODR = 1 << 21; //CR
    delay(3);

    return ADC_val / 4;
}

void potentiation_sel(int pre_en, int pulse_width, int post_en, int zero_time) {
    /*
        Single potentiation event
        Only works when using (0, 0) address
    */
    // turn on weight enable
    PIOC->PIO_SODR  = N4_PINLIST[0];
    delayMicroseconds(pre_en);
    // turn on N1 pin for (pulse_width) us
    PIOC->PIO_SODR  = N1_PINLIST[0];
    delayMicroseconds(pulse_width);
    // turn off N1 pin
    PIOC->PIO_CODR  = N1_PINLIST[0];
    delayMicroseconds(post_en);
    // turn off weight enable after (post_en) us delay
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // cease operation for (zero_time) us
    delayMicroseconds(zero_time);
}

void potentiation_sel_fast() {
    /*
        Single potentiation event with short pulse
        Only works when using (0, 0) address
    */
    // turn on weight enable
    PIOC->PIO_SODR  = N4_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // turn on N1 pin for ~200 ns
    PIOC->PIO_SODR  = N1_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // turn off N1 pin
    PIOC->PIO_CODR  = N1_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // turn off weight enable after ~200 ns delay
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // cease operation for ~200 ns
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
}

void depression_sel(int pre_en, int pulse_width, int post_en, int zero_time) {
    /*
        Single depression event
        Only works when using (0, 0) address
    */
    // turn on weight enable
    PIOC->PIO_SODR  = N4_PINLIST[0];
    delayMicroseconds(pre_en);
    // turn on N2 pin for (pulse_width) us
    PIOC->PIO_SODR  = N2_PINLIST[0];
    delayMicroseconds(pulse_width);
    // turn off N2 pin
    PIOC->PIO_CODR  = N2_PINLIST[0];
    delayMicroseconds(post_en);
    // turn off weight enable after (post_en) us delay
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // cease operation for (zero_time) us
    delayMicroseconds(zero_time);
}

void depression_sel_fast() {
    /*
        Single depression event with short pulse
        Only works when using (0, 0) address
    */
    // turn on weight enable
    PIOC->PIO_SODR  = N4_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // turn on N2 pin for ~200 ns
    PIOC->PIO_SODR  = N2_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // turn off N2 pin
    PIOC->PIO_CODR  = N2_PINLIST[0];
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    // turn off weight enable after ~200 ns delay
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // cease operation for ~200 ns
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
    __asm__ __volatile__ ("nop\n\t");
}

int readADC_sel(int read_time, int read_set_time, int read_delay, bool boost_bool) {
    int ADC_val     = 1024;

    PIOB->PIO_CODR  = 1 << 26;
    PIOB->PIO_CODR  = 1 << 25; // digitalWrite(HL_CHOP,LOW)

    delayMicroseconds(read_set_time);

    PIOB->PIO_CODR  = 1 << 14; //CON
    PIOA->PIO_SODR  = 1 << 19; //DS
    PIOB->PIO_SODR  = 1 << 21; //CR

    // set cap bottom electrode voltage
    PIOC->PIO_CODR  = N4_PINLIST[0];
    // note, CODR config. is HIGH voltage in VBE select scenario
    if (boost_bool) {
        PIOC->PIO_SODR  = N4_PINLIST[0];
    }

    for (int i = 0; i < read_time; i++) {
        PIOD->PIO_SODR = WL_READ_PINLIST[0] ; //WL SET
        PIOA->PIO_SODR = 1 << 7 ; //DFF1 CLK HIGH
        PIOA->PIO_CODR = 1 << 7 ; //DFF1 CLK LOW  --> DFF clocking needed while writing same WL?
        PIOD->PIO_CODR = WL_READ_PINLIST[0] ; //WL clear
        delayMicroseconds(1000);  //  1 ms delay as unit read time
    }

    for (int i = 0; i < read_delay; i++) {
        PIOA->PIO_SODR = 1 << 7; //DFF1 CLK HIGH
        PIOA->PIO_CODR = 1 << 7; //DFF1 CLK LOW
        PIOD->PIO_CODR = WL_READ_PINLIST[0]; //WL clear
        delayMicroseconds(10);  //
    }
    PIOC->PIO_CODR  = N4_PINLIST[0];
    PIOA->PIO_SODR  = 1 << 7 ; //DFF1 CLK HIGH
    PIOA->PIO_CODR  = 1 << 7 ; //DFF1 CLK LOW
    PIOA->PIO_CODR  = 1 << 19; //DS ON
    PIOB->PIO_SODR  = 1 << 14; //CON OFF

    ADC_val         = ADC->ADC_CDR[7];// read data on A0

    PIOB->PIO_CODR  = 1 << 21; //CR
    delay(3);

    return ADC_val / 4;
}


void fast_pulse_200(bool ispot) {
    /*
     * fast pulse function targetting 200 ns pulse width
     * works for both potentiation and depression
    */
    if (ispot) {
        // case potentiation
        // 200 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 200 ns
        PIOC->PIO_SODR  = N1_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N1_PINLIST[0];
        // 200 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
    else {
        // case depression
        // 200 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 200 ns
        PIOC->PIO_SODR  = N2_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N2_PINLIST[0];
        // 200 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
}

void fast_pulse_300(bool ispot) {
    /*
     * fast pulse function targetting 200 ns pulse width
     * works for both potentiation and depression
    */
    if (ispot) {
        // case potentiation
        // 300 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 300 ns
        PIOC->PIO_SODR  = N1_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N1_PINLIST[0];
        // 300 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
    else {
        // case depression
        // 300 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 300 ns
        PIOC->PIO_SODR  = N2_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N2_PINLIST[0];
        // 300 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
}

void fast_pulse_400(bool ispot) {
    /*
     * fast pulse function targetting 400 ns pulse width
     * works for both potentiation and depression
    */
    if (ispot) {
        // case potentiation
        // 400 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 400 ns
        PIOC->PIO_SODR  = N1_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N1_PINLIST[0];
        // 400 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
    else {
        // case depression
        // 400 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 400 ns
        PIOC->PIO_SODR  = N2_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N2_PINLIST[0];
        // 400 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
}

void fast_pulse_500(bool ispot) {
    /*
     * fast pulse function targetting 500 ns pulse width
     * works for both potentiation and depression
    */
    if (ispot) {
        // case potentiation
        // 500 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 500 ns
        PIOC->PIO_SODR  = N1_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N1_PINLIST[0];
        // 500 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
    else {
        // case depression
        // 500 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        // WWL ON for 500 ns
        PIOC->PIO_SODR  = N2_PINLIST[0];
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        PIOC->PIO_CODR  = N2_PINLIST[0];
        // 500 ns delay for voltage stabilize
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
        __asm__ __volatile__ ("nop\n\t");
    }
}
