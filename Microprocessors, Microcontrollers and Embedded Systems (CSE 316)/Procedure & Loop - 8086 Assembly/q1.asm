.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    LINE_GAP DB CR, LF, '$'
    INPUT_OPERAND_1_MSG DB 'Enter operand 1: $'
    INPUT_OPERAND_2_MSG DB CR, LF, 'Enter operand 2: $'
    INPUT_OPERATOR_MSG DB CR, LF, 'Enter operator: $'
    ERROR_MSG DB CR, LF, 'Wrong operator$'
    EQUAL_SIGN DB ' = $'
    
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

    ;input operand 1
    LEA DX, INPUT_OPERAND_1_MSG
    CALL INPUT_OPERAND
    MOV AX, OPERAND
    MOV OPERAND_1, AX
    
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

INPUT_OPERAND PROC
    ;print prompt
    MOV AH, 9
    INT 21H
     
    ;start taking input
    MOV AH, 1
    INT 21H
    
    ;if minus sign given at first
        CMP AL, '-'
        JNE ELSE
    ;then
        MOV IS_NEGATIVE, 1
        JMP END_IF
    ELSE:
        MOV IS_NEGATIVE, 0
    END_IF:               

    ;input loop
    MOV CX, 0   
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
    RET
INPUT_OPERAND ENDP

INPUT_OPERATOR PROC
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
        JMP ELSE3
    THEN:
        MOV OPERATOR, AL
        JMP END_IF3
    ELSE3:
        ;if wrong command
            CMP AL, 'q' ; q is the proper command to quit
            JE END_IF4
        ;then
            LEA DX, ERROR_MSG
            MOV AH, 9
            INT 21H
        END_IF4:
        JMP DOS_EXIT
    END_IF3: 
    RET
INPUT_OPERATOR ENDP

CALCULATE PROC   
    ;if addition
        CMP OPERATOR, '+'
        JNE ELSE_IF_SUB
    ;then
        MOV AX, OPERAND_1
        ADD AX, OPERAND_2
        JMP END_IF5
    ELSE_IF_SUB: ;subtraction
        CMP OPERATOR, '-'
        JNE ELSE_IF_MUL
    ;then
        MOV AX, OPERAND_1
        SUB AX, OPERAND_2
        JMP END_IF5
    ELSE_IF_MUL: ;multiplication
        CMP OPERATOR, '*'
        JNE ELSE5
    ;then
        MOV AX, OPERAND_1
        IMUL OPERAND_2
        JMP END_IF5
    ELSE5: ;division
        MOV AX, OPERAND_1
        CWD
        IDIV OPERAND_2
    END_IF5:
    MOV RESULT, AX
    RET
CALCULATE ENDP

PRINT_RESULT PROC
    LEA DX, LINE_GAP
    MOV AH, 9
    INT 21H
    
    ;print operand 1
    CALL PRINT_OPENING_BRACKET
    MOV AX, OPERAND_1
    CALL PRINT_NUMBER
    CALL PRINT_CLOSING_BRACKET
    
    ;print operator
    CALL PRINT_OPENING_BRACKET
    MOV DL, OPERATOR
    MOV AH, 2
    INT 21H
    CALL PRINT_CLOSING_BRACKET
    
    ;print operand 2
    CALL PRINT_OPENING_BRACKET
    MOV AX, OPERAND_2
    CALL PRINT_NUMBER
    CALL PRINT_CLOSING_BRACKET
    
    LEA DX, EQUAL_SIGN
    MOV AH, 9
    INT 21H
    
    ;print result
    CALL PRINT_OPENING_BRACKET
    MOV AX, RESULT
    CALL PRINT_NUMBER
    CALL PRINT_CLOSING_BRACKET
    RET
PRINT_RESULT ENDP

PRINT_NUMBER PROC
    ;if number is negative
        TEST AX, 8000H
        JZ ELSE6
    ;then
        NEG AX
        MOV IS_NEGATIVE, 1
        JMP END_IF6
    ELSE6:
        MOV IS_NEGATIVE, 0
    END_IF6:
    
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
    
    MOV AH, 2  
    ;if number is negative
        CMP IS_NEGATIVE, 1
        JNE END_IF7
    ;then
        MOV DL, '-'
        INT 21H
    END_IF7:

    ;pop digits from stack and print
    TOP1:
        POP DX
        ADD DL, 30H
        INT 21H
        LOOP TOP1
    RET
PRINT_NUMBER ENDP

PRINT_OPENING_BRACKET PROC
    MOV AH, 2
    MOV DL, '['
    INT 21H
    RET
PRINT_OPENING_BRACKET ENDP

PRINT_CLOSING_BRACKET PROC
    MOV AH, 2
    MOV DL, ']'
    INT 21H
    RET
PRINT_CLOSING_BRACKET ENDP

END MAIN