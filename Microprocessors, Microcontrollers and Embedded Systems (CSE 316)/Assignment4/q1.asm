.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    LINE_GAP DB CR, LF, '$'
    INPUT_1_MSG DB 'Enter matrix 1 elements (2x2): $'
    INPUT_2_MSG DB CR, LF, 'Enter matrix 2 elements (2x2): $'
    RESULT_MSG DB CR, LF, 'The result of the addition is:', CR, LF, '$'
    
    MATRIX_1 DB 4 DUP (?)
    MATRIX_2 DB 4 DUP (?)
    RESULT DB 4 DUP (?)

.CODE
MAIN PROC
    ;initialize DS
    MOV AX, @DATA
    MOV DS, AX

    ;input matrix 1
    LEA DX, INPUT_1_MSG
    MOV AH, 9
    INT 21H  
    MOV AH, 1
    MOV BX, 0
    MOV CX, 2
    ROW_LOOP:
        PUSH CX
        MOV CX, 2
        MOV SI, 0
        COLUMN_LOOP:
            INT 21H
            SUB AL, 30H
            MOV MATRIX_1[BX][SI], AL
            INC SI
            LOOP COLUMN_LOOP
        ADD BX, 2
        POP CX
        LOOP ROW_LOOP   
    
    ;input matrix 2
    LEA DX, INPUT_2_MSG
    MOV AH, 9
    INT 21H
    MOV AH, 1
    MOV BX, 0
    MOV CX, 2
    ROW_LOOP1:
        PUSH CX
        MOV CX, 2
        MOV SI, 0
        COLUMN_LOOP1:
            INT 21H
            SUB AL, 30H
            MOV MATRIX_2[BX][SI], AL
            INC SI
            LOOP COLUMN_LOOP1
        ADD BX, 2
        POP CX
        LOOP ROW_LOOP1
        
    ;addition
    LEA DX, RESULT_MSG
    MOV AH, 9
    INT 21H
    MOV BX, 0
    MOV CX, 2
    ROW_LOOP2:
        PUSH CX
        MOV CX, 2
        MOV SI, 0
        COLUMN_LOOP2:
            MOV AL, MATRIX_1[BX][SI]
            ADD AL, MATRIX_2[BX][SI]
            MOV RESULT[BX][SI], AL
            CALL PRINT_NUMBER
            INC SI
            LOOP COLUMN_LOOP2
        ADD BX, 2
        POP CX
        LEA DX, LINE_GAP
        MOV AH, 9
        INT 21H 
        LOOP ROW_LOOP2
   
    ;dos exit
    MOV AH, 4CH
    INT 21H  
MAIN ENDP

PRINT_NUMBER PROC
    ;save registers
    PUSH BX
    PUSH CX
    
    ;push digits to stack
    MOV DH, 0 ;for storing remainder in DX during while loop
    MOV BL, 10
    MOV CX, 0
    WHILE1:
        CMP AL, 0
        JE END_WHILE1
        MOV AH, 0
        DIV BL
        MOV DL, AH
        PUSH DX
        INC CX
        JMP WHILE1
    END_WHILE1:
    
    MOV AH, 2  
    ;pop digits from stack and print
    TOP1:
        POP DX
        ADD DL, 30H
        INT 21H
        LOOP TOP1    
    MOV DL, ' '
    INT 21H
    
    ;retreive registers
    POP CX
    POP BX
    RET
PRINT_NUMBER ENDP

END MAIN