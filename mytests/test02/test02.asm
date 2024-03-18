; this file mainly focused on error checking capability of assembler

ldc 5
ldnl 45
a:
call a    ; random correct instructions

ldc 8hg ; not a number
ldc ; missing operand
sub 3 ; unexpected operand
ldc 5,,5; invalid operand
84kd: bogus labelname
donut; bogus mnemonic
hola ; bogus mnemonic
