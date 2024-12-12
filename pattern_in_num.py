import ctypes, time
try:
	lib = ctypes.CDLL("./libmahjong.so", winmode=0)
except Exception as e:
	print(e)
	exit(1)
lena = pow(5, 9)

numno = []

with open(f"0_to_{lena}_as_num_distribution.txt", 'r') as fd:
	while True:
		tmp = fd.readline()
		if tmp is None or tmp == '\n' or tmp == '':
			break
		pos = 0
		for idx, v in enumerate(tmp):
			if v == ':':
				pos = idx
				break
		numno.append(int(tmp[:idx]))

def convertor(val: int, limit: int = 9) -> list[int]:
	_res = []
	while val:
		_res.append(val % 5)
		val //= 5
	while len(_res) != limit:
		_res.append(0)
	return _res

# 286550 286550 286550 34995 ? 真的需要跑这么久吗？
# 不如逐个击破，反正中间也有一堆状态不是很好的。
CARDS_BUCKET_TYPE = ctypes.c_ubyte * 34
# sres = ""
cnt = 0
alien = len(numno)
print(alien)
for i in numno:
	idx = convertor(i)
	si = sum(idx)
	if not (2 <= si <= 16):
		continue
	for j in numno:
		jdx = convertor(j)
		if not (14 <= si + sum(jdx) <= 18):
			continue
		for s in idx:
			jdx.append(s)
		while len(jdx) != 34:
			jdx.append(0)
		data = CARDS_BUCKET_TYPE(*jdx)
		jdx.clear()
		res: int = lib.hu_checker(data)
		if res != -1:
			# 加入结果。
			# sres += f'{i},{j}\n'
			cnt += 1
			continue
print(alien * alien, cnt)

# 每个空的约束条件是 0-4
# 所有空的约束条件是 14 <= 总和 <= 18，初始构成理应是 13 份。
# 如果每个花色都要有，又需要总和在 14 - 18 之间，那往往按照均摊来考虑
# 14 / 4 = 3，也就是需要 3 ~ 5 张才可能成一副牌。如果是 13579 2468 这样的，通常就等进张而不会丢出去。
# 18 / 4 = 4

