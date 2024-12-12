lena = pow(5, 9)
lenb = pow(5, 7)

def convertor(val: int) -> int:
	_res = 0
	while val:
		_res += val % 5
		val //= 5
	return _res

# print(convertor(1499), convertor(2099), convertor(1474))
# KILL_SWITCH: bool = True
# if KILL_SWITCH:
# 	exit(0)

# 执行部分
same_color = set()
component = set()

with open(f"0_to_{lenb}_as_char_hu.txt", 'r') as fd:
	while True:
		tmp = fd.readline()
		if tmp is None or len(tmp) <= 1:
			break
		curr = int(tmp[:-1])
		same_color.add(curr)

# with open(f"0_to_{lena}_as_num_distribution.txt", 'r') as fd:
# 	while True:
# 		tmp = fd.readline()
# 		if tmp is None or len(tmp) <= 1:
# 			break
# 		idx = 0
# 		for i, v in enumerate(tmp):
# 			if v == ':':
# 				idx = i
# 				break
# 		curr = int(tmp[:idx])
# 		component.add(curr)
# print(len(same_color), len(component))


res = ""
for i in range(0, lenb):
	if i % 10000 == 0:
		print(i)
	if convertor(i) >= 14 or i in same_color:
		continue
	res += f'{i}\n'

with open(f"possible_valid_state_in_{lenb}.txt", 'w') as fd:
	fd.write(res)


"""
def checker_consumer() -> bool:
	global component
	# 胡牌必由组件构成。
	return False



res = ""
cnt = 0
for i in range(0, lena):
	cnt += 1
	if cnt % 10000 == 0:
		print(cnt)
	if i in same_color or i in component or not (sum(convertor(i)) <= 18):
		continue
	flux = []
	for j in range(0, 9):
		k = i + pow(5, j)
		if k in same_color:
			flux.append(k)
	if len(flux) == 0:
		continue
	res += f'{i}: {flux}\n'

with open(f'num_middle_1_shot.txt', 'w') as fd:
	fd.write(res)

"""


