             CMD PROMPT('CREATE PF FROM CSV FILE')                
                                                                  
             PARM       KWD(OUTFILE) TYPE(*CHAR) LEN(10) MIN(1) + 
                          PROMPT('The output pf name')            
             PARM       KWD(INPATH)  TYPE(*CHAR) LEN(128) MIN(1) +
                          PROMPT('The path to the csv file')      
