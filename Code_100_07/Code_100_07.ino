

 #include <EEPROM.h>
 #include <stdlib.h> 

        float LineLoad [8] , UtilityLoad =0.00, InverterLoad = 0.00 ; 
        float ICC = 0.00 ; // Inverter Captive Capacity
        float IFC = 6000.00 ; // Inverter Full Load Capacity
        int cycle=0, line; bool relay_status [8] ;

     float ScanLineLoad (int line) ;
     float ScanInverterLoad () ;
     void TransferInInverterBus (int line) ;
     void TransferToUtilityBus (int line) ;

 

  void setup() {
  Serial.begin(9600);         
        pinMode(A8, INPUT); pinMode(5, OUTPUT); digitalWrite(5, LOW); pinMode(11, OUTPUT); digitalWrite(11, LOW);  
        pinMode(A9, INPUT); pinMode(6, OUTPUT); digitalWrite(6, LOW); pinMode(12, OUTPUT); digitalWrite(12, LOW); 
        pinMode(A10, INPUT); pinMode(7, OUTPUT); digitalWrite(7, LOW); pinMode(13, OUTPUT); digitalWrite(13, LOW);     
               
     }
                                                     // *************// 
                                                     // Main Function Starts //

    void loop ()  {
                                        
        Serial.println (); Serial.print (" Operating Cycle = "); Serial.println (cycle);
        Serial.println (); Serial.println (); Serial.println ();
        UtilityLoad = ScanUtilityLoad () ;
        InverterLoad = ScanInverterLoad () ;

      for (line = 0; line < 8 ; line++) {

          LineLoad [line] = ScanLineLoad (line);
          ICC = ((IFC -  (InverterLoad + LineLoad [line] ) )/ IFC ) * 100.00 ;
            if ( LineLoad [line] >= 10.00 && ICC >=5.00  ) {
              if ( !relay_status [line]) {
                Serial.println (" Line is to be Transfer IN Inverter BUS ");
                TransferInInverterBus (line); 
              }
              else if ( relay_status [line] ) Serial.println (" Line already IN Inverter BUS ");  
              
            } 
             
           else if  ( LineLoad [line]<=10.00 && relay_status [line] ) {
              Serial.println (" Line is to be Transfer OUT to Utility BUS ");
              TransferToUtilityBus (line) ;
            } 
            else if ( LineLoad [line]<=10.00 && !relay_status [line] ) {
              Serial.println (" Line is not Loaded, Action skipped ");
            }
              
             
                }  
                   Serial.println () ; Serial.println () ;
                   Serial.print ("           * * * *        Memory cleaning in progress, please wait             * * * * ");
                    for (int i = 0 ; i < EEPROM.length() ; i++) {EEPROM.write(i, 0); }  
            cycle++;               
          }   

