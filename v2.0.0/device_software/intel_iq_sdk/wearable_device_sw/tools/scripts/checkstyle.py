#!/usr/bin/python

import argparse
import sys
import fnmatch
import os
import subprocess

#default config file
CONFIG_FILE=os.path.join(os.path.dirname(os.path.realpath(__file__)),
		"thunderdome.cfg")
VERBOSE=False

def log(*args):
	if VERBOSE:
		print(args[0])

def run(cmd):
	try:
		retcode = subprocess.call(cmd, shell=True)
		if (retcode == 0):
			return True
		else:
			return False
	except OSError as e:
		print("Execution failed.\n{}\n".format(e))
		sys.exit(-1)


def find_files(paths):
	matches = [f for f in paths if os.path.isfile(f)]
	for path in paths:
		for root, dirnames, filenames in os.walk(path):
			for filename in fnmatch.filter(filenames, '*.[c|h]'):
				matches.append(os.path.join(root, filename))
	return matches

def fix_files(paths):
	ret = True
	log("Fixing files...\n")
	files = find_files(paths)
	for i in files:
		log("Processing {}".format(i))
		cur_ret = run("uncrustify -q --replace --no-backup -c {} {} > /dev/null".format(CONFIG_FILE, i))
		if cur_ret is False:
			print("File fails format check: {}\n".format(i))
		ret = ret and cur_ret
	return ret

def check_files(paths):
	ret = True
	log("Checking style in files...")
	files = find_files(paths)
	for i in files:
		log("Processing {}".format(i))
		cur_ret = run("uncrustify -q -c {} -f {} | diff {} - > /dev/null".format(CONFIG_FILE, i, i))
		if cur_ret is False:
			if VERBOSE is True:
				cur_ret = run("uncrustify -c {} -f {} | diff {} -".format(CONFIG_FILE, i, i))
			print("File fails format check: {}".format(i))
		ret = ret and cur_ret
	return ret

def main():
	ret = True

	parser = argparse.ArgumentParser(epilog="The script will recursively search"
		" .c and .h files in the given paths. Style check will be done using"
		" uncrustify.")
	parser.add_argument("-v", "--verbose", help="increase output verbosity",
		action="store_true")
	parser.add_argument("-c", "--check", help="set check format mode",
		action="store_true")
	parser.add_argument("-f", "--fix", help="set fix format mode (destructive!)",
		action="store_true")
	parser.add_argument("-s", "--settings", help="set uncrustify settings file")
	parser.add_argument('paths', nargs=argparse.REMAINDER,
		help="paths to be checked recursively, separated by spaces")
	args = parser.parse_args()

	if args.verbose:
		global VERBOSE
		VERBOSE=True
	if args.settings:
		global CONFIG_FILE
		CONFIG_FILE=args.settings
	if (args.check and args.fix) or not (args.check or args.fix):
		print("Please choose if you want to check or fix the files.\n")
		parser.print_help()
		return 1
	if not args.paths:
		print("No paths specified.\n")
		parser.print_help()
		return 1

	log("Uncrustify settings: {}\n".format(CONFIG_FILE))
	if args.check:
		ret = check_files(args.paths)
	elif args.fix:
		ret = fix_files(args.paths)

	if ret is True:
		return 0
	else:
		return 1

if __name__ == '__main__':
	sys.exit(main())

