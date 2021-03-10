.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    MSG1 DB 'Enter an uppercase letter : $'
    MSG2 DB CR, LF, 'Previous letter : $'
    MSG3 DB CR, LF, "1's complement : $"
    PREV_LETTER DB ?
    ONES_COMPLEMENT DB ?

.CODE

MAIN PROC
;initialize DS
    MOV AX, @DATA
    MOV DS, AX
    
;print user prompt
    LEA DX, MSG1
    MOV AH, 9
    INT 21H

;input an upper case character     
    MOV AH, 1
    INT 21H
    
;find previous letter
    MOV PREV_LETTER, AL
    SUB PREV_LETTER, 1 
    
;find 1's complement
    MOV ONES_COMPLEMENT, AL
    NOT ONES_COMPLEMENT
    
;display previous letter
    LEA DX, MSG2
    MOV AH, 9
    INT 21H
    
    MOV AH, 2
    MOV DL, PREV_LETTER
    INT 21H

;display one's complement 
    LEA DX, MSG3
    MOV AH, 9
    INT 21H
    
    MOV AH, 2
    MOV DL, ONES_COMPLEMENT
    INT 21H
     
;DOX exit
    MOV AH, 4CH
    INT 21H
  
MAIN ENDP

END MAIN