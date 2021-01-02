Motorola 68000 instruction disassembly specification

  operand categories
  ---+--------------------------------------------
   @ | effective address
   D | data register only
   A | address register only
   + | postincrement only
   - | predecrement only
   d | address register indirect with displacement
   B | program counter relative with displacement
   # | immediate data as byte, word or long
   M | 8-bit immediate data in -128..127 range
   Q | 3-bit immediate data in 1..8 range
   S | stack pointer register
   U | user stack pointer register
   C | condition code register
   l | register list, reversed bits
   L | register list
   T | 4-bit trap vector


  effective address categories
  -------------+------------------------
  daA+-DXWLpxI | all
  ..A+-DXWL... | alterable memory
  daA+-DXWL... | alterable
  d.A+-DXWLpxI | data
  d.A+-DXWL... | alterable data
  d.A+-DXWLpx. | data, but not immediate
  ..A..DXWLpx. | control
  ..A..DXWL... | alterable control
  ..A+.DXWLpx. | restore operands


  effective address modes and registers
  --+---+------+---+------------+---------------------------------------------
  m | r | op   | v | syntax     | operand
  --+---+------+---+------------+---------------------------------------------
  0 | n | d    | d | Dn         | data register
  1 | n | a    | a | An         | address register
  2 | n | ai   | A | (An)       | address register indirect
  3 | n | pi   | + | (An)+      | address register indirect with postincrement
  4 | n | pd   | - | -(An)      | address register indirect with predecrement
  5 | n | di   | D | (d16,An)   | address register indirect with displacement
  6 | n | ix   | X | (d8,An,Xk) | address register indirect with index
  7 | 0 | aw   | W | (xxx).w    | absolute word address
  7 | 1 | al   | L | (xxx).l    | absolute long address
  7 | 2 | pcdi | p | (d16,PC)   | program counter relative with displacement
  7 | 3 | pcix | x | (d8,PC,Xk) | program counter relative with index
  7 | 4 | imm  | I | #imm       | immediate data


  operand placements
  ---+-----------------------------------------------
   b | second word, low byte
   B | first word, low byte, for branch displacements
   w | second word (entire), for branch displacements
   l | second and third word (entire)
   s | source register, low bits of first word
   d | destination register, shifted 9 in first word


  valid operands
  ---+------------------
     |  b  B  w  l  s  d
  ---+------------------
   @ | @b .. @w @l @s @d
   D | .. .. .. .. Ds Dd
   A | .. .. .. .. As Ad
   + | .. .. .. .. +s +d
   - | .. .. .. .. -s -d
   d | .. .. .. .. ds ..
   B | .. BB Bw .. .. ..
   # | #b .. #w #l .. ..
   M | .. .. .. .. Ms ..
   Q | .. .. .. .. .. Qd
   S | .. .. .. .. Ss Sd
   U | .. .. .. .. .. Ud
   C | .. .. .. .. Cs Cd
   l | .. .. lw .. .. ..
   L | .. .. Lw .. .. ..
   T | .. .. .. .. Ts ..


          operation word      valid ea    operand
