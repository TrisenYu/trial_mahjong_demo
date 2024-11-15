res, cnt = 1, 136
curr = 1
for i in range(0, 14):
    res *= (cnt-i)
    curr *= i+1
print(int(res/curr))