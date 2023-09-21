#!/usr/bin/expect

set timeout 2

spawn ./myshell

send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"
send "bg sleep 10\r"

send "bglist\r"
expect eof