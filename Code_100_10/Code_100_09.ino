

 #include <EEPROM.h>
 #include <stdlib.h> 
 int loadPin      = A8  ; // #define loadPin (A8) ;
 int inverterPin  = A9  ;
 int utilityPin   = A10 ; 

        float LineLoad [8] , UtilityLoad =0.00, InverterLoad = 0.00 ; 
        float ICC = 0.00, ICCP =0.00 ; // Inverter Captive Capacity
        float IFC = 6000.00 ; // Inverter Full Load Capacity
        int cycle=0, line; bool relay_status [8] ;
        int nos = 0;         

     float scanBus ( int, int ) ;
     float findUtilityLoad ()  ;
     float findInverterLoad () ;
     float ScanLineLoad (int line) ;     
     void TransferInInverterBus (int line) ;
     void TransferToUtilityBus (int line) ;
     void loadInverter () ;
     void unLoadInverter () ; 

 

  void setup() {
        Serial.begin(9600);         
        pinMode(A8, INPUT); pinMode(5, OUTPUT); digitalWrite(5, LOW); pinMode(11, OUTPUT); digitalWrite(11, LOW);  
        pinMode(A9, INPUT); pinMode(6, OUTPUT); digitalWrite(6, LOW); pinMode(12, OUTPUT); digitalWrite(12, LOW); 
        pinMode(A10, INPUT); pinMode(7, OUTPUT); digitalWrite(7, LOW); pinMode(13, OUTPUT); digitalWrite(13, LOW);               
     }
  
    void loop ()  {   
  //  Serial.println ("           * * * *        Memory cleaning in progress, please wait             * * * * "); for (int i = 0 ; i < EEPROM.length() ; i++) {EEPROM.write(i, 0); }
      float iScanLoad, uScanLoad, uDiffl, iDiffl  ;
      Serial.println (); Serial.println (); 
      Serial.print ( " -- Utility and Inverter load scan starts.. ");


      iScanLoad = findInverterLoad () ;
      Serial.print ( " Inverter is running load (VA) =  "); Serial.println ( iScanLoad );
      uScanLoad = findUtilityLoad () ;
      Serial.print ( " Utility is running load (VA) =  "); Serial.println ( uScanLoad );
      ICC = ((IFC - InverterLoad  )/ IFC ) * 100.00 ;
      uDiffl = ( (UtilityLoad - uScanLoad ) / UtilityLoad ) * 100.00 ;
      iDiffl = ( (InverterLoad - iScanLoad ) / InverterLoad ) * 100.00 ; 

    if (ICC >= 5.00) { 
      Serial.println ( " Inverter capacity is available .. ");

    }
      if (iScanLoad  <=  0.00) {
         Serial.println ( " Inverter is running without load.. ");
         Serial.println ( " Let's Check Utility load.. "); 
         if ( uScanLoad <= 0.00 ) {
           Serial.println ( " Utility is running without load.. ");
         }  else if ( uScanLoad > 0.00 ) { 
              loadInverter () ;
            }
      } 


      UtilityLoad = findUtilityLoad () ;
      InverterLoad = findInverterLoad () ;

      delay (500) ;
     
/*      iPresLoad = findInverterLoad () ; 
      if ( iPresLoad <= 0.00 ) {
        Serial.print ( " Inverter presently running without load.. "); 
        if ( iPrevLoad <= 0.00 ) {
          Serial.print ( " Inverter is abosolutely running without load.. ");
        }

      }
      uPresLoad = findUtilityLoad ()  ;
      if ( uPresLoad <= 0.00 ) {
        Serial.print ( " Utility presently running without load.. "); 
        if ( uPrevLoad <= 0.00 ) {
          Serial.print ( " Utility is abosolutely running without load.. ");
        }
      }
        if ( uPresLoad < uPrevLoad ) {
          Serial.print ( " Utility load increased (VA) = "); Serial.println ( uPrevLoad - uPresLoad ) ; 
          uPresLoad = uPrevLoad ;
        }
        if ( iPresLoad < iPrevLoad ) {
          Serial.print ( " Inverter load increased (VA) = "); Serial.println ( iPrevLoad - iPresLoad ) ; 
          iPresLoad = iPrevLoad ;
        }
*/
 } // loop

  void loadInverter () {  

        Serial.println (); Serial.println (); 
        Serial.print (" Inverter Loading Process... "); 
      
       Serial.println () ; Serial.println () ;
              
      for (line = 0; line < 8 ; line++) {
            if ( !relay_status [line] ) {
              Serial.println (" Circuit on Utility Bus.. "); 
              LineLoad [line] = ScanLineLoad (line) ;
              if ( LineLoad [line] > 10.00) {        
                Serial.println (" Circuit Loaded.. ");
                Serial.println (" Check for ICCP .. "); 
                InverterLoad = findInverterLoad () ;
                ICCP = ((IFC - (InverterLoad + LineLoad [line] ) )/ IFC ) * 100.00 ;
                Serial.print (" ICCP =  ") ; Serial.println (ICCP) ;
                if ( ICCP > 5.00 ) {
                  Serial.println (" Projected Captive Capacity is available .. "); 
                  TransferInInverterBus ( line ) ;
                }
              }
            }     
     }
  }

    void unLoadInverter () {
      Serial.println (); Serial.println (); 
        Serial.print (" Inverter Un-loading Process... "); 
       Serial.println () ; Serial.println () ;
      // Serial.println ("           * * * *        Memory cleaning in progress, please wait             * * * * "); for (int i = 0 ; i < EEPROM.length() ; i++) {EEPROM.write(i, 0); }         

      for (line = 0; line < 8 ; line++) {
              if ( relay_status [line] ) {
              Serial.println (" Circuit on Inverter Bus.. "); 
              LineLoad [line] = ScanLineLoad (line) ;
              if ( LineLoad [line] > 10.00) {        
                Serial.println (" Circuit Loaded.. ");
                Serial.println (" Check for ICCP .. "); 
                ICCP = ((IFC - (InverterLoad + LineLoad [line] ) )/ IFC ) * 100.00 ;
                Serial.print (" ICCP =  ") ; Serial.println (ICCP) ;
                if ( ICCP < 5.00 ) {
                  Serial.println (" Projected Captive Capacity is un-available .. "); 
                  TransferToUtilityBus ( line ) ; 
                }
              } else if ( LineLoad [line] < 10.00 ) {
                  Serial.println (" Circuit Un-loaded.. ");
                   TransferToUtilityBus ( line ) ;
              }                           
            } 
                    InverterLoad = findInverterLoad () ;
     }
  }



