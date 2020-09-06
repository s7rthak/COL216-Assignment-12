import random

f = open("random.txt", "w")
f.write('16 <Cache size in bytes>\n')
f.write('2 <Chache line/block size in bytes>\n')
f.write('2 <Cache Associavity>\n')
f.write('4 <T>\n')
f.write('#memory access requests')

lines = random.randint(10,100)

option = ['R','W']
option_w = [1,1]

nums = list(range(0,42))

f.write('\n7,W,7')
prev = 7

for i in range(lines):
    weights = [1] * 42
    weights[prev] = 20
    now = random.choices(nums,weights=weights,k=1)[0]
    st = random.choices(option,weights=option_w,k=1)[0]
    if st == 'W':
        f.write('\n{},{},{}'.format(now,st,now))
    else:
        f.write('\n{},{}'.format(now,st))
    prev = now

f.close()