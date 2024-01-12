# TEST CASES FOR EVALUATION

## Steps to run
- Clone this repo
- Place the mems.h or any other header file along this repository. i.e. mems.h and MeMS-Test-Cases in same folder.
- Set Starting Virtual Address=0 & PAGE_SIZE=4096
- run following command to execute test cases
```
$ cd MeMS-Test-Cases/<test-type>
$ make
$ ./test<test-case-number>
```
- One example is:
```
$ cd MeMS-Test-Cases/easy
$ make
$ ./test4
```
