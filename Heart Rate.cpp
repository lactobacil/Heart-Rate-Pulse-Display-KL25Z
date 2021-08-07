
#include <stdio.h>
#include "mbed.h"

Ticker TimerInt;
Timer Global;
Timer BPM; 

DigitalIn Button(PTD5);  //Switch button from the device
DigitalOut gpo(PTD6);       
AnalogIn Ain(PTB1);
AnalogOut Aout(PTE30);

int value;
float frequency, period;
int HeartRate;
int counter = 0;
char  patternarray[8] = {0,0,0,0,0,0,0,0}; //Initialize display array
char  num[8] = {0,0,0,0,0,0,0,0};
float MeasureTime; 
bool load;

SPI max72_spi(PTD2, NC, PTD1);

#define max7219_reg_noop         0x00
#define max7219_reg_digit0       0x01
#define max7219_reg_digit1       0x02
#define max7219_reg_digit2       0x03
#define max7219_reg_digit3       0x04
#define max7219_reg_digit4       0x05
#define max7219_reg_digit5       0x06
#define max7219_reg_digit6       0x07
#define max7219_reg_digit7       0x08
#define max7219_reg_decodeMode   0x09
#define max7219_reg_intensity    0x0a
#define max7219_reg_scanLimit    0x0b
#define max7219_reg_shutdown     0x0c
#define max7219_reg_displayTest  0x0f

#define LOW 0
#define HIGH 1

void write_to_max( int reg, int col)
{
    load = LOW;            // begin
    max72_spi.write(reg);  // specify register
    max72_spi.write(col);  // put data
    load = HIGH;           // make sure data is loaded (on rising edge of LOAD/CS)
}

void pattern_to_display(char *testdata){
    int cdata; 
    for(int idx = 0; idx <= 7; idx++) {
        cdata = testdata[idx]; 
        write_to_max(idx+1,cdata);
    }
} 
    
void digit(int number){         //Ones display of the beats per minute value
    switch(number){
        case 1: num[2]=0x00, num[1]=0x3F, num[0]=0x00;break;
        case 2: num[2]=0x27, num[1]=0x25, num[0]=0x3D;break;
        case 3: num[2]=0x25, num[1]=0x25, num[0]=0x3F;break;
        case 4: num[2]=0x38, num[1]=0x8 , num[0]=0x3F;break;
        case 5: num[2]=0x3D, num[1]=0x25, num[0]=0x27;break;
        case 6: num[2]=0x3F, num[1]=0x25, num[0]=0x27;break;
        case 7: num[2]=0x20, num[1]=0x20, num[0]=0x3F;break;
        case 8: num[2]=0x3F, num[1]=0x25, num[0]=0x3F;break;
        case 9: num[2]=0x38, num[1]=0x28, num[0]=0x3F;break;
        }
        
        pattern_to_display(num); 
    }

void digitens(int number){    //Tens display of the beats per minute value
    switch(number){
        case 0: num[6]=0x00, num[5]=0x3F, num[4]=0x21, num[3]=0x3F;break;
        case 1: num[6]=0x00, num[5]=0x00, num[4]=0x3F, num[3]=0x00;break;
        case 2: num[6]=0x00, num[5]=0x27, num[4]=0x25, num[3]=0x3D;break;
        case 3: num[6]=0x00, num[5]=0x25, num[4]=0x25, num[3]=0x3F;break;
        case 4: num[6]=0x00, num[5]=0x38, num[4]=0x8 , num[3]=0x3F;break;
        case 5: num[6]=0x00, num[5]=0x3D, num[4]=0x25, num[3]=0x27;break;
        case 6: num[6]=0x00, num[5]=0x3F, num[4]=0x25, num[3]=0x27;break;
        case 7: num[6]=0x00, num[5]=0x20, num[4]=0x20, num[3]=0x3F;break;
        case 8: num[6]=0x00, num[5]=0x3F, num[4]=0x25, num[3]=0x3F;break;
        case 9: num[6]=0x00, num[5]=0x38, num[4]=0x28, num[3]=0x3F;break;
        }
        pattern_to_display(num); 
    }
    
void digithundreds(int number){
    if (number == 1){
        num[7]=0x3f;
        }
    else {
        num[7]=0x00;
    }
        pattern_to_display(num); 
    }


void digitbig(int number){                  //Display values of ones when digit less than 100
    switch(number){
        case 1: num[2]=0x00, num[1]=0x3F, num[0]=0x00;break;
        case 2: num[2]=0x27, num[1]=0x25, num[0]=0x3D;break;
        case 3: num[2]=0x25, num[1]=0x25, num[0]=0x3F;break;
        case 4: num[2]=0x38, num[1]=0x8 , num[0]=0x3F;break;
        case 5: num[2]=0x3D, num[1]=0x25, num[0]=0x27;break;
        case 6: num[2]=0x3F, num[1]=0x25, num[0]=0x27;break;
        case 7: num[2]=0x20, num[1]=0x20, num[0]=0x3F;break;
        case 8: num[2]=0x3F, num[1]=0x25, num[0]=0x3F;break;
        case 9: num[2]=0x38, num[1]=0x28, num[0]=0x3F;break;
        }
        
        pattern_to_display(num); 
    }

