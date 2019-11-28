f = open("measurements1.txt")
print("Opened Measurements")
registers = {}
for line in f:
    vals = line.split('-->')
    if vals[0] not in registers:
        registers[vals[0]] = [vals[1]]
    else:
        registers[vals[0]].append(vals[1])

f.close()
print(len(registers['0x01']))
