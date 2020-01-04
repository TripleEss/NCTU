Homework 3: Write a A5/1 stream cipher encryptor/decryptor

For the A5/1 stream cipher algorithm, please refer to the video:
    https://www.youtube.com/watch?v=LgZAI3DdUA4

We provide you two cpp files as the skeleton.

    1. main.cpp
        a. The program entry point, the argument parsing, and
            the read/write of files are well-implemented.
        b. This file calls two declared but not implemeted functions of A5/1:
            - void* A51Initial(unsigned long long sessionKey,
                                     unsigned int frameCounter);
            - unsigned char A51GetByte(void* LFSR);
            So, you have to implement these two functions.
        c. You have to only modify one line, line 15, in this file
            to combine your implementation of A5/1.
            - If you are familiar with compile source files together, just delete it.
            - Otherwise, modify this line to include your <studentID>.cpp file.

    2. hw3.cpp
        a. You should rename this file to <studentID>.cpp (ex. 0123456.cpp)
        b. A51Initial amd A51GetByte should be implemented here.
            - Please read the comments to ensure your implementation
              of these functions use their arguments correctly.

Therefore, we suggest you use C/C++ to complete this homework.

For students using languages other than C/C++, please follow this guide:
    1. Email TA, along with your preferred language, to request the permission to do so.
    2. Make sure your program can be executed the same as our skeleton.
    ./your_prog [<Session Key> <Frame Counter> <Input Filename> <Output Filename>]
    3. Make sure your program can be run on linux1.cs.nctu.edu.tw without any modification.


Sample Input & Output

Sample 1.
    1. Download the original hw3.cpp file
    2. Run your program to encrypt hw3.cpp using following arguments
    ./your_prog 0x123456789abcdef 0x12345 hw3.cpp hw3.cpp.enc2
    3. The output hw3.cpp.enc2 should be identical to hw3.cpp.enc

Sample 2.
    1. Download encrypt.mp3
    2. Run your program without any argument
    ./your_prog
    3. The output decrypt.mp3 is a song you can find here
        https://archive.org/details/LastChristmas_614

We will test your program in linux1.cs.nctu.edu.tw.
It's your responsibility to assure yourself your code can be run on it correctly.

Grading Policy
    Pass sample tests: 60%
    Pass extra tests: 40%