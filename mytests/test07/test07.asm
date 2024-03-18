; this file focused on checking that all three decimal,hex and octal numbers are accepted 
; also error in end in the number

ldc 394
ldc 0100
ldc 0x83f4e

label: SET 0x32

data 034
data 0x74fe

ldl 0xghijkl ; invalid hex here
