#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, math, argparse, time
from cdecimal import Decimal
from prettytable import PrettyTable
from common import *

def main(*args):
	global verbose, long_width, timer_width, param_width, timer
	# docs: http://docs.python.org/dev/library/argparse.html#argparse.ArgumentParser.add_argument
	parser = argparse.ArgumentParser(description='Script to calculate AVR timer constants.')
	parser.add_argument('-v', action='count', dest='verbose', help='How much output is printed')
	parser.add_argument('-f', action='store', dest='freq', default='-1', help='Frequency in Hz')
	parser.add_argument('-r', '--res', action='store', dest='res', default='-1', help='Resolution in seconds. The smallest period the timer should support.')
	parser.add_argument('-tw', '--timer_width', action='store', dest='timer_width', default='8', help='Width of timer counter in bits')
	parser.add_argument('-lw', '--long_width', action='store', dest='long_width', default='8', help='Width of the variable storing the long count')
	parser.add_argument('-pw', '--param_width', action='store', dest='param_width', default='8', help='Width of the parameter configuring the duration')
	parser.add_argument('-t', '--timer', action='store', dest='timer', default='0', help='Timer number')

	orig_args = " ".join(args)
	args = parser.parse_args()
	verbose = args.verbose
	timer = int(args.timer)
	freq = int(args.freq)
	res = Decimal(args.res)
	timer_width = int(args.timer_width)
	long_width = int(args.long_width)
	param_width = int(args.param_width)
	if param_width < long_width:
		param_width = long_width

	if \
		freq < 0 or \
		freq > 100 * pow(10, 6) or \
		res < 0 or \
		not isPowOfTwo(timer_width) or \
		not isPowOfTwo(param_width) or \
		not isPowOfTwo(long_width) \
		:
		printerr("Invalid arguments")
		print("args: %s" % str(args))
		return 1

	if verbose:
		print("args: %s" % str(args))
		print
		print("RES [s]:\t%f" % res)
		print("RES [ms]:\t%f" % (res * pow(10, 3)))
		print("RES [us]:\t%f" % (res * pow(10, 6)))
		print("RES [ns]:\t%f" % (res * pow(10, 9)))
		print

		min_res = "min period [RES]"
		min_ns = "min period [µs]"
		max_res = "max period [RES]"
		max_s = "max period [s]"
		presc_tbl = PrettyTable(["prescaler", min_res, min_ns, max_res, max_s])
		presc_tbl.align = "r"

	clk_period = (1 / Decimal(freq))
	timer_max = pow(2, timer_width) - 1
	long_max = pow(2, long_width) - 1
	ocr_short = timer_max + 1
	presc_short = 0
	for presc in (1, 8, 64, 256, 1024):
		if verbose:
			presc_tbl.add_row([
							   presc,
							   (presc * clk_period / res).quantize(Decimal("0.0001")),
							   (presc * clk_period * pow(10, 6)).quantize(Decimal("0.000001")),
							   (presc * clk_period * timer_max / res).quantize(Decimal("0.001")),
							   (presc * clk_period * timer_max).quantize(Decimal("0.000001")),
							  ])
		for i in xrange (1, timer_max + 1):
			val = Decimal(clk_period * presc * i / res)
			if val == 1:
				ocr_short = i
				presc_short = presc
				continue

	if verbose:
		print presc_tbl

	if presc_short == 0:
		print "No perfect match of prescaler and top value found for this resolution (%f), frequency and word lengths" % res
		return 1

	short_max_time = Decimal(clk_period * presc_short * ocr_short * 255)
	short_max_res = Decimal(short_max_time / res)
	if verbose:
		print "OCR_SHORT: %d, PRESC_SHORT: %d" % (ocr_short, presc_short)
		print "maximum interval reachable with the short prescaler (%d) alone: %fs (%d RES)" % (presc_short, short_max_time, short_max_res)

	ocr_long = timer_max + 1
	presc_long = 0
	for presc in (1, 8, 64, 256, 1024):
		if presc < presc_short:
			continue
		for i in xrange (1, timer_max + 1):
			val = Decimal(clk_period * presc * i / res)
			if val <= short_max_res and Decimal.remainder(val, 1) == 0:
				presc_long = presc
				ocr_long = i

	long_dur = clk_period * presc_long * ocr_long
	long_max_time = Decimal(long_dur * long_max)
	long_max_res = int(long_max_time / res)

	if verbose:
		print "OCR_LONG: %d, PRESC_LONG: %d" % (ocr_long, presc_long)
		print "long interval is %fs (%d RES)" % (long_dur, long_dur / res)
		print "maximum interval reachable with the long prescaler (%d) alone: %fs (%d RES)" % (presc_long, long_max_time, long_max_res)
		print
	print """\
#define PRESC{timer}_SHORT    PRESC_{presc_short}
#define PRESC{timer}_LONG     PRESC_{presc_long}
#define OCR{timer}_SHORT      {ocr_short}
#define OCR{timer}_LONG       {ocr_long}
#define DUR{timer}_LONG       {dur_long}
#define DUR{timer}_LONG_WIDTH {long_width}
#define DUR{timer}_PARAM_WIDTH {param_width}
/* for documentation only:
created by '{orig_args}' */
#define DUR{timer}_MAX        {max_time}f /* [s] */
#define F_TIMER{timer}        {freq} /* [Hz] */
#define RES{timer}            {res}f /* [s] */""".format(timer=timer, presc_short=presc_short, presc_long=presc_long, ocr_short=ocr_short, ocr_long=ocr_long, dur_long=int(long_dur/res), long_width=long_width, param_width=param_width, orig_args=orig_args, max_time=long_max_time + short_max_time, freq=freq, res=res)

	return 0

if __name__ == '__main__':
	sys.exit(main(*sys.argv))
