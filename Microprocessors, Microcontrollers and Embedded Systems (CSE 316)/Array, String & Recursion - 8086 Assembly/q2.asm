.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    BS EQU 08H
    
    SEPARATOR DB ', $'
    LINE_GAP DB CR, LF, '$'
    CLEAR DB BS, BS, ' $'
    INPUT_MSG DB 'Enter number of terms in Fibonacci sequence(enter two digits): $'
    
    NUM_OF_TERMS DW ?
    FIBONACCI_TERMS DW 99 DUP (-1)

.CODE
MAIN PROC
    ;initialize DS
    MOV AX, @DATA
    MOV DS, AX

    ;input
    CALL INPUT_NUMBER
    
    ;generate Fibonacci sequence
    MOV AX, NUM_OF_TERMS
    DEC AX
    MOV BX, 2
    MUL BX
    PUSH AX ;pushing the location of nth term in FIBONACCI_TERMS array i.e. (n-1)*2  
    CALL GENERATE_FIBONACCI
    
    ;print fibonacci sequence
    CALL PRINT_FIBONACCI
   
    ;dos exit
    MOV AH, 4CH
    INT 21H  
MAIN ENDP

INPUT_NUMBER PROC
    ;show prompt
    LEA DX, INPUT_MSG
    MOV AH, 9
    INT 21H
        
    ;input loop
    MOV AH, 1
    MOV CX, 2
    INPUT:
        INT 21H   
        SUB AL, 30H
        MOV AH, 0
        PUSH AX
        MOV AH, 1
        LOOP INPUT
    
    ;pop digits from stack & construct the number
    MOV NUM_OF_TERMS, 0   
    MOV BX, 1
    MOV CX, 2
    TOP:
        POP AX
        MUL BX
        ADD NUM_OF_TERMS, AX
        MOV AX, 10
        MUL BX
        MOV BX, AX
        LOOP TOP
    RET
INPUT_NUMBER ENDP

GENERATE_FIBONACCI PROC
    ;save previous value of BP
    PUSH BP
    MOV BP,SP
    
    ;if already computed
    MOV BX, [BP+4]
    CMP FIBONACCI_TERMS[BX], -1
    JNE RETURN
    
    ;if n <= 2
        CMP WORD PTR[BP + 4], 2 ;2nd term's location = (2-1)*2 = 2
        JG ELSE
    ;then
        MOV FIBONACCI_TERMS[0], 0 ;1st term is 0
        ;if n = 2
            CMP WORD PTR[BP + 4], 2
            JNE END_IF
        ;then
            MOV FIBONACCI_TERMS[2], 1 ;2nd term is 1    
        END_IF:
        JMP END_IF2             
    ELSE:
        MOV CX, [BP+4]
        SUB CX, 2
        PUSH CX
        CALL GENERATE_FIBONACCI ;fibonacci(n-1)
        MOV CX, [BP+4]
        SUB CX, 4
        PUSH CX
        CALL GENERATE_FIBONACCI ;fibonacci(n-2)
        
        ;fibonacci(n) = fibonacci(n-1) + fibonacci(n-2)
        MOV BX, [BP + 4]
        MOV SI, BX
        SUB SI, 2
        MOV DX, FIBONACCI_TERMS[SI]
        SUB SI, 2
        ADD DX, FIBONACCI_TERMS[SI]
        MOV FIBONACCI_TERMS[BX], DX
    END_IF2:
          
    RETURN:     
        POP BP
        RET 2
    RET
GENERATE_FIBONACCI ENDP

PRINT_FIBONACCI PROC
    LEA DX, LINE_GAP
    MOV AH, 9
    INT 21H
    MOV CX, NUM_OF_TERMS
    MOV BX, 0
    PRINT:
        MOV AX, FIBONACCI_TERMS[BX]
        CALL PRINT_NUMBER
        ADD BX, 2
        LOOP PRINT
    LEA DX, CLEAR
    MOV AH, 9
    INT 21H
    RET
PRINT_FIBONACCI ENDP

PRINT_NUMBER PROC
    ;save registers
    PUSH BX
    PUSH CX
    
    ;push digits to stack
    ;if number is zero
        CMP AX, 0
        JNE ELSE1
    ;then
        PUSH 0
        MOV CX, 1
        JMP END_IF1
    ELSE1:     
        MOV BX, 10
        MOV CX, 0
        WHILE1:
            CMP AX, 0
            JE END_WHILE1
            CWD
            DIV BX
            PUSH DX
            INC CX
            JMP WHILE1
        END_WHILE1:
    END_IF1:
    
    ;pop digits from stack and print
    MOV AH, 2  
    TOP1:
        POP DX
        ADD DL, 30H
        INT 21H
        LOOP TOP1        
    LEA DX, SEPARATOR
    MOV AH, 9
    INT 21H
    
    ;retreive registers
    POP CX
    POP BX
    RET
PRINT_NUMBER ENDP

END MAIN