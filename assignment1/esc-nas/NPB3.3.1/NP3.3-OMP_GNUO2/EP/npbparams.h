c CLASS = D
c  
c  
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the class of the NPB
c  in this directory. Do not modify it by hand.
c  
        character class
        parameter (class ='D')
        integer m
        parameter (m=36)
        logical  convertdouble
        parameter (convertdouble = .false.)
        character compiletime*11
        parameter (compiletime='28 Mar 2020')
        character npbversion*5
        parameter (npbversion='3.3.1')
        character cs1*8
        parameter (cs1='gfortran')
        character cs2*6
        parameter (cs2='$(F77)')
        character cs3*6
        parameter (cs3='(none)')
        character cs4*6
        parameter (cs4='(none)')
        character cs5*28
        parameter (cs5='-O2 -fopenmp -mcmodel=medium')
        character cs6*28
        parameter (cs6='-O2 -fopenmp -mcmodel=medium')
        character cs7*6
        parameter (cs7='randi8')
