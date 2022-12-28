bits 64

;entry:
;    mov qword [rsp - 8], rsp
;    sub rsp, 8
;    call $+5
;    sub qword [rsp], 14
;    mov qword [rsp - 8], 12345678
;    sub rsp, 8
;    jmp 0x400000

; opcode_offset
; rip
; rsp

tranisition:






    sub rsp, 160 + 10240
    mov qword [rsp], rax
    mov qword [rsp + 8], rbx
    mov qword [rsp + 16], rcx
    mov qword [rsp + 24], rdx
    mov qword [rsp + 32], rsi
    mov qword [rsp + 40], rdi

    push qword [rsp + 160 + 10240 + 16]
    pop qword [rsp + 48]


    mov qword [rsp + 56], rbp
    mov qword [rsp + 64], r8
    mov qword [rsp + 72], r9
    mov qword [rsp + 80], r10
    mov qword [rsp + 88], r11
    mov qword [rsp + 96], r12
    mov qword [rsp + 104], r13
    mov qword [rsp + 112], r14
    mov qword [rsp + 120], r15
    push qword [rsp + 160 + 10240 + 8]
    pop qword [rsp + 128]
    mov qword [rsp + 136], 0
    mov qword [rsp + 144], 0
    mov qword [rsp + 152], 0

    mov rsi, rsp
    mov rax, qword [rsp + 160 + 10240]
    lea rdi, [rel vm_opcodes]
    add rdi, rax
    call vm_func

    mov rax, qword [rsp]
    mov rbx, qword [rsp + 8]
    mov rcx, qword [rsp + 16]
    mov rdx, qword [rsp + 24]
    mov rsi, qword [rsp + 32]
    mov rdi, qword [rsp + 40]
    
    mov rbp, qword [rsp + 56]
    mov r8, qword [rsp + 64]
    mov r9, qword [rsp + 72]
    mov r10, qword [rsp + 80]
    mov r11, qword [rsp + 88]
    mov r12, qword [rsp + 96]
    mov r13, qword [rsp + 104]
    mov r14, qword [rsp + 112]
    mov r15, qword [rsp + 120]

    mov rsp, qword [rsp + 48]
    sub rsp, 8
    ret


vm_func:
times 0x70C1 db 0

vm_opcodes:
dq 0
    


