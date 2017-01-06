#!/bin/bash
./dump.sh | grep -Ev "\(00(0[0-9A-F]|10) 00(0[0-9A-F]|10) 00(0[0-9A-F]|10)\)"