mnemonic  bit pattern       daA+-DXWLpxI   0   1
========  ================  ============  === ===
abcd      1100...100000...  ............  Ds  Dd
abcd      1100...100001...  ............  -s  -d
adda.w    1101...011......  daA+-DXWLpxI  @w  Ad
adda.l    1101...111......  daA+-DXWLpxI  @l  Ad
addi.b    0000011000......  d.A+-DXWL...  #b  @s
addi.w    0000011001......  d.A+-DXWL...  #w  @s
addi.l    0000011010......  d.A+-DXWL...  #l  @s
addq.b    0101...000......  d.A+-DXWL...  Qd  @b
addq.w    0101...001......  daA+-DXWL...  Qd  @w
addq.l    0101...010......  daA+-DXWL...  Qd  @l
add.b     1101...000......  d.A+-DXWLpxI  @b  Dd
add.b     1101...100......  ..A+-DXWL...  Dd  @b
add.w     1101...001......  daA+-DXWLpxI  @w  Dd
add.w     1101...101......  ..A+-DXWL...  Dd  @w
add.l     1101...010......  daA+-DXWLpxI  @l  Dd
add.l     1101...110......  ..A+-DXWL...  Dd  @l
addx.b    1101...100000...  ............  Ds  Dd
addx.b    1101...100001...  ............  -s  -d
addx.w    1101...101000...  ............  Ds  Dd
addx.w    1101...101001...  ............  -s  -d
addx.l    1101...110000...  ............  Ds  Dd
addx.l    1101...110001...  ............  -s  -d
andi.b    0000001000......  d.A+-DXWL...  #b  @s
andi.b    0000001000111100  ............  #b  Cs
andi.w    0000001001......  d.A+-DXWL...  #w  @s
andi.w    0000001001111100  ............  #w  Ss
andi.l    0000001010......  d.A+-DXWL...  #l  @s
and.b     1100...000......  d.A+-DXWLpxI  @b  Dd
and.b     1100...100......  ..A+-DXWL...  Dd  @b
and.w     1100...001......  d.A+-DXWLpxI  @w  Dd
and.w     1100...101......  ..A+-DXWL...  Dd  @w
and.l     1100...010......  d.A+-DXWLpxI  @l  Dd
and.l     1100...110......  ..A+-DXWL...  Dd  @l
asl.b     1110...100000...  ............  Qd  Ds
asl.b     1110...100100...  ............  Dd  Ds
asl.w     1110...101000...  ............  Qd  Ds
asl.w     1110...101100...  ............  Dd  Ds
asl.w     1110000111......  ..A+-DXWL...  @s  ..
asl.l     1110...110000...  ............  Qd  Ds
asl.l     1110...110100...  ............  Dd  Ds
asr.b     1110...000000...  ............  Qd  Ds
asr.b     1110...000100...  ............  Dd  Ds
asr.w     1110...001000...  ............  Qd  Ds
asr.w     1110...001100...  ............  Dd  Ds
asr.w     1110000011......  ..A+-DXWL...  @s  ..
asr.l     1110...010000...  ............  Qd  Ds
asr.l     1110...010100...  ............  Dd  Ds
bhi.w     0110001000000000  ............  Bw  ..
bls.w     0110001100000000  ............  Bw  ..
bcc.w     0110010000000000  ............  Bw  ..
bcs.w     0110010100000000  ............  Bw  ..
bne.w     0110011000000000  ............  Bw  ..
beq.w     0110011100000000  ............  Bw  ..
bvc.w     0110100000000000  ............  Bw  ..
bvs.w     0110100100000000  ............  Bw  ..
bpl.w     0110101000000000  ............  Bw  ..
bmi.w     0110101100000000  ............  Bw  ..
bge.w     0110110000000000  ............  Bw  ..
blt.w     0110110100000000  ............  Bw  ..
bgt.w     0110111000000000  ............  Bw  ..
ble.w     0110111100000000  ............  Bw  ..
bhi.s     01100010........  ............  BB  ..
bls.s     01100011........  ............  BB  ..
bcc.s     01100100........  ............  BB  ..
bcs.s     01100101........  ............  BB  ..
bne.s     01100110........  ............  BB  ..
beq.s     01100111........  ............  BB  ..
bvc.s     01101000........  ............  BB  ..
bvs.s     01101001........  ............  BB  ..
bpl.s     01101010........  ............  BB  ..
bmi.s     01101011........  ............  BB  ..
bge.s     01101100........  ............  BB  ..
blt.s     01101101........  ............  BB  ..
bgt.s     01101110........  ............  BB  ..
ble.s     01101111........  ............  BB  ..
bchg      0000...101......  d.A+-DXWL...  Dd  @s
bchg      0000100001......  d.A+-DXWL...  #b  @s
bclr      0000...110......  d.A+-DXWL...  Dd  @s
bclr      0000100010......  d.A+-DXWL...  #b  @s
bra.w     0110000000000000  ............  Bw  ..
bra.s     01100000........  ............  BB  ..
bset      0000...111......  d.A+-DXWL...  Dd  @s
bset      0000100011......  d.A+-DXWL...  #b  @s
bsr.w     0110000100000000  ............  Bw  ..
bsr.s     01100001........  ............  BB  ..
btst      0000...100......  d.A+-DXWLpxI  Dd  @b
btst      0000100000......  d.A+-DXWLpx.  #b  @s
chk.w     0100...110......  d.A+-DXWLpxI  @w  Dd
clr.b     0100001000......  d.A+-DXWL...  @s  ..
clr.w     0100001001......  d.A+-DXWL...  @s  ..
clr.l     0100001010......  d.A+-DXWL...  @s  ..
cmpa.w    1011...011......  daA+-DXWLpxI  @w  Ad
cmpa.l    1011...111......  daA+-DXWLpxI  @l  Ad
cmpi.b    0000110000......  d.A+-DXWLpx.  #b  @s
cmpi.w    0000110001......  d.A+-DXWLpx.  #w  @s
cmpi.l    0000110010......  d.A+-DXWLpx.  #l  @s
cmpm.b    1011...100001...  ............  +s  +d
cmpm.w    1011...101001...  ............  +s  +d
cmpm.l    1011...110001...  ............  +s  +d
cmp.b     1011...000......  d.A+-DXWLpxI  @b  Dd
cmp.w     1011...001......  daA+-DXWLpxI  @w  Dd
cmp.l     1011...010......  daA+-DXWLpxI  @l  Dd
dbcc      0101010011001...  ............  Ds  Bw
dbcs      0101010111001...  ............  Ds  Bw
dbeq      0101011111001...  ............  Ds  Bw
dbf       0101000111001...  ............  Ds  Bw
dbge      0101110011001...  ............  Ds  Bw
dbgt      0101111011001...  ............  Ds  Bw
dbhi      0101001011001...  ............  Ds  Bw
dble      0101111111001...  ............  Ds  Bw
dbls      0101001111001...  ............  Ds  Bw
dblt      0101110111001...  ............  Ds  Bw
dbmi      0101101111001...  ............  Ds  Bw
dbne      0101011011001...  ............  Ds  Bw
dbpl      0101101011001...  ............  Ds  Bw
dbt       0101000011001...  ............  Ds  Bw
dbvc      0101100011001...  ............  Ds  Bw
dbvs      0101100111001...  ............  Ds  Bw
divu.w    1000...011......  d.A+-DXWLpxI  @w  Dd
eori.b    0000101000......  d.A+-DXWL...  #b  @s
eori.b    0000101000111100  ............  #b  Cs
eori.w    0000101001......  d.A+-DXWL...  #w  @s
eori.w    0000101001111100  ............  #w  Ss
eori.l    0000101010......  d.A+-DXWL...  #l  @s
eor.b     1011...100......  d.A+-DXWL...  Dd  @s
eor.w     1011...101......  d.A+-DXWL...  Dd  @s
eor.l     1011...110......  d.A+-DXWL...  Dd  @s
exg       1100...101000...  ............  Dd  Ds
exg       1100...101001...  ............  Ad  As
exg       1100...110001...  ............  Dd  As
ext.w     0100100010000...  ............  Ds  ..
ext.l     0100100011000...  ............  Ds  ..
illegal   0100101011111100  ............  ..  ..
jmp       0100111011......  ..A..DXWLpx.  @s  ..
jsr       0100111010......  ..A..DXWLpx.  @s  ..
lea       0100...111......  ..A..DXWLpx.  @s  Ad
link.w    0100111001010...  ............  As  #w
lsl.b     1110...100001...  ............  Qd  Ds
lsl.b     1110...100101...  ............  Dd  Ds
lsl.w     1110...101001...  ............  Qd  Ds
lsl.w     1110...101101...  ............  Dd  Ds
lsl.w     1110001111......  ..A+-DXWL...  @s  ..
lsl.l     1110...110001...  ............  Qd  Ds
lsl.l     1110...110101...  ............  Dd  Ds
lsr.b     1110...000001...  ............  Qd  Ds
lsr.b     1110...000101...  ............  Dd  Ds
lsr.w     1110...001001...  ............  Qd  Ds
lsr.w     1110...001101...  ............  Dd  Ds
lsr.w     1110001011......  ..A+-DXWL...  @s  ..
lsr.l     1110...010001...  ............  Qd  Ds
lsr.l     1110...010101...  ............  Dd  Ds
movea.l   0010...001......  daA+-DXWLpxI  @l  Ad
movea.w   0011...001......  daA+-DXWLpxI  @w  Ad
movem.w   0100100010......  ..A..DXWL...  Lw  @s
movem.w   0100100010100...  ............  lw  -s
movem.w   0100110010......  ..A+.DXWLpx.  @s  Lw
movem.l   0100100011......  ..A..DXWL...  Lw  @s
movem.l   0100100011100...  ............  lw  -s
movem.l   0100110011......  ..A+.DXWLpx.  @s  Lw
movep.w   0000...100001...  ............  ds  Dd
movep.w   0000...110001...  ............  Dd  ds
movep.l   0000...101001...  ............  ds  Dd
movep.l   0000...111001...  ............  Dd  ds
moveq     0111...0........  ............  Ms  Dd
move.b    0001...000......  d.A+-DXWLpxI  @b  @d
move.b    0001...010......  d.A+-DXWLpxI  @b  @d
move.b    0001...011......  d.A+-DXWLpxI  @b  @d
move.b    0001...100......  d.A+-DXWLpxI  @b  @d
move.b    0001...101......  d.A+-DXWLpxI  @b  @d
move.b    0001...110......  d.A+-DXWLpxI  @b  @d
move.b    0001000111......  d.A+-DXWLpxI  @b  @d
move.b    0001001111......  d.A+-DXWLpxI  @b  @d
move.w    0011...000......  daA+-DXWLpxI  @w  @d
move.w    0011...010......  daA+-DXWLpxI  @w  @d
move.w    0011...011......  daA+-DXWLpxI  @w  @d
move.w    0011...100......  daA+-DXWLpxI  @w  @d
move.w    0011...101......  daA+-DXWLpxI  @w  @d
move.w    0011...110......  daA+-DXWLpxI  @w  @d
move.w    0011000111......  daA+-DXWLpxI  @w  @d
move.w    0011001111......  daA+-DXWLpxI  @w  @d
move.w    0100000011......  d.A+-DXWL...  Ss  @s
move.w    0100010011......  d.A+-DXWLpxI  @w  Cd
move.w    0100011011......  d.A+-DXWLpxI  @w  Sd
move.l    0010...000......  daA+-DXWLpxI  @l  @d
move.l    0010...010......  daA+-DXWLpxI  @l  @d
move.l    0010...011......  daA+-DXWLpxI  @l  @d
move.l    0010...100......  daA+-DXWLpxI  @l  @d
move.l    0010...101......  daA+-DXWLpxI  @l  @d
move.l    0010...110......  daA+-DXWLpxI  @l  @d
move.l    0010000111......  daA+-DXWLpxI  @l  @d
move.l    0010001111......  daA+-DXWLpxI  @l  @d
move.l    0100111001100...  ............  As  Ud
move.l    0100111001101...  ............  Ud  As
muls.w    1100...111......  d.A+-DXWLpxI  @w  Dd
mulu.w    1100...011......  d.A+-DXWLpxI  @w  Dd
nbcd      0100100000......  d.A+-DXWL...  @s  ..
neg.b     0100010000......  d.A+-DXWL...  @s  ..
neg.w     0100010001......  d.A+-DXWL...  @s  ..
neg.l     0100010010......  d.A+-DXWL...  @s  ..
negx.b    0100000000......  d.A+-DXWL...  @s  ..
negx.w    0100000001......  d.A+-DXWL...  @s  ..
negx.l    0100000010......  d.A+-DXWL...  @s  ..
nop       0100111001110001  ............  ..  ..
not.b     0100011000......  d.A+-DXWL...  @s  ..
not.w     0100011001......  d.A+-DXWL...  @s  ..
not.l     0100011010......  d.A+-DXWL...  @s  ..
ori.b     0000000000......  d.A+-DXWL...  #b  @s
ori.b     0000000000111100  ............  #b  Cs
ori.w     0000000001......  d.A+-DXWL...  #w  @s
ori.w     0000000001111100  ............  #w  Ss
ori.l     0000000010......  d.A+-DXWL...  #l  @s
or.b      1000...000......  d.A+-DXWLpxI  @b  Dd
or.b      1000...100......  ..A+-DXWL...  Dd  @s
or.w      1000...001......  d.A+-DXWLpxI  @w  Dd
or.w      1000...101......  ..A+-DXWL...  Dd  @s
or.l      1000...010......  d.A+-DXWLpxI  @l  Dd
or.l      1000...110......  ..A+-DXWL...  Dd  @s
pea       0100100001......  ..A..DXWLpx.  @s  ..
reset     0100111001110000  ............  ..  ..
rol.b     1110...100011...  ............  Qd  Ds
rol.b     1110...100111...  ............  Dd  Ds
rol.w     1110...101011...  ............  Qd  Ds
rol.w     1110...101111...  ............  Dd  Ds
rol.w     1110011111......  ..A+-DXWL...  @s  ..
rol.l     1110...110011...  ............  Qd  Ds
rol.l     1110...110111...  ............  Dd  Ds
ror.b     1110...000011...  ............  Qd  Ds
ror.b     1110...000111...  ............  Dd  Ds
ror.w     1110...001011...  ............  Qd  Ds
ror.w     1110...001111...  ............  Dd  Ds
ror.w     1110011011......  ..A+-DXWL...  @s  ..
ror.l     1110...010011...  ............  Qd  Ds
ror.l     1110...010111...  ............  Dd  Ds
roxl.b    1110...100010...  ............  Qd  Ds
roxl.b    1110...100110...  ............  Dd  Ds
roxl.w    1110...101010...  ............  Qd  Ds
roxl.w    1110...101110...  ............  Dd  Ds
roxl.w    1110010111......  ..A+-DXWL...  @s  ..
roxl.l    1110...110010...  ............  Qd  Ds
roxl.l    1110...110110...  ............  Dd  Ds
roxr.b    1110...000010...  ............  Qd  Ds
roxr.b    1110...000110...  ............  Dd  Ds
roxr.w    1110...001010...  ............  Qd  Ds
roxr.w    1110...001110...  ............  Dd  Ds
roxr.w    1110010011......  ..A+-DXWL...  @s  ..
roxr.l    1110...010010...  ............  Qd  Ds
roxr.l    1110...010110...  ............  Dd  Ds
rte       0100111001110011  ............  ..  ..
rtr       0100111001110111  ............  ..  ..
rts       0100111001110101  ............  ..  ..
sbcd      1000...100000...  ............  Ds  Dd
sbcd      1000...100001...  ............  -s  -d
scc       0101010011......  d.A+-DXWL...  @s  ..
scs       0101010111......  d.A+-DXWL...  @s  ..
seq       0101011111......  d.A+-DXWL...  @s  ..
sf        0101000111......  d.A+-DXWL...  @s  ..
sge       0101110011......  d.A+-DXWL...  @s  ..
sgt       0101111011......  d.A+-DXWL...  @s  ..
shi       0101001011......  d.A+-DXWL...  @s  ..
sle       0101111111......  d.A+-DXWL...  @s  ..
sls       0101001111......  d.A+-DXWL...  @s  ..
slt       0101110111......  d.A+-DXWL...  @s  ..
smi       0101101111......  d.A+-DXWL...  @s  ..
sne       0101011011......  d.A+-DXWL...  @s  ..
spl       0101101011......  d.A+-DXWL...  @s  ..
st        0101000011......  d.A+-DXWL...  @s  ..
svc       0101100011......  d.A+-DXWL...  @s  ..
svs       0101100111......  d.A+-DXWL...  @s  ..
stop      0100111001110010  ............  #w  ..
suba.l    1001...111......  daA+-DXWLpxI  @l  Ad
suba.w    1001...011......  daA+-DXWLpxI  @w  Ad
subi.b    0000010000......  d.A+-DXWL...  #b  @s
subi.w    0000010001......  d.A+-DXWL...  #w  @s
subi.l    0000010010......  d.A+-DXWL...  #l  @s
subq.b    0101...100......  daA+-DXWL...  Qd  @s
subq.w    0101...101......  daA+-DXWL...  Qd  @s
subq.l    0101...110......  daA+-DXWL...  Qd  @s
sub.b     1001...000......  d.A+-DXWLpxI  @b  Dd
sub.b     1001...100......  ..A+-DXWL...  Dd  @s
sub.w     1001...001......  daA+-DXWLpxI  @w  Dd
sub.w     1001...101......  ..A+-DXWL...  Dd  @s
sub.l     1001...010......  daA+-DXWLpxI  @l  Dd
sub.l     1001...110......  ..A+-DXWL...  Dd  @s
subx.b    1001...100000...  ............  Ds  Dd
subx.b    1001...100001...  ............  -s  -d
subx.w    1001...101000...  ............  Ds  Dd
subx.w    1001...101001...  ............  -s  -d
subx.l    1001...110000...  ............  Ds  Dd
subx.l    1001...110001...  ............  -s  -d
swap      0100100001000...  ............  Ds  ..
tas       0100101011......  d.A+-DXWL...  @s  ..
trap      010011100100....  ............  Ts  ..
trapv     0100111001110110  ............  ..  ..
tst.b     0100101000......  d.A+-DXWL...  @b  ..
tst.w     0100101001......  d.A+-DXWL...  @w  ..
tst.l     0100101010......  d.A+-DXWL...  @l  ..
unlk      0100111001011...  ............  As  ..
