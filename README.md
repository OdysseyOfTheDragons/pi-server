# Pi server

A few years ago, with a friend, we tried to implement the
[Bailey-Borwein-Plouffe formula](https://en.wikipedia.org/wiki/Bailey%E2%80%93Borwein%E2%80%93Plouffe_formula)
in `C`, and we succeeded.

However, many of the ideas we had in mind to call it a full-fledged project
never saw the light as we parted ways during our studies.

It gnawed at my soul, therefore I will go back to this project and execute
what we both wanted at the time. However, a simple copy of the old project
wouldn't be an interesting project, so I will change the formula used to the
faster [Bellard's formula](https://en.wikipedia.org/wiki/Bellard%27s_formula).

This formula inherently does the same as the preceding one, but is a bit
faster. And when we want speed, this is a good idea.

## Ideas

My goal, at the end of the project, is to have an implementation for both
a server and clients that can compute the digits of $\pi$ together.

The server is linked to a database, where the base 16 digits are stored,
and listens for client connections. When a client connects, it asks to
compute a number of digits at some position. Then the client answers with
the base 16 digits.

The server can either store these digits in the database if they were unknown,
or use these to verify already computed digits. This prevents, to some extent,
a client or server fail (though I will try my utmost to prevent such failures).

Finally, a converter module will be able to convert the stored hexadecimal
digits inside the database to be in base 10 for human consumption.

### Database

My current idea for the database is both simple to understand and simple to
implement, but is has some major flaws.

The main idea is: a fixed size header (to store some metadata), a fixed size
relocation table (I'll get back to it later), and the data in a growing
section.

A base 16 digit fits inside 4 bits, so I can pack two digits inside a single
byte. Thus, a `uint64_t` can store 16 digits. This will be the basic block
of data the server will ask the clients to compute.

A fixed size block is also better for compilers to optimize when unrolling
loops. And I will use this to reduce the size overhead of the fixed size
relocation table.

To access a single position to get the digit, I can compute the 16 digits block
and the position within the block, and access it directly inside the file
(because the header is of fixed size, this is a fixed offset).

Now, onto the relocation table: I want to know quickly whether a block has been
computed and whether it has been verified. So a bitmap array is what I'll do.
A first bitmap tells whether the block has been computed (1 bit for 16 digits),
and a second bitmap tells whether the block has been verified (1 bit for 16
digits). This means I need only 2 bits for 16 digits, so a byte is enough for
64 digits! 10 billion digits can fit in a header of roughly 157 MB.

You can say this is much, but when you realize that 10 billion digits, with
two digits per byte, is 5 GB, 157 MB isn't that much of an overhead.

You may also say that if a computer only computes 16 digits at a time, the fact
I communicate through network is a performance bottleneck. And I agree!
On the first thousands of digits, the network speed will be the biggest
slowdown. But once we get into the higher n-th digits, the computational speed
will decrease tremendously, thus making the network speed less of a bottleneck.

Also, the end goal is to be able to have multiple computers running at the same
time, and this needs to go through the network...

_Note:_ I mentioned earlier that the header will allow to compute up to
10 billion digits. I want no fixed size limit. Thus, the header of the file
will specify _his_ own limit (specified when compiling the project). This means
that if I only want to compute 1 billion digits because I don't need more, and
that another one wants to compute 100 billion, both should be possible. I am
also thinking of an implementation that would allow the program to run
migrations on a database file (to change the limit) and that would allow the
user to specify the wanted size when launching the server rather than when
compiling the tool.

## What to do add in the following steps

I must finish to write the database implementation. To do so, I also need
to implement the exclusive mutexes on the file (concurrent reading, exclusive
writing).

To improve performance, I shall use `const`, `restrict`, `register`,
`inline`, `static` where possible.

There are a lot of places where I use magic numbers. Sometimes, an 8 is
to have a shift, sometimes this is the length of a 16-digit block. I need
to introduce `#define` to be clearer.

I also need to document the fact I use libraries (`openmp`, `mpfr`, `gmp`,
will I also use `libdivide`?).

Finally, I also need to add formatting to my Makefile, in order to have a
consistent style (and to have lines that fit my terminal width).
