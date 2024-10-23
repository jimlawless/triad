# triad
An esoteric language based on three-letter identifiers

I like to write a lot of mini interpreters and compilers.  Most are only seen by a few friends. One day, I wondered what a language based on three-character identifiers might look like ... or more particularly, three-letter identifiers.  Rather than implementing hashmaps and such in C, I thought this technique might have some merit.  You can think of the identifiers as being a base-26 number with the lower-case alphabet as the set of valid characters.  The function three_three2int() in triad.c illustrates the conversion of these three letters to an int.

Folks used a technique like this to compress three-letter assembly-language operands into two-byte identifiers to save space.  I just used them as an index to an array of 17576 (26 to the 3rd power) char pointers.  The only ID's used in this mini version of Triad are for procedures ( "procs" ) ... both user-defined procs and built-in procs.  I didn't implement variables.  It's basically just a language that can display stuff and that can call subroutines. 

The output of this program should be:

    Hi, everyone.  This is "Triad".
    You're going to get greeted three times.
    Inside of greeting word.
    Inside of greeting word.
    Inside of greeting word.

It looks like there's already an esolang called Triad, so I hope this doesn't cause too much confusion.
