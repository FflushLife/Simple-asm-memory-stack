solve_linear:
mul bx -1
div bx ax
sub ax ax
add ax bx
ret
solve_quadro:
push ax
push bx
mul bx bx
mul ax 4
mul ax cx
sub bx ax
sqrt bx
sub cx cx
add cx bx
pop
sub bx bx
add bx dx
mul bx -1
pop
sub ax ax
add ax dx
mul ax 2
push ax
push bx
add bx cx
div bx ax
pop
sub ax ax
add ax dx
pop
push bx
sub bx bx
add bx dx
sub ax cx
div ax bx
pop
sub bx bx
add bx dx
ret
main:
add ax 2
add bx 6
call solve_linear
sub ax ax
sub bx bx
add ax 1
sub bx 3
add cx 2
call solve_quadro
