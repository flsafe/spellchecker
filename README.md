## Toy spell checker 

  Implemented using C and Ruby on Ubuntu Linux, GCC 4.4.5 and
  Ruby 1.8.7
  
  + checker.c Implements a simple spell checker
  + test.rb   A script that generates misspelled words based on a set of rules
  + freqs.txt A list of words

## To compile the spell checker

    make

## To run the spell checker

    ./check

## To produce spelling errors

    ruby test.rb

## To feed spelling errors to the spell checker

    ruby test.rb | ./check
