
.global main
.global printf
 
.data 
 
    list1:
        .asciz "%d"
    list2:
        .asciz "\n"
 
.text
main:
        mov r3,#1
        mov r2,#0
        mov r1,#0
	push {r1,r2,r3,lr}
	bl init
	pop {r1,r2,r3,lr}
 
add1:
	orr r1,r1,r3
        push {r1,r2,r3,lr}
	bl init
	pop {r1,r2,r3,lr}
	lsl r1,#1
	add r2,#1
	cmp r2,#16
	beq break
	b add1
	
break:
	mov r2,#0
	mov r3,#32767
	mov r1,#65535
	
loop:
	and r1,r1,r3
	push {r1,r2,r3,lr}
	bl init
	pop {r1,r2,r3,lr}
	ror r3,#01
	add r2,#01
	cmp r2,#16
	beq over
	b loop
init:
    mov r3,#32768
    mov r4,#0
    mov r2,#0
    b print
   
print:
    and r2,r1,r3
    cmp r2,#0
    push {r1,r2,r3,r4,lr}
    bleq set_bit0
    blgt set_bit1
    pop {r1,r2,r3,r4,lr}
    ror r3,#01
    add r4,#01
    cmp r4,#16
    beq exit
    b print
 


set_bit0:
    ldr r0,=list1 
    mov r1,#0
    push {lr}
    bl printf
    pop {lr}
    bx lr
 
set_bit1:
    ldr r0,=list1
    mov r1,#1
    push {lr}
    bl printf
    pop {lr}
    bx lr 
 
exit:
    ldr r0,=list2
    push {lr}
    bl printf
    pop {lr}
    bx lr
over:
    swi 0
