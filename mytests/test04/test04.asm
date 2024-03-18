; program to find the sum of array

ldc array
a2sp

loop:   ldc SUM
        ldnl 0
        ldl 0  
        add    ; summing the elements, logic taken from test01
        ldc SUM
        stnl 0
        
        sp2a   ; loop counter check starts,logic taken from test05
        ldc size
        sub
        ldc size
        ldnl 0
        sub
        brz halt
        
        adj 1
        br loop

halt:   HALT

; data here

SUM: data 0
size: data 10
array: data 1
       data 2
       data 3
       data 4
       data 5
       data 6
       data 7
       data 8
       data 9
       data -9
