 // serial scans lines, if found loaded transfers it in Inverter Grid 
// as long as inverter is under load. if inverter is overloaded line is left in utility Grid
// All load lines and Grid lines are interfaced with microcontroller without ADC
// if absolute value of difference of two samples is greater than a threshold, then scan of lines starts    

 #include <EEPROM.h>

        float Line_sample [100], Inverter_sample [100], LineLoad [8] , UtilityLoad, InverterLoad, InvCapacity = 2.300 ; 
        float ICC = 0.00 ; // Inverter Captive Capacity
        int cycle=0, line; bool relay_status [8], lineLoaded [8];

     float ScanLineLoad () ;
     float ScanInverterLoad () ;
     void TransferInInverterBus (int line) ;
     void TransferInUtilityBus (int line) ;

 

  void setup() {
  Serial.begin(9600);         
        pinMode(A8, INPUT); pinMode(5, OUTPUT); digitalWrite(5, LOW); pinMode(11, OUTPUT); digitalWrite(11, LOW);  
        pinMode(A9, INPUT); pinMode(6, OUTPUT); digitalWrite(6, LOW); pinMode(12, OUTPUT); digitalWrite(12, LOW); 
        pinMode(A10, INPUT); pinMode(7, OUTPUT); digitalWrite(7, LOW); pinMode(13, OUTPUT); digitalWrite(13, LOW);     
    //  Serial.print (" Memory cleaning in progress, please wait.. "); for (int i = 0 ; i < EEPROM.length() ; i++) {EEPROM.write(i, 0);              
     }
                                                     // END of SETUP// 
                                                     // Main Function Starts //

          void loop ()  {
                    float IFC = 6000.00 ; // Inverter Full Load Capacity
                    float IOC = 5500.00 ; // Inverter Optimum Capacity
                    float ICC = 0.00 ; // Inverter Captive Capacity
                    
                     Serial.println (); Serial.print (" Operating Cycle = "); Serial.println (cycle);
                     Serial.println (); Serial.println (); Serial.println ();
                     int time1 =  millis ();
                  //   Serial.print ("           * * * *        Memory cleaning in progress, please wait             * * * * "); for (int i = 0 ; i < EEPROM.length() ; i++) {EEPROM.write(i, 0); } 
                     int time2 = millis () ; 

                for (line = 0; line < 8; line++) {
                    // Line selection
                    //    UtilityLoad = ScanUtilityLoad () ;
                        InverterLoad = ScanInverterLoad () ; 
                        LineLoad [line] = ScanLineLoad ();
                        Serial.print (" Load Circuit # "); Serial.print (line); Serial.print (" , Load = "); Serial.println (LineLoad [line], 4);
                        ICC = ( (IFC - (LineLoad [line] + InverterLoad ))/ IFC ) * 100.00 ;
                        Serial.println (); Serial.println (); Serial.println ();
                        Serial.print (" Inverter Running Load (VA) before Switching Operation = "); Serial.println (InverterLoad, 4);
                        
                        Serial.print (" Calculated Inverter Captive Capacity for next line Switching = "); Serial.print (ICC, 4); Serial.println ("%");
                        delay(100) ;

                        if (ICC >=5.00 && LineLoad[line]>=50) TransferInInverterBus (line);
                        if  ( LineLoad[line]<=50 ) TransferInUtilityBus (line) ;
                        
                        InverterLoad = ScanInverterLoad () ;
                        Serial.print (" Inverter Running Load (VA) after Switching Operation = "); Serial.println (InverterLoad, 4);
                        ICC = ((IFC -  InverterLoad )/ IFC ) * 100.00 ;
                        Serial.print (" Inverter Captive Capacity after switching Operation = "); Serial.print (ICC, 2); Serial.println ("%");
                        
                    cycle++;
                }  // End of Main Function // void loop (0)
          }   

