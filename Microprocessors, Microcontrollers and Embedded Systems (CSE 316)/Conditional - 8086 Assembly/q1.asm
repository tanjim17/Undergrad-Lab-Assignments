.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    INPUT_X_MSG DB 'Enter X (any number between 0-9): $'
    INPUT_Y_MSG DB CR, LF, 'Enter Y (any number between 0-9): $'
    INPUT_Z_MSG DB CR, LF, 'Enter Z (any number between 0-9): $'
    ALL_EQUAL_MSG DB CR, LF, 'All the numbers are equal$'
    SECOND_HIGHEST_MSG DB CR, LF, '2nd highest number is: $'
    X DB ?
    Y DB ?
    Z DB ?

.CODE

MAIN PROC
;initialize DS
    MOV AX, @DATA
    MOV DS, AX
    
;input X
    LEA DX, INPUT_X_MSG
    MOV AH, 9
    INT 21H     
    MOV AH, 1
    INT 21H
    MOV X, AL      

;input Y
    LEA DX, INPUT_Y_MSG
    MOV AH, 9
    INT 21H     
    MOV AH, 1
    INT 21H
    MOV Y, AL
    
;input Z
    LEA DX, INPUT_Z_MSG
    MOV AH, 9
    INT 21H     
    MOV AH, 1
    INT 21H
    MOV Z, AL
     
;if X < Y
    MOV AL, Y
    CMP X, AL
    JNL END_IF1
;then
    XCHG X, AL
    MOV Y, AL
END_IF1:

;if X < Z
    MOV AL, Z
    CMP X, AL
    JNL END_IF2
;then
    XCHG X, AL
    MOV Z, AL
END_IF2:   
       
;if Y < Z
    MOV AL, Z
    CMP Y, AL
    JNL END_IF3
;then
    XCHG Y, AL
    MOV Z, AL
END_IF3:

;if X = Y
    MOV AL, Y
    CMP X, AL
    JNE ELSE1
;then if Y = Z
        CMP Z, AL
        JNE ELSE2
    ;then    
        LEA DX, ALL_EQUAL_MSG
        MOV AH, 9
        INT 21H
        JMP END_IF5
    ELSE2:
        LEA DX, SECOND_HIGHEST_MSG
        MOV AH, 9
        INT 21H
        MOV AH, 2
        MOV DL, Z
        INT 21H
    END_IF5:
        JMP END_IF4
ELSE1:
    LEA DX, SECOND_HIGHEST_MSG
    MOV AH, 9
    INT 21H
    MOV AH, 2
    MOV DL, Y
    INT 21H
END_IF4:
    
;return to DOS
    MOV AH, 4CH
    INT 21H
  
MAIN ENDP

END MAIN