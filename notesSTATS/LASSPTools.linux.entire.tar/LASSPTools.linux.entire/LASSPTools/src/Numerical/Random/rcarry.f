      subroutine rcarry(rvec,lenv)
c     
c     Portable pseudorandom number generator with period of
c     about (1/48)*(2**24)**24 = 2**570 = 10**171
c
c     author F. James, CERN, 1989.
c     algorithm due to: G. Marsaglia and A Zaman
c
      common/rcardat/seeds(24),i24,j24
      dimension rvec(lenv)
     
      parameter (twop24 = 16777216.)
      parameter (twom24 = 1./twop24)
c
c     The basic generator algorithm only
c
      do 100 ivec =1, lenv
         uni = seeds(i24) - seeds(j24) - carry
         if (uni.lt.0.) then
            uni = uni + 1.0
            carry = twom24
         else
            carry = 0.
         endif
         seeds(i24) = uni
         i24 = i24 - 1
         if (i24.eq.0) i24 = 24
         j24 = j24 - 1
         if (j24.eq.0) j24 = 24
         rvec(ivec) = uni
 100     continue
      return
      end


c
c     Routine to set the seeds for the generator.
c
      subroutine rcarryset(seedv)
      dimension seedv(24)
      common/rcardat/seeds(24),i24,j24

      do 1 i=1, 24
         seeds(i) = seedv(i)
 1    continue
      i24 = 12
      j24 = 24
      return
      end






