## Spell Checker

  Implemented in C and Ruby on Ubuntu Linux GCC 4.4.5 and
  Ruby 1.8.7
  
  + checker.c Implements a simple spell checker
  + test.rb   A script that generates mispelled words

## To comile spell checker

    make

## To run spell checker

    ./check

## To produce test spelling errors

    ruby test.rb

## To run test spelling errors on spell checker

    ruby test.rb | ./check