// Function Definition Area // 

                float ScanLineLoad () { // Start of ScanLineLoad () Function //

                    float a, Line_sample [8] [100] ;
                    Serial.println ();
                    Serial.println ("           * * * *       Load Circuit Scanning in progress, please wait       * * * * ");
                for (line = 0; line < 8; line++) {
                    digitalWrite (11, line & 0x01); 
                    digitalWrite(12, (line & 0x02) >> 1); 
                    digitalWrite(13, (line & 0x04) >> 2); // Line selection

                    Serial.print (" Load Circuit No #  ");  Serial.print (line);
                    for (int i=0; i<100; i++) { Line_sample [line] [i]   = analogRead(A8) ;}          
                    Serial.print (" Scanning completed. ");
                  //  Serial.println (" Values Printing in progress ... ");
                
                      for (int i=0; i<100; i++) {             // sort in ascending order
                        Line_sample [line] [i]   = abs (( Line_sample [line] [i] * (5.00/1023.00))-2.50075 ) * 808.00 ;  
                    //  Serial.print (" Line  =  "); Serial.print (line); Serial.print (",  Sample no  =   ");  Serial.print (i);                      
                    //  Serial.print (",  Line Load (VA) =   ");  Serial.println (Line_sample [line] [i],4); 
                      }
                      
                    //   Serial.println (" Line Samples Printing complete, Finds the Peak Value ... ");
                
                    for (int i=1; i<100; i++) { 
                    //  Line_sample [line] [i]   = abs ((( Line_sample [line] [i] * 5.00/1023.00)-2.50075 ) * 808.00) ;                        
                        if (Line_sample [line] [0] < Line_sample [line] [i]) {
                          Line_sample [line] [0] = Line_sample [line] [i]; 
                        }                       
                    } 
                      LineLoad [line] = Line_sample [line] [0] ; 
                    //  Serial.print (" Line  =  "); Serial.print (line);         
                      Serial.print (",  Peak Load (VA) =   ");  Serial.println (Line_sample [line] [0],4); 
                      
                }  
        return ;                      
      }
                      

                                          /**********************/

      float ScanUtilityLoad () {  // Start of ScanInverterLoad () Function // 

              float a ;   
              float Utility_sample [200] ;
              float Deviation [200] ; 
              
              Serial.println ();
              Serial.println ("           * * * *       Utility Load Scanning in progress, please wait       * * * * "); 
                delay (500) ;   
                for (int i=0 ; i<200; i++) { Utility_sample [i] = analogRead(A10) ; } // reads line load at Pin A9   
              Serial.println (" Scanning completed ");                  
            //  Serial.println ("   Samples printing in progress ... ");

               for (int i=0; i<200; i++) { // sort in ascending order
                 Utility_sample [i] =  abs ((( Utility_sample [i] * (5.00/1023.00))-0.503424) * 2011.00 )  ; // convert sample in VA
              //   Serial.print (" Utility Sample No = "); Serial.print (i); 
              //   Serial.print (",  Value = "); Serial.println (Utility_sample [i], 4);
             }            

            //  Serial.println (" Utility Load Samples printing completed. "); 
              Serial.println (" Finding peak Load value in progress ... ");  
              for (int i=1; i<=200; i++) { // Finds peak Value and stores in 0 index
                if (Utility_sample [0] < Utility_sample [i]) {Utility_sample [0] = Utility_sample [i] ; }
              }
              Serial.print (" Utility Peak Load (VA)=  \t"); Serial.println ( Utility_sample [0], 4 );     
              return ( Utility_sample [0] );   
            }

                                          /**********************/  

      float ScanInverterLoad () {  // Start of ScanInverterLoad () Function // 

              float a ;   
              float Inverter_sample [200] ;
              float Deviation [200] ; 
              
              Serial.println ();
              Serial.println ("           * * * *     Iverter Load measurement in progrees, please wait     * * * * "); 
                delay (500) ;   
                for (int i=0 ; i<200; i++) { Inverter_sample [i] = analogRead(A9) ; } // reads line load at Pin A9 
              Serial.println (" Scanning completed. ");   
            //  Serial.println (" Samples printing in progress ... ");

               for (int i=0; i<200; i++) { // sort in ascending order
                 Inverter_sample [i] =  abs ((( Inverter_sample [i] * (5.00/1023.00))-0.503424) * 2011.00 )  ; // convert sample in VA
              //   Serial.print (" Inverter Sample No = "); Serial.print (i); 
              //   Serial.print (", Value = "); Serial.println (Inverter_sample [i], 4);
             }            
            //  Serial.println (" Iverter Load sample printing completed. "); 
              Serial.println (" Finding peak Load in progress ... ");  
              for (int i=1; i<=200; i++) { // Finds peak Value and stores in 0 index
                if (Inverter_sample [0] < Inverter_sample [i]) {Inverter_sample [0] = Inverter_sample [i] ; }
              }
              Serial.print (" Inverter Peak Load (VA)=  \t"); Serial.println ( Inverter_sample [0], 4 );     
              return ( Inverter_sample [0] );   
            }
                 
                 

  void TransferInInverterBus (int line) {  // Start of TransferToInverterBus () Function // 
  
        digitalWrite (7, HIGH); delay (10); digitalWrite (6, HIGH); delay (10); 
        digitalWrite (6, LOW); delay (10); digitalWrite (7, LOW); 
        relay_status [line] = HIGH;       
        Serial.println (" -- Circuit loaded, Inverter in Capacity, Circuit Transferred to INV Grid ");
        Serial.print (" Realy Status = "); Serial.println ( relay_status [line] );
        return ;  
      } // End of TransferToInverterBus () Function //   

  void TransferInUtilityBus (int line) {  // Start of TransferToUtilityBus () Function // 
        digitalWrite (7, LOW); delay (10); digitalWrite (6, HIGH); delay (10); 
        digitalWrite (6, LOW); delay (10); //digitalWrite (7, LOW); 
        relay_status [line] = LOW;
         Serial.println (" *** Inverter Overloaded or Circuit Unloaded  "); 
         Serial.println (" *** Load Transferred Or Left to Utility Grid ");
         Serial.print (" Realy Status = "); Serial.println ( relay_status [line] );          
        return ;
      } // End of TransferToUtilityBus () Function // 

                    /* 
                   configuaration 
                    ****************
                    no of load lines                            =
                    microcontroller supply voltage              =
                    current sensor supply voltage               = 
                    Relay coil supply voltage                   =
                    current sensor no load voltage (line)       =
                    Current sensor no load voltage (Bus)        =
                    max current for linearity of voltage (line) = 
                    max current for linearity of voltage (Bus)  =
                    peak voltage/ ampere (Bus)                  =
                    peak voltage/ ampere (line)                 = 
                    No of voltage samples for Bus               =
                    No of voltage samples for lines             = 
                    No of Bit of ADC converter                  = 
                  */





