#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, math, argparse, time
from cdecimal import Decimal
from prettytable import PrettyTable
from common import *

def check_subzero(*args):
	if True:
		return

	printerr("Invalid arguments")
	print("args: %s" % str(args))
	sys.exit(1)

def main(*args):
	global timer, verbose, freq, period, ratio, freq
	# docs: http://docs.python.org/dev/library/argparse.html#argparse.ArgumentParser.add_argument
	parser = argparse.ArgumentParser(description='Script to help with AVR PWMs.')
	parser.add_argument('-v', action='count', dest='verbose', help='How much output is printed')
	parser.add_argument('-f_cpu', action='store', dest='f_cpu', default="16000000", help='Frequency in Hz')
	parser.add_argument('-m', '--mode', action='store', dest='pwm_mode', default='ctc', help='Operation mode (ctc, fast, phase)')
	parser.add_argument('-f', action='store', dest='freq', default='-1', help='Frequency in Hz')
	parser.add_argument('-p', '--period', action='store', dest='period', default='-1', help='[s]')
	parser.add_argument('-r', '--ratio', action='store', dest='ratio', default='-1', help='high/low')
	parser.add_argument('-ht', '--hightime', action='store', dest='hightime', default='-1', help='[s]')
	parser.add_argument('-lt', '--lowtime', action='store', dest='lowtime', default='-1', help='[s]')
	parser.add_argument('-t', '--timer', action='store', dest='timer', default='0', help='Timer number')
	parser.add_argument('-tw', '--timerwidth', action='store', dest='timerwidth', default='8', help='Width of timer counter in bits')

	orig_args = " ".join(args)
	args = parser.parse_args()
	verbose = args.verbose
	timer = int(args.timer)
	timerwidth = int(args.timerwidth)
	f_cpu = Decimal(args.f_cpu)
	freq = Decimal(args.freq)
	period = Decimal(args.period)
	ratio = Decimal(args.ratio)
	hightime = Decimal(args.hightime)
	lowtime = Decimal(args.lowtime)

	if not isPowOfTwo(timerwidth):
		printerr("Invalid arguments")
		print("args: %s" % str(args))
		return 1

	if freq > 0 or period > 0:
		if freq > 0:
			period = 1 / freq
		elif period > 0:
			freq = 1 / period

		if ratio > 0:
			check_subzero(hightime, lowtime)
			lowtime = period / (ratio + 1)
			hightime = period - lowtime
		elif hightime > 0:
			check_subzero(ratio, lowtime)
			lowtime = period - hightime
			ratio = hightime / lowtime
		elif lowtime > 0:
			check_subzero(ratio, hightime)
			hightime = period - lowtime
			ratio = hightime / lowtime
	else:
		if ratio > 0 and hightime > 0:
			check_subzero(lowtime)
			lowtime = hightime / ratio
		if ratio > 0 and lowtime > 0:
			check_subzero(hightime)
			hightime = lowtime * ratio
		if hightime > 0 and lowtime > 0:
			check_subzero(ratio)
			ratio = hightime / lowtime
		period = hightime + lowtime
		freq = 1 / period

	calc_ocr = False
	if ratio < 0:
		print "Missing arguments defining the high/low ratio. OCR values will not be calculated."
	else:
		calc_ocr = True

	pwm_mode = args.pwm_mode
	freq_mul = { 'fast': 1, 'phase': 2, 'ctc': 2 }
	freq_add = { 'fast': 1, 'phase': 0, 'ctc': 1 }

	if verbose:
		print """\
freq={freq}
period={period}
hightime={hightime}
lowtime={lowtime}
ratio={ratio}
f_cpu={f_cpu}
timerwidth={timerwidth}
""".format(freq=freq.quantize(Decimal("0.000")), period=period, hightime=hightime, lowtime=lowtime, ratio=ratio, f_cpu=f_cpu, timerwidth=timerwidth)

	presc_tbl = PrettyTable(["prescaler", "min freq [Hz]", "max freq [Hz]", "min period [µs]", "max period [µs]"])
	presc_tbl.align = "r"

	relerr = 0.05
	top = -1
	ocr = -1
	pwm_presc = -1
	pwm_step = -1
	pwm_freq = -1
	pwm_relerr = 1
	ocr_relerr = 1
	period_cpu = 1 / f_cpu
	timer_max = pow(2, timerwidth) - 1
	for presc in (1, 8, 64, 256, 1024):
	#for presc in (1024, 256, 64, 8, 1):
		if verbose:
			presc_tbl.add_row([
							   presc,
							   (f_cpu / (2 * presc * timer_max)).quantize(Decimal("0.01")),
							   (f_cpu / (2 * presc)).quantize(Decimal("0.01")),
							   (2 * presc * period_cpu * pow(10, 6)).quantize(Decimal("0.000001")),
							   (2 * presc * period_cpu * timer_max * pow(10, 6)).quantize(Decimal("0.000001")),
							  ])
		for i in xrange (1, timer_max):
			cur_freq = f_cpu / (freq_mul[pwm_mode] * presc * (i + freq_add[pwm_mode]))
			cur_relerr = abs(1 - cur_freq / freq)
			if calc_ocr:
				cur_ocr = (i + 1) * hightime / period - 1
				cur_ocr_relerr = abs(1 - hightime/((cur_ocr + 1) * presc * 1 / f_cpu))
			if cur_relerr < relerr and cur_relerr < pwm_relerr and \
			  (not calc_ocr or (cur_ocr_relerr < relerr and cur_ocr_relerr < ocr_relerr)):
				top = i
				if calc_ocr:
					ocr = cur_ocr
					ocr_relerr = cur_ocr_relerr
				pwm_presc = presc
				pwm_freq = cur_freq
				pwm_relerr = cur_relerr
				res = math.log(top + 1) / math.log(2)
				pwm_step = presc * 1 / f_cpu

	if verbose:
		print presc_tbl


	if top < 0 or pwm_relerr > relerr or (calc_ocr and (ocr < 0 or ocr_relerr > relerr)):
		printerr("No suitable combination of values found.")
		return 1

	if calc_ocr:
		if ocr_relerr > 0:
			print "Warning: no exact solution possible, rounding OCR value (ocr=%f)." % ocr
			ocr =  int(round(ocr))

		print """\
#define PWM{timer}_OCR       {ocr}""".format(timer=timer, ocr=ocr)

	print """\
#define PWM{timer}_TOP       {top}
#define PWM{timer}_PRESC     PRESC_{pwm_presc}
/* for documentation only:
created by '{orig_args}' */
#define PWM{timer}_FREQ      {pwm_freq}f /* [Hz] */
#define PWM{timer}_RES       {res}f /* [bits] */
#define PWM{timer}_STEP      {pwm_step}f /* [us] */""".format(timer=timer, top=top, pwm_step=pwm_step * 1000000, pwm_presc=pwm_presc, orig_args=orig_args, pwm_freq=pwm_freq.quantize(Decimal("0.000")), res=res)
	if calc_ocr:
		print """#define PWM{timer}_HIGHTIME  {ht}f /* [us] */
#define PWM{timer}_LOWTIME   {lt}f /* [us] */""".format(timer=timer, ht=(hightime * 1000000).quantize(Decimal("0.000")), lt=(lowtime * 1000000).quantize(Decimal("0.000")))


if __name__ == '__main__':
	sys.exit(main(*sys.argv))
