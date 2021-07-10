machineOpcode = {}
registerId = {}
linesOfCode = []
flagLabel = {}
curInsNo = 1



def initialize():
    pattern = "GHCIAFEBJKDPLNOM"
    opcodes = ["add", "addi", "sub", "subi", "and", "andi", "or", "ori", "sll", "srl", "nor", "sw", "lw", "beq", "bneq", "j"]

    patternLength = len(pattern)

    for i in range(patternLength):
        tmp_id = ord(pattern[i]) - ord('A')
        machineOpcode[opcodes[tmp_id]] = hex(i)[2:]

    registerId["$zero"] = "0"
    registerId["$t0"] = "1"
    registerId["$t1"] = "2"
    registerId["$t2"] = "3"
    registerId["$t3"] = "4"
    registerId["$t4"] = "5"
    registerId["$sp"] = "6"


def printTheResult(opcode, extra, machineValue):
    print("       currInsNo: ", curInsNo)
    print("       assemblyValue: " +opcode + " " +extra)
    print("       machineValue: " + machineValue)


def Rtype(opcode, extra, ):
    global curInsNo
    curInsNo += 1
    machineValue = str(machineOpcode[opcode])

    tmpStr = extra.split(",")
    dst = registerId[tmpStr[0].strip()]
    s1 = registerId[tmpStr[1].strip()]

    if tmpStr[2].strip()[0] == "$":
        s2 = registerId[tmpStr[2].strip()]
        machineValue += s1 + s2 + dst + "0\n"
    else:
        shAmt = hex(int(tmpStr[2].strip()))[2:]
        machineValue += "0" + s1 + dst + shAmt + "\n"

    printTheResult(opcode, extra, machineValue)

    return machineValue


def Itype(opcode, extra):
    global curInsNo
    curInsNo += 1
    machineValue = str(machineOpcode[opcode])

    tmpStr = extra.split(",")
    dst = registerId[tmpStr[0].strip()]
    s1 = registerId[tmpStr[1].strip()]
    imme = format(int(tmpStr[2].strip()), '02x')

    machineValue += s1 + dst + imme + "\n"

    printTheResult(opcode, extra, machineValue)
    return machineValue


def LoadOrStore(opcode, extra):
    global curInsNo
    curInsNo += 1
    machineValue = str(machineOpcode[opcode])

    tmpStr = extra.split(",")
    dst = registerId[tmpStr[0].strip()]

    tmpStr2 = tmpStr[1].strip().split("(")
    imme = format(int(tmpStr2[0].strip()), '02x')
    src = registerId[tmpStr2[1].strip()[:-1]]

    machineValue += src + dst + imme + "\n"

    printTheResult(opcode, extra, machineValue)
    return machineValue


def Jtype(opcode, extra):
    global curInsNo
    curInsNo += 1
    machineValue = str(machineOpcode[opcode])
    print("line no: ", int(flagLabel[extra.strip()]))
    addressLine = format(int(flagLabel[extra.strip()]), '02x')
    machineValue += addressLine + "00\n"

    printTheResult(opcode, extra, machineValue)
    return machineValue


def EquOrNot(opcode, extra):
    global curInsNo
    curInsNo += 1
    machineValue = str(machineOpcode[opcode])

    tmpStr = extra.split(",")
    reg1 = registerId[tmpStr[0].strip()]
    reg2 = registerId[tmpStr[1].strip()]

    print(flagLabel)
    print(tmpStr[2].strip())
    print(flagLabel[tmpStr[2].strip()])

    flagLine = int(flagLabel[tmpStr[2].strip()])
    lineDiff = flagLine-curInsNo-1
    print("line diff: ", lineDiff)

    if lineDiff < 0:
        lineDiff = 256 + lineDiff
    addressLine = format(lineDiff, '02x')

    machineValue += reg2 + reg1 + addressLine + "\n"

    printTheResult(opcode, extra, machineValue)
    return machineValue


