#!/usr/env/python3
# SPDX-LICENSE-IDENTIFIER: GPL2.0
# (C) 2024-11 Author: <kisfg@hotmail.com>
# 先实现，再优化。
import queue, ctypes
from typing import Optional, Union
import os, threading, time
import functools

try:
	lib = ctypes.CDLL("./libmahjong.so", winmode=0)
except Exception as e:
	print(e)
	exit(1)

task_queue: queue.Queue = queue.Queue(200)
record_sema: list[threading.Semaphore] = [threading.Semaphore(1) for _ in range(4)]
num_hu_record: str = ""
character_rec: str = ""
num_dist_record: str = ""
char_dist_rec: str = ""
WORK_LOAD: int = 25
CARDS_BUCKET_TYPE = ctypes.c_ubyte * 34
KILL_SWITCH: bool = True


def safe_add_num_hu_record(content: str) -> None:
	global num_hu_record
	record_sema[0].acquire()
	num_hu_record += content
	record_sema[0].release()


def safe_add_dist_record(content: str) -> None:
	global num_dist_record
	record_sema[1].acquire()
	num_dist_record += content
	record_sema[1].release()


def safe_add_char_record(content: str) -> None:
	global character_rec
	record_sema[2].acquire()
	character_rec += content
	record_sema[2].release()


def safe_add_char_dist_record(content: str) -> None:
	global char_dist_rec
	record_sema[3].acquire()
	char_dist_rec += content
	record_sema[3].release()


def convertor(val: int, lena: int = 9) -> list[int]:
	_res: list[int] = []
	while val:
		_res.append(val % 5)
		val //= 5
	while len(_res) != lena:
		_res.append(0)
	return _res


def de_convertor(arr: list[int]) -> int:
	_res = 0
	for idx, v in enumerate(arr):
		_res += v * pow(5, idx)  # 低级错误
	return _res


# 个人觉得写得严谨的话应该需要识别边张和嵌张和双边。
# 杠要不要拆成对？我觉得不是胡就不要拆，只保留整体。
# 有时候只需要一张对，有时候确实有多张对，这个时候怎么处理？
# 00: 对
# 10: 吃
# 20: 碰
# 30: 杠

fun = lambda x: x - 1
gun = lambda x: x + 1


def counterable_components_analyzer(arr: list[int], __dep: int = 0) -> Optional[Union[list, list[int]]]:
	def check_selfref(idx: int) -> Optional[Union[list[int], list]]:
		if arr[idx] == 1:
			return None
		elif arr[idx] == 2:
			arr[idx] = 0
			_payload = counterable_components_analyzer(arr, __dep + 1)
			arr[idx] = 2
			if _payload is None:
				return _payload
			_payload.append(idx + 1)
			return _payload
		elif arr[idx] == 3:
			arr[idx] = 0
			_payload = counterable_components_analyzer(arr, __dep + 1)
			arr[idx] = 3
			if _payload is None:
				return _payload
			_payload.append(21 + idx)
			return _payload
		elif arr[idx] == 4:
			arr[idx] = 0
			_payload = counterable_components_analyzer(arr, __dep + 1)
			arr[idx], final_res = 4, 31 + idx
			if _payload is None:
				arr[idx] = 2
				_payload = counterable_components_analyzer(arr, __dep + 1)
				arr[idx] = 4
				if _payload is None:
					return _payload
				final_res = idx + 1
			_payload.append(final_res)
			return _payload

	if len(arr) != 9 or __dep > 9 or not (0 <= sum(arr) < 14):
		return None
	if sum(arr) == 0:
		return []

	for i in range(0, 7):
		if arr[i] == 0:
			continue
		if arr[i + 1] != 0 and arr[i + 2] != 0:
			arr[i] = fun(arr[i])
			arr[i + 1] = fun(arr[i + 1])
			arr[i + 2] = fun(arr[i + 2])
			payload = counterable_components_analyzer(arr, __dep + 1)
			arr[i] = gun(arr[i])
			arr[i + 1] = gun(arr[i + 1])
			arr[i + 2] = gun(arr[i + 2])
			if payload is not None:
				payload.append(11 + i)
				return payload
		return check_selfref(i)

	for i in range(7, 9):
		if arr[i] == 0:
			continue
		return check_selfref(i)

	return [] if sum(arr) == 0 else None


def character_analyzer(arr: list[int]) -> list:
	res = []
	for idx, v in enumerate(arr):
		if v == 0:
			res.append(0)
		elif v == 1:
			res.append(1)
		elif v == 2:
			res.append(2)
		elif v == 3:
			res.append(3)
		elif v == 4:
			res.append(4)
	return res


def broducer(l: int, r: int) -> None:
	for _i in range(l, r):
		_curr = convertor(_i, 7)
		_cnt = sum(_curr)
		if _cnt > 18:
			continue
		elif 14 <= _cnt <= 18:
			zeros = []
			while len(_curr) + len(zeros) != 34:
				zeros.append(0)
			for v in _curr:
				zeros.append(v)
			data = CARDS_BUCKET_TYPE(*zeros)
			zeros.clear()
			try:
				res: int = lib.hu_checker(data)
			except Exception as ee:
				print(f'{ee}, we have to skip.')
				continue
			if res != -1:
				safe_add_char_record(f'{_i}\n')
			continue
		else:
			attr = character_analyzer(_curr)
			safe_add_char_dist_record(f'{_i}: {attr}\n')


