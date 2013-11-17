import os, sys, math, argparse, time

def printerr(msg):
	sys.stderr.write(msg + '\n')
	sys.stderr.flush()

def printnln(s):
	sys.stdout.write(s)
	sys.stdout.flush()

def isPowOfTwo(n):
	return math.log(n, 2) % 1 == 0