def Push(opcode, extra):
    machineValue = ""
    extra = extra.strip()
    if extra[0] == "$":
        s = "sw " + extra + ", 0($sp)"
        machineValue += convertToMachineCode(s)
        s = "subi $sp, $sp, 1"
        machineValue += convertToMachineCode(s)
        return machineValue
    else:
        tmpStr = extra.split("(")
        imme = tmpStr[0]
        reg = tmpStr[1].strip()[:-1]

        s = "lw $zero, " + imme + "(" + reg + ")"
        machineValue += convertToMachineCode(s)

        s = "sw $zero, 0($sp)"
        machineValue += convertToMachineCode(s)

        s = "subi $sp, $sp, 1"
        machineValue += convertToMachineCode(s)

        s = "andi $zero, $zero, 0"
        machineValue += convertToMachineCode(s)

    printTheResult(opcode, extra, machineValue)
    return machineValue


def Pop(opcode, extra):
    machineValue = ""
    extra = extra.strip()

    s = "addi $sp, $sp, 1"
    machineValue += convertToMachineCode(s)
    s = "lw " + extra + ", 0($sp)"
    machineValue += convertToMachineCode(s)


    printTheResult(opcode, extra, machineValue)
    return machineValue


def convertToMachineCode(line):
    firstSplit = line.split(" ", 1)
    opCode = firstSplit[0]

    if opCode == "add":
        return Rtype(opCode, firstSplit[1])
    if opCode == "addi":
        return Itype(opCode, firstSplit[1])
    if opCode == "sub":
        return Rtype(opCode, firstSplit[1])
    if opCode == "subi":
        return Itype(opCode, firstSplit[1])
    if opCode == "and":
        return Rtype(opCode, firstSplit[1])
    if opCode == "andi":
        return Itype(opCode, firstSplit[1])
    if opCode == "or":
        return Rtype(opCode, firstSplit[1])
    if opCode == "ori":
        return Itype(opCode, firstSplit[1])
    if opCode == "sll":
        return Rtype(opCode, firstSplit[1])
    if opCode == "srl":
        return Rtype(opCode, firstSplit[1])
    if opCode == "nor":
        return Rtype(opCode, firstSplit[1])
    if opCode == "sw":
        return LoadOrStore(opCode, firstSplit[1])
    if opCode == "lw":
        return LoadOrStore(opCode, firstSplit[1])
    if opCode == "beq":
        return EquOrNot(opCode, firstSplit[1])
    if opCode == "bneq":
        return EquOrNot(opCode, firstSplit[1])
    if opCode == "j":
        return Jtype(opCode, firstSplit[1])
    if opCode == "push":
        return Push(opCode, firstSplit[1])
    if opCode == "pop":
        return Pop(opCode, firstSplit[1])


def isLabel(label):
    if len(label.split(" ")) == 1:
        return True
    else:
        return False


def isPushOrPop(line):
    firstSplit = line.split(" ", 1)
    opCode = firstSplit[0]
    extra = firstSplit[1]
    if opCode == "push":
        extra = extra.strip()
        if extra[0] == "$":
            return 1
        else:
            return 2
    elif opCode == "pop":
        return 3
    else:
        return 0


def main():
    inputFile = open("input.txt", "r")
    outputFile = open("machine_code.txt", "w")
    initialize()

    instructionNo = 1
    for line in inputFile:

        # to identify the label
        tmp = line.strip()
        mmLabel = isLabel(tmp)
        if mmLabel:
            flagLabel[tmp[:-1]] = instructionNo + 1
            continue
        ##############################

        linesOfCode.append(line)
        # to identify the push and pop opcode
        mmPushOrPop = isPushOrPop(line)
        if mmPushOrPop == 1:
            instructionNo += 2
        elif mmPushOrPop == 2:
            instructionNo += 4
        elif mmPushOrPop == 3:
            instructionNo += 2
        else:
            instructionNo += 1

    print("total instruction: ", instructionNo)
    print(flagLabel)
    outputFile.write("v2.0 raw\n")
    outputFile.write("50000\n")
    outputFile.write("706ff\n")

    global curInsNo
    print("curINsno :  ", curInsNo)
    for i in range(len(linesOfCode)):

        machineValue = convertToMachineCode(linesOfCode[i])
        outputFile.write(machineValue)

    inputFile.close()
    outputFile.close()



main()
