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
in ax
in bx
in cx
call solve_quadro
