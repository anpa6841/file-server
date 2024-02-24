### Run Server

- gcc -o server server.c
- ./server

#### Test with netcat

<pre>
MacBook-Air:file-server anish$ nc 127.0.0.1 8080
Username: bob
Password: pass

Authentication successful.

client: download zen.txt
The Zen of Python, by Tim Peters

Beautiful is better than ugly.
Explicit is better than implicit.
Simple is better than complex.
Complex is better than complicated.
Flat is better than nested.
Sparse is better than dense.
Readability counts.
Special cases aren't special enough to break the rules.
Although practicality beats purity.
Errors should never pass silently.
Unless explicitly silenced.
In the face of ambiguity, refuse the temptation to guess.
There should be one-- and preferably only one --obvious way to do it.
Although that way may not be obvious at first unless you're Dutch.
Now is better than never.
Although never is often better than *right* now.
If the implementation is hard to explain, it's a bad idea.
If the implementation is easy to explain, it may be a good idea.
Namespaces are one honking great idea -- let's do more of those!

Transfer Complete

client: upload hello.txt
hello, world!

Transfer Complete
client: exit

MacBook-Air:file-server anish$ cat hello.txt 
hello, world!
</pre>

<pre>
MacBook-Air:file-server anish$ ./server 
Client connected - IP address: [127.0.0.1] Port: [49794]
File sent: zen.txt
Client pressed Enter
File uploaded successfully: hello.txt
</pre>

#### Test with Expect script

- Update action var to either upload/download
- ./test_file_transfer.exp