.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    LINE_GAP DB CR, LF, '$'
    INPUT_MSG DB 'Enter number of terms in Fibonacci sequence(enter two digits) : $'
    
    OPERAND DW ?
    OPERAND_1 DW ?
    OPERAND_2 DW ?
    RESULT DW ?
    OPERATOR DB ?    
    IS_NEGATIVE DB ?

.CODE
MAIN PROC
    ;initialize DS
    MOV AX, @DATA
    MOV DS, AX

    ;input
    LEA DX, INPUT_MSG
    MOV AH, 1
    MOV CX, 2
    INPUT:
        INT 21H   
    WHILE:
        ;if enter is pressed
        CMP AL, 0DH
        JE END_WHILE 
        ;if input is digit
            CMP AL, 30H
            JL END_IF1
            CMP AL, 39H
            JG END_IF1
        ;then push to stack
            SUB AL, 30H
            MOV AH, 0
            PUSH AX
            MOV AH, 1
            INC CX
        END_IF1:
        INT 21H
        JMP WHILE
    END_WHILE:
    
    ;pop digits from stack & construct the number
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
        
    ;if number is supposed to be negative
        CMP IS_NEGATIVE, 1
        JNE END_IF2
    ;then
        NEG OPERAND
    END_IF2:
    
    ;input operator
    CALL INPUT_OPERATOR
    
    ;input operand 2
    LEA DX, INPUT_OPERAND_2_MSG
    CALL INPUT_OPERAND
    MOV AX, OPERAND
    MOV OPERAND_2, AX
                      
    ;calculate and print result
    CALL CALCULATE                     
    CALL PRINT_RESULT
   
    DOS_EXIT:
    MOV AH, 4CH
    INT 21H  
MAIN ENDP



END MAIN