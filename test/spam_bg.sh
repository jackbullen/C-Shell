#!/usr/bin/expect

set timeout 2

spawn ./bin/myshell

send "bg sleep 2\r"
send "bg sleep 2\r"
send "bglist\r"

send "bgp 1\r"
send "bgp 2\r"
send "bglist\r"

send "bgr 1\r"
send "bglist\r"

send "bgkill 2\r"
expect "Wait for process to terminate..."

send "bglist\r"

expect eof