// Function Definition Area // 

                float ScanLineLoad (int line) { // Start of ScanLineLoad () Function //

                    float sample [100] ;
            
                   // Serial.print ("Address of sample array : ") ; // Serial.println ( &sample) ;
                    Serial.println ("           * * * *       Load Circuit Scanning in progress, please wait       * * * * ");
                    Serial.println ();
                    digitalWrite (11, line & 0x01); 
                    digitalWrite(12, (line & 0x02) >> 1); 
                    digitalWrite(13, (line & 0x04) >> 2); // Line selection
                    delay (10) ;
                    Serial.print (" Load Circuit No #  ");  Serial.print (line);
                    for (int i=0; i<100; i++) { *( sample + i )   = analogRead(A8) ;}          
                    Serial.print (", Scanning completed. ");
                  //  Serial.println (" Values Printing in progress ... ");
                
                      for (int i=0; i<100; i++) {             // sort in ascending order
                        sample [i]   = abs (( sample [i] * (5.00/1023.00))-2.50075 ) * 808.00 ;  
                    //  Serial.print (" Line  =  "); Serial.print (line); Serial.print (",  Sample no  =   ");  Serial.print (i);                      
                    //  Serial.print (",  Line Load (VA) =   ");  Serial.println ( Line_sample [line] [i] ); 
                      }
                    //   Serial.println (" Line Samples Printing complete, Finds the Peak Value ... ");
                    for (int i=1; i<100; i++) { 
                    //  Line_sample [i]   = abs ((( Line_sample [i] * 5.00/1023.00)-2.50075 ) * 808.00) ;                        
                        if (sample  [0] < sample [i]) { 
                          sample [0] = sample [i]; 
                                               
                         } 

                    } 

                       if (sample [0] > LineLoad [line]) {
                         LineLoad [line] = sample [0] ;
                       }

                      LineLoad [line] = sample [0] ; 
                      if ( LineLoad [line] <= 10.00 )  LineLoad [line] = 0.00 ;   
                      Serial.print (",   Load (VA)  = ");  Serial.println (LineLoad [line] );
                    //  free (sample) ; 
                     // sample =NULL ;
                      if (sample !=NULL) Serial.println(" Line Load Memory not reclaimed") ;                               
        return ( LineLoad [line] )  ;                      
      }
                      

                                          /**********************/

      float ScanUtilityLoad () {  // Start of ScanInverterLoad () Function // 

              float sample [200] ;
              Serial.println ();
              Serial.println ("           * * * *       Utility Load Scanning in progress, please wait       * * * * "); 
              Serial.println ();
                delay (10) ;   
                for (int i=0 ; i<200; i++) { *( sample + i ) = analogRead(A10) ; } // reads line load at Pin A9   
              Serial.print (" Scanning completed ");                  
            //  Serial.println ("   Samples printing in progress ... ");

               for (int i=0; i<200; i++) { // sort in ascending order
                 sample [i] =  abs ((( sample [i] * (5.00/1023.00))-0.503424) * 2011.00 )  ; // convert sample in VA
              //   Serial.print (" Utility Sample No = "); Serial.print (i); 
              //   Serial.print (",  Value = "); Serial.println ( sample [i] );
             }            

            //  Serial.println (" Utility Load Samples printing completed. "); 
              Serial.println (",   Finding peak Load value in progress ... ");  
              for (int i=1; i<=200; i++) { // Finds peak Value and stores in 0 index
                if (sample [0] < sample [i]) {sample [0] = sample [i] ; 
                }
                //  sample [i]  = NULL ;
                //  if ( sample [i] != NULL ) Serial.println(" Not Nulled ");                  
              }
              if ( sample [0]<= 10.00 ) sample [0] = 0.00 ;
              Serial.print (" Utility Peak Load (VA)=  \t"); Serial.println ( sample [0] ); 
            //  UtilityLoad = sample [0] ; 
              free (sample) ;   
              if (sample !=NULL) Serial.println(" Utility Load Memory not reclaimed") ;
              return sample [0] ;   
            }

                                          /**********************/  

      float ScanInverterLoad () {  // Start of ScanInverterLoad () Function //  
              float sample [200] ;
              Serial.println ();
              Serial.println ("           * * * *     Iverter Load measurement in progrees, please wait     * * * * ");
              Serial.println (); 
                delay (50) ;   
                for (int i=0 ; i<200; i++) { *( sample + i ) = analogRead(A9) ; } // reads line load at Pin A9 
              Serial.print (" Scanning completed. ");   
            //  Serial.println (" Samples printing in progress ... ");

               for (int i=0; i<200; i++) { // sort in ascending order
                 sample [i] =  abs ((( sample [i] * (5.00/1023.00))-0.503424) * 2011.00 )  ; // convert sample in VA
              //   Serial.print (" Inverter Sample No = "); Serial.print (i); 
              //   Serial.print (", Value = "); Serial.println ( Inverter_sample [i] );
             }            
            //  Serial.println (" Iverter Load sample printing completed. "); 
              Serial.println (",  Finding peak Load in progress ... ");  
              for (int i=1; i<=200; i++) { // Finds peak Value and stores in 0 index
                if (sample [0] < sample [i]) {sample [0] = sample [i] ; }
              }
              if ( sample [0] <= 10.00 ) sample [0] = 0.00 ;

              Serial.print (" Inverter Load (VA)          = \t"); Serial.println ( sample [0] ); 
              Serial.print (" Inverter Total Capacity     = \t");  Serial.println (IFC);
              Serial.print (" Inverter Available Capacity = \t"); Serial.println (IFC - sample [0] );  
              Serial.print (" Inverter Available Capacity = \t"); Serial.print (((IFC - sample [0]) / IFC ) * 100.00); Serial.println ("%");     
              InverterLoad = sample [0] ;
              free (sample) ;
              if (sample !=NULL) Serial.println(" Inverter Load Memory not reclaimed") ;
              return sample [0] ;   
            }
                 
                 

  void TransferInInverterBus (int line) {  // Start of TransferToInverterBus () Function // 
  
        digitalWrite (7, HIGH); delay (10); digitalWrite (6, HIGH); delay (10); 
        digitalWrite (6, LOW); delay (10); digitalWrite (7, LOW);        
        Serial.println (" -- Circuit Transferred IN INV BUS -- ");
        Serial.println ();Serial.println ();
        InverterLoad +=  LineLoad [line] ; 
        Serial.print (" Inverter Load after Transfer IN = "); Serial.println ( InverterLoad );
        ICC = ((IFC -  InverterLoad )/ IFC ) * 100.00 ;
        Serial.print (" Inverter Captive Capacity (ICC) after Transfer IN            = "); Serial.print (ICC, 2); Serial.println ("%");
        relay_status [line] = HIGH;
        Serial.print (" Relay Status = "); Serial.println ( relay_status [line] );
        return 0 ;  
      } // End of TransferToInverterBus () Function //   

  void TransferToUtilityBus (int line) {  // Start of TransferToUtilityBus () Function // 
        digitalWrite (7, LOW); delay (10); digitalWrite (6, HIGH); delay (10); 
        digitalWrite (6, LOW); delay (10); //digitalWrite (7, LOW); 
        
         Serial.println (" -- Circuit Unloaded, Load Transferred to Utility Grid --  "); 
         Serial.println (); 
         Serial.println ();Serial.println (); 
         InverterLoad -=  LineLoad [line] ;
         Serial.print (" Inverter Load after Transfer OUT = "); Serial.println ( InverterLoad );
        ICC = ((IFC -  InverterLoad )/ IFC ) * 100.00 ;
        Serial.print (" Inverter Captive Capacity (ICC) after Transfer OUT            = "); Serial.print (ICC, 2); Serial.println ("%");
        relay_status [line] = LOW;
        Serial.print (" Relay Status = "); Serial.println ( relay_status [line] );
        return 0 ;
      } // End of TransferToUtilityBus () Function // 

                    