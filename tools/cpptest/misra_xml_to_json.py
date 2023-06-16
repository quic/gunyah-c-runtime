#!/usr/bin/env python3
# coding: utf-8
#
# © 2023 Qualcomm Innovation Center, Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause

"""
Run as a part of gitlab CI, after Parasoft reports have been generated.

This script converts the Parasoft XML-format report to a Code Climate
compatible json file, that gitlab code quality can interpret.
"""

import xml.etree.ElementTree as ET
import json
import argparse
import sys
import os

argparser = argparse.ArgumentParser(
    description="Convert Parasoft XML to Code Climate JSON")
argparser.add_argument('input', type=argparse.FileType('r'), nargs='?',
                       default=sys.stdin, help="the Parasoft XML input")
argparser.add_argument('--output', '-o', type=argparse.FileType('w'),
                       default=sys.stdout, help="the Code Climate JSON output")
args = argparser.parse_args()

tree = ET.parse(args.input)

parasoft_viols = tree.findall(".//StdViol") + tree.findall(".//FlowViol")

cc_viols = []

severity_map = {
    1: "blocker",
    2: "critical",
    3: "major",
    4: "minor",
    5: "info",
}

cc_viols = [
    ({
        "type": "issue",
        "categories": ["Bug Risk"],
        "severity": severity_map[int(v.attrib['sev'])],
        "check_name": v.attrib['rule'],
        "description": (v.attrib['msg'] + '. ' +
                        v.attrib['rule.header'] + '. (' +
                        v.attrib['rule'] + ')'),
        "fingerprint": v.attrib['unbViolId'],
        "location": {
            "path": v.attrib['locFile'].split(os.sep, 2)[2],
            "lines": {
                "begin": int(v.attrib['locStartln']),
                "end": int(v.attrib['locEndLn'])
            }
        }
    })
    for v in parasoft_viols]

args.output.write(json.dumps(cc_viols))
args.output.close()
