; asm program to sum two integers



ldc  a      ; acc=(address of a) ,base=0(previous acc)
ldnl 0        ; acc= 10
ldc  b        ; acc=(address of b) , base=10(previous acc)
ldnl 0        ; acc= 20
add           ; acc= acc+base (10 +20)
ldc  result   ; acc=(address of result),base=30(prev accumulator)
stnl 0        ; mem[acc+0]= base
HALT          ; program to halt before memory block

a:	data 10
b:	data 20
result: data 0