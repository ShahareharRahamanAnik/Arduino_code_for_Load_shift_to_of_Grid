// Definition
#include<stdio.h>
// #include<conio.h>

void setup() {

  
	// Microcontroller pin setting 
        //Analog input of Load circuit, utility grid and inverter Grid
  pinMode(A0, INPUT);  // set A0 as input of load circuit N
  pinMode(A1, INPUT);  // set A1 as input of Load of Utility Grid
  pinMode(A2, INPUT);  // set A2 as input of Load of inverter Grid
  pinMode(A3, INPUT);   // command input for single loop of operation


        // PinMode for Load Circuit relay switching (on/off)
  pinMode(5, OUTPUT);   // set pin 5 as output (LSB for channel selection)
  pinMode(6, OUTPUT);   // set pin 6 as output (MSB for channel selection)
  pinMode(7, OUTPUT);   // set pin 7 as output (ESB for channel selection)
//Set Initial Values (for load circuit switching  Pins)

		digitalWrite(5, LOW);   // multiplexer input of master reset pin
    digitalWrite(6, LOW);       // multiplexer Write enable (WR) Pin 
    digitalWrite(7, LOW);       // multiplexer input signal (D) Pin

      // Digital Output for channel Selection 
      pinMode(11, OUTPUT);  // set pin 11 as output
      pinMode(12, OUTPUT);  // set pin 12 as output
      pinMode(13, OUTPUT);  // set pin 13 as output

          //Set Initial Values (for Channel Selector Pins)
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);

	   // initial setting of variables
	    int num=0;
	    int relay_state (num)		                  =0;       // rls, stores logical condition of N-th circuit relay
       float utility_instantaneous_level		    =0.0000;  // uil, instantaneous values of utility grid load
	//
        float inverter_upper_threshold_level		=4.9999; //iutl, upper limit of inverter load
        float inverter_instantaneous_level		  =0.0000; // iil, instantaneous value of inverter load
        float inverter_instantaneous_diff_limit	=0.0500; // iidl, limit between threshold and instantaneous load

	//
        float load_circuit_N_threshold_level 	      =2.4600; // lctl_N, Lower limit of N-th load line
         float load_circuit_N_instantaneous_level_1	=0.0000; // lcil_N_1, instantaneous value-1 of N-th load line
        float load_circuit_N_instantaneous_level_2	=0.0000; // lcil_N_2, instantaneous value-2 of N-th load line 
        float load_circuit_N_transient_level		    =0.0000; // lctl, initial value of transient load of N-th load line 
        float load_circuit_N_diff_limit		          =0.0500; // lcdl_N, limit of difference of two instantaneous input values of selected load line
        float load_circuit_N_diff      	            =0.0500; // lcd_N, difference between two successive input values of selected load line 

              }



// MAIN PROCESS // 
	//----------------------------------------------------------------------//


//void loop () {
   for (int num = 0; num < 8; num++) {     // loop through decimal numbers 0 to 7
      int printf("Load Line= %d", num, );
	// Select Load circuit N (N=0 to 7)
      digitalWrite (11, num & 0x01);         // set pin 11 to the least significant bit of num
      digitalWrite(12, (num & 0x02) >> 1);  // set pin 12 to the second least significant bit of num
      digitalWrite(13, (num & 0x04) >> 2);  // set pin 13 to the third least significant bit of num
      delay(3000);                          // wait for one second



      load_circuit_N_instantaneous_level_1 = analogread(A0); 	// First instantaneous sample of load line N 
      int printf (" inst. level_1= %f", load_circuit_N_instantaneous_level_1 ); // 
      delay(300); 							                              // wait 1 second for next transient (if) sample 
      load_circuit_N_instantaneous_level_2 = analogread(A0); 	// Second instantaneous sample of load line N
      int printf (" inst. level_2= %f", load_circuit_N_instantaneous_level_2 ); // 



		        // calculate difference between two successive inputs 
      load_circuit_N_diff = load_circuit_N_instantaneous_level_1 - load_circuit_N_instantaneous_level_2;
          int printf (" Differential= %f", load_circuit_N_diff ); // 
            // calculate % difference between two successive inputs
      load_circuit_N_transient_level= (load_circuit_N_diff /load_circuit_N_instantaneous_level_1)*100; 
             int printf (" Transient level= %f", load_circuit_N_transient_level);

      if (load_circuit_N_transient_level>10.00 !! load_circuit_N_diff> load_circuit_N_diff_limit ) {
            // check if transient is upward,Load load line N is just loaded 
            // check if load circuit N is already Higher than limit i.e load line is loaded earlier


            //process to Switch on the N-th load line relay //
      If (relay_state (num)!=0) {
      digitalWrite (7, HIGH); relay_state (num) = 1;    // set pin 7 to High, (ready for switch ON (latch) the relay)
	    digitalWrite (6, HIGH);                         // set pin 6 to high (Confirms Turn on the relay)
      delay (100);                          	          // wait for 100 msecond, settle the pin in desired mode
      int printf ("Relay state=%d")
                                       }  
// Reset pins
        digitalWrite (6, LOW);
        delay(1000);                      // set pin 6 to LOW, switching disabled 
        digitalWrite (7, LOW);                       // set pin 7 to LOW,  


} 
    else if (load_circuit_N_transient_level<10.00 !! load_circuit_N_diff< load_circuit_N_diff_limit) {
						// check if Load circuit N is just unloaded 
						// or if already unloaded earlier

	// Process to Switch OFF the N-th load line relay
  If (relay_state (num)!=01 {
        digitalWrite (7, LOW); relay_state (num) = 0;   // set pin 7 to LOW,  
        digitalWrite (6, LOW);                          // set pin 6 to LOW, switching disabled 
        delay (100);                          	        // wait for 100 msecond, settle the pin in desired mode
  }
	}	
					
	 // Resets Switching Pins in initial state       
        digitalWrite (6, LOW);                        // set pin 6 to LOW, switching disabled 
        digitalWrite (7, LOW);                        // set pin 7 to LOW,  
        delay (100);                          	      // wait for 100 msecond, to settle the pin in desired mode

    	utility_instantaneous_level         = analogread(A1); // reading utility Grid_instantaneous_load
    	inverter_instantaneous_level        = analogread(A2); // reading inverter Grid instantaneous load


    if ((inverter_upper_threshold_level -inverter_instantaneous_level)> inverter_instantaneous_diff_limit)) {
					// checks if inverter is overloaded 

//process to Switch OFF the N-th load circuit relay //

 	  digitalWrite (7, LOW); relay_state (num) = 0;   // set pin 6 to high (Confirm Turn off the relay)
	  digitalWrite (6, HIGH);                         // set pin 6 to LOW (Initial state)
         delay (100);                               // wait for one second
	  digitalWrite (6, LOW);                          // set pin 6 to LOW (Initial state)


}

      }

      
    //}
   

