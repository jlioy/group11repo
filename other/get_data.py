import re

array = []
with open('output.txt') as file:
  for line in file:
    if "SSTF" in line and "15." in line: 
      array.append(line)
    
times = []
sectors = []
print array
for item in array:
  times.append(item[4:12])
  sectors.append(item[-7:])

