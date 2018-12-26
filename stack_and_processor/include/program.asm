point:
mul cx bx
sub bx 1
jne point
ret

main:
fluffy_bastard
in bx

         add cx 1


  call point
add ax cx
fluffy_bastard
sub [cx] [cx]
add [cx] 15
add bx [cx]
