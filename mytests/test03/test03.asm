; program to find max of array and store it in max label


; in loop first we get max and store in B then get array element store in A 
; subtract it , if<0 then branch to update the max( as B < A)

; now we check for the loop counter
; here logic is we subtract the address of element - addres of data
; which give the counter(number) upto which we have iterated
; then we subtract the count-max or count -10
; if the if subtract==0 it halts 


ldc array ; sp=array
a2sp

loop:       ldc max  ;in loop first we get max
            ldnl 0   ; B=A,A=mem(max)
            ldl 0    ; B=max, A=array[0]
            sub        ; A= A-B
            brlz updatemax  

            sp2a   ; loop counter check starts
            ldc size
            sub
            ldc size
            ldnl 0
            sub
            brz halt

return:     adj 1
            br loop

updatemax:  ldl 0      ; here it updates the max variable
            ldc max
            stnl 0
            br return

halt:       HALT

; data here

max: data 0
size: data 10
array: data 34
       data 2
       data 12
       data 0
       data 86
       data -2
       data 84
       data 1234
       data 3
       data 4

