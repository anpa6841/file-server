### Run Server

- gcc -o server server.c
- ./server

#### Test with Expect script

- Update action var to either upload/download
- ./test_file_transfer.exp

#### Download

<pre>
MacBook-Air:file-server anish$ ./test_file_transfer.exp 
spawn nc 127.0.0.1 8080
Username: bob
Password: pass
Authentication successful.
Please select 'upload' or 'download' action: download
Enter Filename: zen.txt
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
</pre>

#### Upload

<pre>
Transfer Complete^CMacBook-Air:file-server anish$ ./test_file_transfer.exp 
spawn nc 127.0.0.1 8080
Username: bob
Password: pass
Authentication successful.
Please select 'upload' or 'download' action: upload
Enter Filename: zen_1.txt
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
^CMacBook-Air:file-server anish$ ls zen_1.txt 
zen_1.txt
</pre>

#### Server 

<pre>
MacBook-Air:file-server anish$ gcc -o server server.c
MacBook-Air:file-server anish$ ./server
Client connected - IP address: [127.0.0.1] Port: [51734]
File sent: zen.txt
Client connected - IP address: [127.0.0.1] Port: [51799]
File uploaded successfully: zen_1.txt
</pre>


