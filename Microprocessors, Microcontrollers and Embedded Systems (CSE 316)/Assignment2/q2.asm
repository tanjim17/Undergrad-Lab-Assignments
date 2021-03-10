.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    PROMPT_MSG DB 'Enter password: $'
    VALID_MSG DB CR, LF, 'Valid Password $'
    INVALID_MSG DB CR, LF, 'Invalid Password $'
    UPPER_CASE_COUNT DB 0
    LOWER_CASE_COUNT DB 0
    DIGIT_COUNT DB 0

.CODE

MAIN PROC
;initialize DS
    MOV AX, @DATA
    MOV DS, AX
    
;show prompt
    LEA DX, PROMPT_MSG
    MOV AH, 9
    INT 21H
    MOV AH, 1
    
INPUT:     
    INT 21H
;if chracter is between 21h and 7Eh      
    CMP AL, 21H
    JL END_IF1
    CMP AL, 7EH
    JG END_IF1
;then
    ;if character is upper case
        CMP AL, 41H
        JL ELSE_IF1
        CMP AL, 5AH
        JG ELSE_IF1
    ;then
        MOV UPPER_CASE_COUNT, 1
        JMP END_IF2
    
    ELSE_IF1: ;else if character is lower case
        CMP AL, 61H
        JL ELSE_IF2
        CMP AL, 7AH
        JG ELSE_IF2
    ;then
        MOV LOWER_CASE_COUNT, 1
        JMP END_IF2
        
    ELSE_IF2: ;else if character is digit
        CMP AL, 30H
        JL END_IF2
        CMP AL, 39H
        JG END_IF2
    ;then
        MOV DIGIT_COUNT, 1
    END_IF2:
        JMP INPUT
END_IF1:

;if passoword is valid
    CMP UPPER_CASE_COUNT, 1
    JNE ELSE
    CMP LOWER_CASE_COUNT, 1
    JNE ELSE    
    CMP DIGIT_COUNT, 1
    JNE ELSE
;then
    LEA DX, VALID_MSG
    MOV AH, 9
    INT 21H
    JMP END_IF3
ELSE:
    LEA DX, INVALID_MSG
    MOV AH, 9
    INT 21H
END_IF3:

;return to DOS
    MOV AH, 4CH
    INT 21H
  
MAIN ENDP

END MAIN