#!/usr/bin/env rooki
! rooki:fortran_flags -pedantic -Wall -x f95
! rooki:spell f95 $fortran_flags $f -o $bin

program main
implicit none
  write(*,*) "Hello World rooki"
end program main
