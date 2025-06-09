📆 2025-06-05 🕓 10:25
Tags: #testing 

---
# Start the tester (ubuntu_tester)

` ./tester/ubuntu_tester http://127.0.0.1:8080`
## This shows up when the tester is started

```shell
Welcome in this little webserver tester.
Passing the test here is the minimum before going to an evaluation.

press enter to continue

THIS TEST IS NOT MEANT TO BE THE ONLY TEST IN THE EVALUATION!!!

press enter to continue

Before starting please follow the next few steps (files content can be anything and will be shown to you by the test):
- Download the cgi_test executable on the host
- Create a directory YoupiBanane with:
        -a file name youpi.bad_extension
        -a file name youpi.bla
        -a sub directory called nop
                -a file name youpi.bad_extension in nop
                -a file name other.pouic in nop
        -a sub directory called Yeah
                -a file name not_happy.bad_extension in Yeah
press enter to continue

Setup the configuration file as follow:
- / must answer to GET request ONLY
- /put_test/* must answer to PUT request and save files to a directory of your choice
- any file with .bla as extension must answer to POST request by calling the cgi_test executable
- /post_body must answer anything to POST request with a maxBody of 100
- /directory/ must answer to GET request and the root of it would be the repository YoupiBanane and if no file are requested, it should search for youpi.bad_extension files

press enter to continue

Before starting please verify that the server is launched
press enter to continue
```

- webserver must be running
- cgi-tester is executed by the tester
- some directories and files have to been created before the tester can run
- the tester sends a Request without proper `CRLF`
	- Output from our Server: `Missing CRLF after final 0 chunk`
	- someone else mentioned this also in slack
	- if i deactivate the if statement for this check, then it works
		- but is this not technically incorrect?
- then the tester tries a Post with size 0, but we already redirected him to index.html
```
- Test POST http://127.0.0.1:8080/ with a size of 0
FATAL ERROR ON LAST TEST: bad status code
```


```ad-summary
- we have to rebuild some stuff to use it
- the missing CRLF is probably wrong
- other tools are available (like siege, burpsuite, firefox dev tools, our own test....)
```