def producer(l: int, r: int) -> None:
	global task_queue
	for _i in range(l, r):
		_curr = convertor(_i)
		_cnt = sum(_curr)
		if _cnt > 18:
			continue
		elif 14 <= _cnt <= 18:
			# 因为在范围之内，所以考虑扩充到 34 位，统一花色为万。
			while len(_curr) != 34:
				_curr.append(0)
			data = CARDS_BUCKET_TYPE(*_curr)
			_curr.clear()
			try:
				res: int = lib.hu_checker(data)
			except Exception as ee:
				print(f'{ee}, we have to skip.')
				continue

			# TODO: 后续考虑胡牌类型？可能不需要。
			if res != -1:
				safe_add_num_hu_record(f'{_i}\n')
			continue
		else:
			task_queue.put(_curr)


def consumer() -> None:
	global task_queue
	while True:
		try:
			subject = task_queue.get(timeout=10.0)
		except Exception as eee:
			print(f'exception: {eee}')
			break
		result = counterable_components_analyzer(subject)
		if result is None or len(result) == 0:
			continue
		result.sort()
		safe_add_dist_record(f'{de_convertor(subject)}: {str(result)}\n')


def cmp_for_dist(x: str, y: str) -> int:
	def find_colon(p: str):
		for idx, v in enumerate(p):
			if v == ':':
				return idx
		return -1

	ix = find_colon(x)
	iy = find_colon(y)
	if ix == -1 or iy == -1:
		return 0
	dx, dy = int(x[:ix]), int(y[:iy])
	if dx > dy:
		return 1
	elif dx < dy:
		return -1
	return 0


snow = [0, 1, 1, 1, 0, 1, 1, 4, 0]
now = de_convertor(snow)
print(now)
print(counterable_components_analyzer([0, 1, 1, 1, 0, 1, 1, 4, 0]))
print(counterable_components_analyzer([1, 1, 4, 1, 1, 0, 0, 0, 0]))
print(convertor(789585), convertor(77128), convertor(1074), convertor(197137))
if KILL_SWITCH:
	exit(0)

# 符合规范的牌直接拿去判胡
# 不合但是数量少的，可以拿去分析组成。
# 组成只能说构成一种参考，遇到复杂顺子可能逻辑还没有足够优秀。
bound_for_char: int = pow(5, 7)
boundary: int = pow(5, 9)
num_hu_txt: str = f'{0}_to_{boundary}_as_num_hu.txt'
char_txt: str = f'{0}_to_{pow(5, 7)}_as_char_hu.txt'
distribut_txt: str = f'{0}_to_{boundary}_as_num_distribution.txt'
char_dist_txt: str = f'{0}_to_{bound_for_char}_as_char_dist.txt'
choice: bool = not (os.access(distribut_txt, os.F_OK) and os.access(num_hu_txt, os.F_OK) and
                    os.access(char_txt, os.F_OK) and os.access(char_dist_txt, os.F_OK))

if choice:
	work_size = boundary // WORK_LOAD
	labor_siz = bound_for_char // WORK_LOAD
	print(boundary, work_size)
	gen: list[threading.Thread] = [
		threading.Thread(target=producer, args=(sizer, sizer + work_size)) \
		for sizer in range(0, boundary, work_size)
	]
	con: list[threading.Thread] = [
		threading.Thread(target=consumer, args=()) \
		for _ in range(WORK_LOAD)
	]
	rem: list[threading.Thread] = [
		threading.Thread(target=broducer, args=(sizer, sizer + labor_siz)) \
		for sizer in range(0, bound_for_char, labor_siz)
	]
	st = time.perf_counter()
	for g in gen:
		g.start()
	for c in con:
		c.start()
	for rr in rem:
		rr.start()
	for g in gen:
		g.join()
	for c in con:
		c.join()
	for rr in rem:
		rr.join()
	ed = time.perf_counter()
	print(f'consumed: {ed - st}s.')
	with open(num_hu_txt, 'w') as fd:
		fd.write(num_hu_record)
	with open(distribut_txt, 'w') as fd:
		fd.write(num_dist_record)
	with open(char_txt, 'w') as fd:
		fd.write(character_rec)
	with open(char_dist_txt, 'w') as fd:
		fd.write(char_dist_rec)

else:  # not choice
	with open(num_hu_txt, 'r+') as fd:
		datum = []
		while True:
			tmp = fd.readline()
			if tmp is None or len(tmp) <= 1:
				break
			datum.append(int(tmp[:-1]))
	datum.sort()
	with open(num_hu_txt, 'w') as fd:
		for d in datum:
			fd.write(str(d) + '\n')
	with open(char_txt, 'r+') as fd:
		datum = []
		while True:
			tmp = fd.readline()
			if tmp is None or len(tmp) <= 1:
				break
			datum.append(int(tmp[:-1]))
	datum.sort()
	with open(char_txt, 'w') as fd:
		for d in datum:
			fd.write(str(d) + '\n')

	with open(distribut_txt, 'r+') as fd:
		datum = []
		while True:
			tmp = fd.readline()
			if tmp is None or len(tmp) <= 1:
				break
			datum.append(tmp)
	datum.sort(key=functools.cmp_to_key(cmp_for_dist))
	with open(distribut_txt, 'w') as fd:
		for d in datum:
			fd.write(d)

	with open(char_dist_txt, 'r+') as fd:
		datum = []
		while True:
			tmp = fd.readline()
			if tmp is None or len(tmp) <= 1:
				break
			datum.append(tmp)
	datum.sort(key=functools.cmp_to_key(cmp_for_dist))
	with open(char_dist_txt, 'w') as fd:
		for d in datum:
			fd.write(d)
print('ok')
