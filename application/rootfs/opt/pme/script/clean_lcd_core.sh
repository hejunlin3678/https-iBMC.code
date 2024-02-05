#!/bin/sh

# This script delete coredump file which gen by led app.

# note: This script only running at PME C10 of PME project term.


# May you do good and not evil.
# May you find forgiveness for yourself and forgive others.
# May you share freely, never taking more than you give.

# author: 
# date: Fri Apr 18 10:48:07 CST 2014
# version: 0.8.0-1



rm -f /var/core-lcd--* >/dev/null 2>&1

exit 0