void digittensbig(int number){            //Display values of tens when digit is less than 100
    switch(number){
        case 0: num[7]=0x00, num[6]=0x3F, num[5]=0x21, num[4]=0x3F, num[3]=0x00;break;
        case 1: num[7]=0x00, num[6]=0x00, num[5]=0x3F, num[4]=0x00, num[3]=0x00;break;
        case 2: num[7]=0x00, num[6]=0x27, num[5]=0x25, num[4]=0x3D, num[3]=0x00;break;
        case 3: num[7]=0x00, num[6]=0x25, num[5]=0x25, num[4]=0x3F, num[3]=0x00;break;
        case 4: num[7]=0x00, num[6]=0x38, num[5]=0x8 , num[4]=0x3F, num[3]=0x00;break;
        case 5: num[7]=0x00, num[6]=0x3D, num[5]=0x25, num[4]=0x27, num[3]=0x00;break;
        case 6: num[7]=0x00, num[6]=0x3F, num[5]=0x25, num[4]=0x27, num[3]=0x00;break;
        case 7: num[7]=0x00, num[6]=0x20, num[5]=0x20, num[4]=0x3F, num[3]=0x00;break;
        case 8: num[7]=0x00, num[6]=0x3F, num[5]=0x25, num[4]=0x3F, num[3]=0x00;break;
        case 9: num[7]=0x00, num[6]=0x38, num[5]=0x28, num[4]=0x3F, num[3]=0x00;break;
        }
        pattern_to_display(num); 
    }


void setup_dot_matrix () {                      //Based on Jon Trinder Display Test
    
    max72_spi.format(8,0);
    max72_spi.frequency(100000);
    
    write_to_max(max7219_reg_scanLimit, 0x07);
    write_to_max(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
    write_to_max(max7219_reg_shutdown, 0x01);    // not in shutdown mode
    write_to_max(max7219_reg_displayTest, 0x00); // no display test
    for (int e=1; e<=8; e++) {    // empty registers, turn all LEDs off
        write_to_max(e,0);
    }
    // maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
    write_to_max(max7219_reg_intensity,  0x08);
    
}

void clearDisp(){
    for (int e=1; e<=8; e++) {    // empty registers, turn all LEDs off
        write_to_max(e,0);
    }
}

void datasample() {             // Function To select the datas
    int j, one, ten, hundred;
    value = Ain.read_u16();                                      
    wait(0.046875*2.25);
    MeasureTime = BPM.read();
    
    if(value<30000)
    {
    patternarray[7] = 0x01;
    }
    else if(value<31350)
    {
    patternarray[7] = 0x03;
    }
    else if(value<31950)
    {
    patternarray[7] = 0x07;
    }
    else if(value<32550)
    {
    patternarray[7] = 0xf;
    }
    else if(value<33150)
    {
    patternarray[7] = 0x1f;
    }
    else if(value<33750)
    {
    patternarray[7] = 0x3f;
    }
    else if(value<34350)
    {
    patternarray[7] = 0x7f;
    }
    else if(value>35050)
    {
    patternarray[7] = 0xff;
    
    
    }
    if(Button){          //When button is switched
        BPM.reset();     //Reset BpmTime
        MeasureTime=0;
        counter = 0;     //Reset Counter
        printf("MeasureTime = %f\n\n", MeasureTime);
        
        for(j = 0; j<7; j++)    {
        patternarray[j] = patternarray[j+1];     //Scrollfunction
        
        }
        pattern_to_display(patternarray);        //Call the function to display the pattern
        

    }
    else {
       
       period = (MeasureTime/(counter+1));      //counter value is passed as a divisor for MeasureTime
       frequency = (1/period);                  // Frequency conversion
       HeartRate = (frequency*60);              // Convert frequency to bpm
       /*Mathematics to sort out values of the bpm */
       one =  HeartRate%10;
       ten = ((HeartRate - one)/10)%10;
       hundred = HeartRate/100;
       
       if (HeartRate>99){   //Display when HeartRate is above 100 BPM
       digit(one);
       digitens(ten);
       digithundreds(hundred);
       }
       else{                //Display when HeartRate is below 100 BPM
       digitbig(one);
       digittensbig(ten);
       }
       
        printf("Data = %i\n", value);              
        printf("Counter = %i\n", counter);
        printf("time = %f\n", Global.read());
        printf("MeasureTime = %f\n", MeasureTime);
        printf("Period = %f\n", period);
        printf("Frequency = %f\n", frequency);
        printf("HeartRate = %i\n", HeartRate);
        printf("One = %i\n",one);
        printf("Ten = %i\n",ten);
        printf("Hundred = %i\n",hundred);
        /*Sets of print function to connect to teraterm*/
    
       if (i>34050){
             counter++;       // Bpm Counter thresholding to count the maximum value 
             if (i<32000){
                 }
                 else wait (0.046875*2.25); // Ideal time to wait for the display refreshes
        }
    }
}


int main() {                                            //Main Function Routine
    Global.start();                                     //Start Timers
    BPM.start();
    TimerInt.attach(&datasample, 0.0125);               //try to erase this
    setup_dot_matrix ();                                //Function to setup the matrix
    clearDisp();                                        //Clear the display
    
    while (1) {
    }
}