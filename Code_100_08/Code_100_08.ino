

 #include <EEPROM.h>
 #include <stdlib.h> 
 int loadPin      = A8  ;
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

 

  void setup() {
        Serial.begin(9600);         
        pinMode(A8, INPUT); pinMode(5, OUTPUT); digitalWrite(5, LOW); pinMode(11, OUTPUT); digitalWrite(11, LOW);  
        pinMode(A9, INPUT); pinMode(6, OUTPUT); digitalWrite(6, LOW); pinMode(12, OUTPUT); digitalWrite(12, LOW); 
        pinMode(A10, INPUT); pinMode(7, OUTPUT); digitalWrite(7, LOW); pinMode(13, OUTPUT); digitalWrite(13, LOW);               
     }
  
    void loop ()  {     

                               
        Serial.println (); Serial.print (" Operating Cycle = "); Serial.println (cycle);
        Serial.println (); Serial.println (); Serial.println ();
        UtilityLoad   = findUtilityLoad () ; // scanBus ( utilityPin, 200 ) ;
        InverterLoad  = findInverterLoad () ;// scanBus ( inverterPin, 200) ;
       // ICC = ((IFC -  InverterLoad  )/ IFC ) * 100.00 ;
       // Serial.print (" Inverter Captive Capacity (ICC)     = "); Serial.println (ICC);
      //  delay (1000) ;
       Serial.println () ; Serial.println () ;
      // Serial.println ("           * * * *        Memory cleaning in progress, please wait             * * * * ");
      // for (int i = 0 ; i < EEPROM.length() ; i++) {EEPROM.write(i, 0); }         

      for (line = 0; line < 8 ; line++) {
          ICC = 0.00 ; ICCP = 0.00 ;
        //  UtilityLoad   = findUtilityLoad () ; // scanBus ( utilityPin, 200 ) ;
        //  InverterLoad  = findInverterLoad () ;// scanBus ( inverterPin, 200) ;
          LineLoad [line] = ScanLineLoad (line);
          ICC = ((IFC -  InverterLoad  )/ IFC ) * 100.00 ;
          ICCP = ((IFC - (InverterLoad + LineLoad [line] ) )/ IFC ) * 100.00 ; 
          Serial.print (" ICC =  ") ; Serial.println (ICC) ;
          Serial.print (" ICCP =  ") ; Serial.println (ICCP) ;


        if (ICC>5.00) {                                            // ICC > 5% ..
          Serial.println (" Inverter Capacity is available.. ") ;  
          if ( LineLoad [line] > 10.00 ) {                          // ICC > 5% & Circuit on inverter.. 
            Serial.println (" Circuit Loaded.. "); 
            if ( relay_status [line] )  {                    // ICC > 5% & Circuit on inverter & Circuit loaded.. 
            Serial.println (" Circuit already in inverter Bus , operation skipped....");                                                     // ICC > 5% & Circuit on inverter & Circuit loaded..
                                                                // No action needed. 
            } else if ( ! relay_status [line] ) {               // ICC > 5% & Circuit on utility..
                Serial.println (" Circuit on Utility Bus, ") ; 
              //  InverterLoad  = findInverterLoad () ;
                Serial.print (" Inverter Load = ") ;  Serial.println (InverterLoad) ;
              //  ICC = ((IFC -  InverterLoad  )/ IFC ) * 100.00 ;
                Serial.print (" ICC = ") ;  Serial.println (ICC) ;
              //  ICCP = ((IFC - (InverterLoad + LineLoad [line] ) )/ IFC ) * 100.00 ;
                Serial.print (" ICCP =  ") ; Serial.println (ICCP) ;

                if (ICCP > 5.00)  {    
                Serial.println (" Circuit shall be transferred to Inverter Bus, ") ;
                TransferInInverterBus (line); 
                InverterLoad  = findInverterLoad () ;// scanBus ( inverterPin, 200) ;

                } else if ( ICCP < 5.00) {
                    Serial.println (" Inverter Will be out of Capacity for this circuit loading.. ") ;
                     Serial.println (" So Tranferring Process skipped.. ") ;
                }
                  //  ICCP = ((IFC - (InverterLoad - LineLoad [line] ) )/ IFC ) * 100.00 ;        // .. Circuit Transfered to Inverter                                                
              } 
                                              // No action
            } else if ( LineLoad [line] < 10.00 ) {
                Serial.println (" Circuit UnLoaded.. "); 
                  if ( relay_status [line] )  {                    // ICC > 5% & Circuit on inverter & Circuit loaded.. 
                Serial.println (" Circuit on inverter Bus, shall be Transferred to Utility Bus.. "); //        // ICC > 5% & Circuit on inverter & Circuit loaded..
                  TransferToUtilityBus (line) ;
                  InverterLoad  = findInverterLoad () ;// scanBus ( inverterPin, 200) ;                                             // No action needed. 
            } else if ( ! relay_status [line] ) {               // ICC > 5% & Circuit on utility..
                Serial.println (" Circuit on Utility Bus, ") ;
                Serial.println (" Operation skipped.. ") ;
                                            // .. Circuit Transfered to Inverter                                                
              } 
               // ICCP = ((IFC - (InverterLoad - LineLoad [line] ) )/ IFC ) * 100.00 ;                              // No action
            }

          } else if (ICC < 5.00 )  {                         // 
              Serial.println (" Inverter is overloaded.. ") ;
                if ( LineLoad [line] > 10.00 ) { 
                Serial.println (" Circuit loaded...  ") ; 
                  if ( relay_status [line] )  {
                    Serial.println (" Circuit on Iverter Bus...  ") ;
                    Serial.println (" Circuit shall be Transferred to Utility Bus ...  ") ;
                    TransferToUtilityBus (line) ;
                    InverterLoad  = findInverterLoad () ;// scanBus ( inverterPin, 200) ;
                  } else if (! relay_status [line]) {
                      Serial.println (" Circuit on Utility Bus.. Operation skipped..  ") ; 
                    } 
             
                } else if (  LineLoad [line] < 10.00 ) {
                  Serial.println (" Circuit running without load..  ") ;  
                    if ( relay_status [line] )  {
                      Serial.println (" Circuit on Iverter Bus!!!!  ") ;
                      Serial.println (" Circuit shall be Transferred to Utility Bus ...  ") ;
                      TransferToUtilityBus (line) ;
                      InverterLoad  = findInverterLoad () ;// scanBus ( inverterPin, 200) ;

                    } else if ( ! relay_status [line] ) {
                        Serial.println (" Circuit on Utility Bus.. Operation skipped..  ") ; 
                      }  
                  }
          }
        }
        cycle ++ ;
                    
    }     
        

// Function Definition Area // 
                                        /**********************/

              float ScanLineLoad (int line) { // Start of ScanLineLoad () Function //
                    float sample [100] ;
                    float temp ;
                    Serial.println ();                   
                  //.  Serial.println ("           * * * *             Circuit Scanning in progress, please wait       * * * * ");
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
        //. Serial.println (" Utility Bus ") ;
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
        //.  Serial.println (" Inverter Bus ") ;
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
              Serial.println ("            ...Scanning in progress, please wait ... "); Serial.println ();
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
               Serial.print ("  Peak Value =  \t"); Serial.println ( sample [0] ); 
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
      //.  Serial.println (" -- Circuit Transferred IN INV BUS -- ");
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
      //.  Serial.println (" -- Circuit Unloaded, Load Transferred to Utility Grid --  "); 
      //  Serial.print (" Inverter Load after Transfer OUT = "); Serial.println ( InverterLoad );
      //  Serial.print (" Inverter Captive Capacity (ICC) after Transfer OUT            = "); Serial.print (ICC, 2); Serial.println ("%");
      //.  Serial.print (" Relay Status = "); Serial.println ( relay_status [line] );
        return 0 ;
      } // End of TransferToUtilityBus () Function // 

                    