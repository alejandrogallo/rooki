#!/usr/bin/env rooki
! rooki:fortran_flags -pedantic -Wall -std=f95
! rooki:spell f95 $fortran_flags $f -o $bin

PROGRAM MAIN
IMPLICIT NONE
  WRITE(*,*) "Hello World"
END PROGRAM MAIN
