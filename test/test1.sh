#!/usr/bin/expect

set timeout 2

spawn ./bin/myshell

send "bg sleep 100\r"
send "bg sleep 200\r"
send "bgp 1\r"
send "bgp 2\r"
send "bglist\r"
send "bgkill 2\r"
send "bglist\r"
send "bgkill 1\r"
send "bglist\r"
expect eof