// Function Definition Area // 
                                        /**********************/

              float ScanLineLoad (int line) { // Start of ScanLineLoad () Function //
                    float sample [100] ;
                    float temp ;
                    Serial.println ();                   
                    Serial.println ("           * * * *             Circuit Scanning in progress, please wait       * * * * ");
                    Serial.println ();
                    digitalWrite (11, line & 0x01); 
                    digitalWrite(12, (line & 0x02) >> 1); 
                    digitalWrite(13, (line & 0x04) >> 2); // Line selection
                  //.  delay (1000) ;
                    Serial.print ("       Circuit No #  ");  Serial.print (line);
                    temp = scanBus(loadPin, 200) ;
                    LineLoad [line] = (temp -512.00 ) * 0.017201929 *240.00 ; 
                    if (LineLoad [line] < 10.00 ) LineLoad [line] = 0.00 ;
                    Serial.print(", Line Load (VA) = " ); Serial.println( LineLoad [line] );
                    return LineLoad [line] ;                     
                }


       float findUtilityLoad ()  {
         Serial.println( );Serial.println( );
         Serial.print ("     Utility Bus ") ;
          float load ;
          float temp ; 
          temp = scanBus (A10, 200) ;
          load = ( temp-87.00) * 0.039732746 * 240.00  ;
          if ( load< 160.00 ) load = 0.00 ;
          Serial.print(" Utility Load (VA) = " ); Serial.println(load );
          return load ;
       }

       float findInverterLoad () {
          Serial.println();
          Serial.print ("     Inverter Bus ") ;
          float load ;
          float temp ;
          temp = scanBus (A9, 200) ;
          load = ( temp - 87.00) * 0.039732746 * 240.00 ;
          if ( load< 160.00 ) load = 0.00 ;
          Serial.print(" Inverter Load (VA) = " ); Serial.println(load ); 
          return load ;
       }

      float scanBus ( int pin, int nos ) {  // Start of ScanInverterLoad () Function // 
              float sample [nos] ;
              Serial.println ("    ..Scanning in progress, please wait ... "); Serial.println ();
              delay (100) ;
              for (int i=0 ; i<nos; i++)   sample [i]  =  analogRead(pin) ;    // reads line load at Pin A9   
            //.  Serial.print ("             ---Scanning completed "); // Serial.println ("   Samples printing in progress ... ");

              // for (int i=0; i<nos; i++) { // sort in ascending order
                //. sample [i] =  abs ( sample [i] * (5.00/1023.00)) ;

                 // sample [i] =  abs ((( sample [i] * (5.00/1023.00))-0.503424) * 2011.00 )  ; // convert sample in VA
                //  Serial.print ("  Sample No = "); Serial.print (i); 
                //  Serial.print (",  Value = "); Serial.println ( sample [i] );
            // }            
            //  Serial.println (" Samples printing completed. "); 
             // Serial.println (",   Finding peak Load in progress ... ");  
              for (int i=1; i<=nos; i++) { // Finds peak Value and stores in 0 index
                if (sample [0] < sample [i]) {sample [0] = sample [i] ; 
                }
              }
             // if ( sample [0]<= 10.00 ) sample [0] = 0.00 ;
             //  Serial.print ("  Peak Value =  \t"); Serial.println ( sample [0] ); 
              //  free (sample) ;   
              // if (sample !=NULL) Serial.println("  Memory not reclaimed") ;
              return sample [0] ;   
            }

           

                                          /**********************/  

  void TransferInInverterBus (int line) {  // Start of TransferToInverterBus () Function // 
        digitalWrite (7, HIGH); delay (10); digitalWrite (6, HIGH); delay (10); 
        digitalWrite (6, LOW); delay (10); digitalWrite (7, LOW);  
        relay_status [line] = HIGH;      
      //  InverterLoad +=  LineLoad [line] ; 
      //  ICC = ((IFC -  InverterLoad )/ IFC ) * 100.00 ;

        Serial.println ();Serial.println ();
        Serial.println (" -- Circuit Transferred IN Inverter Bus -- ");
      //  Serial.print (" Inverter Load after Transfer IN = "); Serial.println ( InverterLoad );
      //  Serial.print (" Inverter Captive Capacity (ICC) after Transfer IN            = "); Serial.print (ICC, 2); Serial.println ("%");
      //.  Serial.print (" Relay Status = "); Serial.println ( relay_status [line] );
        return 0 ;  
      } // End of TransferToInverterBus () Function //   

  void TransferToUtilityBus (int line) {  // Start of TransferToUtilityBus () Function // 
        digitalWrite (7, LOW); delay (10); digitalWrite (6, HIGH); delay (10); 
        digitalWrite (6, LOW); delay (10); //digitalWrite (7, LOW); 
        relay_status [line] = LOW;
      //  InverterLoad -=  LineLoad [line] ;
      //  ICC = ((IFC -  InverterLoad )/ IFC ) * 100.00 ;
         
        Serial.println (); Serial.println ();Serial.println (); 
        Serial.println (" -- Circuit Transferred to Utility Bus --  "); 
      //  Serial.print (" Inverter Load after Transfer OUT = "); Serial.println ( InverterLoad );
      //  Serial.print (" Inverter Captive Capacity (ICC) after Transfer OUT            = "); Serial.print (ICC, 2); Serial.println ("%");
      //.  Serial.print (" Relay Status = "); Serial.println ( relay_status [line] );
        return 0 ;
      } // End of TransferToUtilityBus () Function // 

                    