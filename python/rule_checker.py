#!/usr/bin/env python

import argparse
import ruleset

parser = argparse.ArgumentParser(description='Print ruleset details')

parser.add_argument('states',type=int,help='Number of cell states')
parser.add_argument('rule',type=int,help="Rule number")

args = parser.parse_args()

a = ruleset.ruleset(args.rule,args.states)
print a.toString()
