res = 1
quotient = 1
for i in range(1, 15):
    res = res * (36 - i)
    quotient *= i
print(res / quotient)
# 好像算不对。理论来说应该是 三千多亿。