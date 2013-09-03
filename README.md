COMP590Tests
============

Tests for the programming assignments in COMP 590


### Requirements

- [CMake 2.6](http://www.cmake.org/) or higher

### Usage

In your directory structure that has all of your programming assignments, clone this repository into a separate "test" folder:

    $ cd COMP590-001
    $ ls
    pa0 pa1
    $ git clone https://github.com/Mokosha/COMP590Tests.git test
    $ ls
    pa0 pa1 test
    
Once you've cloned it, it's good to set up a separate build folder for all of the
tests. Then simply generate make files that will build both your class files and
the tests for the homework. This should be fairly straightforward:

    $ cd COMP590-001
    $ mkdir test-build
    $ cd test-build
    $ cmake ../test -DCMAKE_BUILD_TYPE=Debug
    $ make
    $ make test
    ------- All tests passed =) -------
    
Please feel free to submit pull requests for new tests to be added!
