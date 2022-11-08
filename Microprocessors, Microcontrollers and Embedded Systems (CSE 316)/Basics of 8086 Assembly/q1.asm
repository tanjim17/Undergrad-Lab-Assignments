.MODEL SMALL

.STACK 100H
                                   
.DATA
    CR EQU 0DH
    LF EQU 0AH
    
    MSG1 DB 'Enter X (any number between 0-9): $'
    MSG2 DB CR, LF, 'Enter Y (any number between 0-9): $'
    MSG3 DB CR, LF, 'Z = X - 2Y = $'
    MSG4 DB CR, LF, 'Z = 25 - (X + Y) = $'
    MSG5 DB CR, LF, 'Z = 2X - 3Y = $'
    MSG6 DB CR, LF, 'Z = Y - X + 1 = $'
    X DW ?
    Y DW ?
    Z DW ?

.CODE

MAIN PROC
;initialize DS
    MOV AX, @DATA
    MOV DS, AX
    
;print user prompt
    LEA DX, MSG1
    MOV AH, 9
    INT 21H

;input X     
    MOV AH, 1
    INT 21H
    SUB AL, 30h
    MOV AH, 00h
    MOV X, AX
    MOV BX, AX      
    
;print user prompt
    LEA DX, MSG2
    MOV AH, 9
    INT 21H

;input Y     
    MOV AH, 1
    INT 21H
    SUB AL, 30h
    MOV AH, 00h
    MOV Y, AX
    
;operation, Z = X - 2Y
    MOV AX, X
    MOV Z, AX
    MOV AX, Y
    SUB Z, AX
    SUB Z, AX 
              
;display z
    LEA DX, MSG3
    MOV AH, 9
    INT 21H  

    MOV AH, 2
    ADD Z, 30h
    MOV DX, Z
    INT 21H
    
;operation, 25 - (X + Y)
    MOV Z, 25
    MOV AX, X
    SUB Z, AX
    MOV AX, Y
    SUB Z, AX 
           
;display z
    LEA DX, MSG4
    MOV AH, 9
    INT 21H  

    MOV AH, 2
    ADD Z, 30h
    MOV DX, Z
    INT 21H
    
;operation, 2X - 3Y
    MOV AX, X
    MOV Z, AX
    ADD Z, AX
    MOV AX, Y
    SUB Z, AX
    SUB Z, AX
    SUB Z, AX
            
;display z
    LEA DX, MSG5
    MOV AH, 9
    INT 21H  

    MOV AH, 2
    ADD Z, 30h
    MOV DX, Z
    INT 21H

;operation, Z = Y - X + 1
    MOV AX, Y
    MOV Z, AX
    MOV AX, X
    SUB Z, AX
    ADD Z, 1 
           
;display z
    LEA DX, MSG6
    MOV AH, 9
    INT 21H  

    MOV AH, 2
    ADD Z, 30h
    MOV DX, Z
    INT 21H
       
;DOX exit
    MOV AH, 4CH
    INT 21H
  
MAIN ENDP

    END MAIN