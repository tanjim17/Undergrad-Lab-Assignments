.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    INPUT_OPERAND_1_MSG DB 'Enter operand 1: $'
    INPUT_OPERAND_2_MSG DB CR, LF, 'Enter operand 2: $'
    INPUT_OPERATOR_MSG DB CR, LF, 'Enter operator: $'
    PRINT_OPERATION_MSG DB CR, LF, 'The given operation is: $'
    ERROR_MSG DB CR, LF, 'Wrong operator$'
    
    OPERAND DW ?
    OPERAND_1 DW ?
    OPERAND_2 DW ?
    OPERATOR DB ?

.CODE

MAIN PROC
;initialize DS
    MOV AX, @DATA
    MOV DS, AX

;input operand 1
    LEA DX, INPUT_OPERAND_1_MSG
    CALL INPUT_OPERAND
    MOV AX, OPERAND
    MOV OPERAND_1, AX
    
;input operator
    LEA DX, INPUT_OPERATOR_MSG
    MOV AH, 9
    INT 21H
    MOV AH, 1
    INT 21H
    ;if input operator is valid
        CMP AL, '+'
        JE THEN
        CMP AL, '-'
        JE THEN
        CMP AL, '*'
        JE THEN
        CMP AL, '/'
        JE THEN
        JMP ELSE
    THEN:
        MOV OPERATOR, AL
        JMP END_IF1
    ELSE:
        ;if wrong command
            CMP AL, 'q'
            JE END_IF2
        ;then
            LEA DX, ERROR_MSG
            MOV AH, 9
            INT 21H
        END_IF2:
        JMP DOS_EXIT
    END_IF1:
    
;input operand 2
    LEA DX, INPUT_OPERAND_2_MSG
    CALL INPUT_OPERAND
    MOV AX, OPERAND
    MOV OPERAND_2, AX
                      
;print given operation
    LEA DX, PRINT_OPERATION_MSG
    MOV AH, 9
    INT 21H
    MOV AX, OPERAND_1                  
    CALL PRINT_OPERAND 
    
    MOV AH, 2
    MOV DL, ' '
    INT 21H
    MOV DL, OPERATOR
    INT 21H
    MOV DL, ' '
    INT 21H
    
    MOV AX, OPERAND_2
    CALL PRINT_OPERAND
   
DOS_EXIT:
    MOV AH, 4CH
    INT 21H  
MAIN ENDP

INPUT_OPERAND PROC
    MOV AH, 9
    INT 21H  

;input loop
    MOV CX, 0
    MOV AH, 1   
    INT 21H
    WHILE:
        CMP AL, 0DH
        JE END_WHILE 
        ;if input is digit
            CMP AL, 30H
            JL END_IF
            CMP AL, 39H
            JG END_IF
        ;then
            SUB AL, 30H
            MOV AH, 0
            PUSH AX
            MOV AH, 1
            INC CX
        END_IF:
        INT 21H
        JMP WHILE
    END_WHILE:
    
;construct number from digits
    MOV OPERAND, 0   
    MOV BX, 1
    TOP:
        POP AX
        MUL BX
        ADD OPERAND, AX
        MOV AX, 10
        MUL BX
        MOV BX, AX
        LOOP TOP
        
    RET
INPUT_OPERAND ENDP

PRINT_OPERAND PROC
;push digits to stack
    MOV BX, 10
    MOV CX, 0
    WHILE1:
        CMP AX, 0
        JE END_WHILE1
        MOV DX, 0
        DIV BX
        PUSH DX
        INC CX
        JMP WHILE1
    END_WHILE1:

;pop digits from stack and print
    TOP1:
        POP DX
        ADD DL, 30H
        MOV AH, 2
        INT 21H
        LOOP TOP1
        
    RET
PRINT_OPERAND ENDP

END MAIN