import os
import sys

import xmlschema
from lxml import etree

ACCEPTED_ROOT_TAGS = ("CLASSTREE", "OBJECTS_CONFIG")


def print_error(error, index):
    print("[ERROR %d]" % index)
    try:
        print(error)
    except UnicodeEncodeError as e:
        # 当error对应的那段xml存在中文乱码时，无法打印error
        print("There may be Chinese garbled characters after %s lines, "
              "please check again after fixing it." % error.sourceline)


def validate_one_file(xsd, file_path):
    if os.path.splitext(file_path)[-1] != '.xml':
        return

    print("check for file : %s" % file_path)
    try:
        file_tree = etree.parse(file_path, etree.XMLParser())
        # 如果xml文件的根节点标签不是CLASSTREE或OBJECTS_CONFIG，则不进行检查
        if file_tree.getroot().tag not in ACCEPTED_ROOT_TAGS:
            return
        errors = xsd.iter_errors(file_tree)
        for index, error in enumerate(errors, start=1):
            print_error(error, index)
    except Exception as e:
        print("[WARNING] an exception(%s) occurs while checking this file" % e)


def validate_directory(xsd, dir_path):
    for root, dirs, files in os.walk(dir_path):
        for file in files:
            validate_one_file(xsd, os.path.join(root, file))


if len(sys.argv) != 3:
    raise ValueError("usage: python lint.py xsd_file_path file_path")

xsd_file_path = sys.argv[1]
files_path = sys.argv[2]
xsd = xmlschema.XMLSchema(xsd_file_path)

if os.path.isfile(files_path):
    validate_one_file(xsd, files_path)
else:
    validate_directory(xsd, files_path)
