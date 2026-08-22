#!/bin/bash

: '

Example command to return basic file
metadata in a single command. Baseline for
what a table record is. The commands below
are different based on OS


'

# Macos /NetBSD

find . -exec stat -f '%R %N	%HT	%z	%m	%Mp%Lp	%Sp	%u	%g	%i	%d	%l' {} \;