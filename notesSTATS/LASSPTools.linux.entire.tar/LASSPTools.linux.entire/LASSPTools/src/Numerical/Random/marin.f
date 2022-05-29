c
c The following random number generators are from the paper by F. James
c "A review of pseudorandom number generators" 
c in Computer Physics Communications 60 (1990) pages 329-344
c

      Subroutine rmarin(ijkl)
c          Initializing routine for ranmar, must be called before
c          generating any pseudorandom numbers with ranmar. The input 
c          value should be in the range: 0 <= ijkl <= 900 000 000
      common/raset1/u(97),c,cd,cm,i97,j97
c     
c     This shows the correspondence between the simplified input seed ijkl
c     and the original Marsaglia-Zaman seeds i,j,k,l
c     To get the standard values in the Marsaglia-Zaman paper:
c        (i=12, j=34, k=56, l=78) set ijkl=54217137
c
      ij = ijkl/30082
      kl = ijkl - 30082*ij
      i = mod(ij/177, 177) + 2
      j = mod(ij, 177) + 2
      k = mod(kl/169,178) + 1
      l = mod(kl,169)
      do 2 ii = 1, 97
         s = 0.
         t = .5
         do 3 jj = 1, 24
            m = mod(mod(i*j,179)*k,179)
            i = j
            j = k
            k = m
            l = mod(53*l + 1, 169)
            if (mod(l*m,64).ge.32) s = s + t
 3       t = 0.5 * t
 2    u(ii) = s
      c = 362436./16777216.
      cd = 7654321./16777216.
      cm = 16777213./16777216.
      i97 = 97
      j97 = 33
      return 
      end

      subroutine ranmar(rvec,len)
c
c     Universal random number generator proposed by Marsaglia and Zaman
c     in report FSU-SCRI-87-50
c     slightly modified by F. James, 1988 to generate a vector
c     of pseudorandom numbers rvec of length LEN
c     and making the common block include everything needed to specify 
c     completely the state of the generator.
c
      dimension rvec(*)
      common/raset1/u(97),c,cd,cm,i97,j97
c
      do 100 ivec = 1, len
         uni = u(i97)-u(j97)
         if (uni.lt.0.) uni = uni + 1.
         u(i97) = uni
         i97 = i97-1
         if (i97.eq.0) i97 = 97
         j97 = j97-1
         if (j97.eq.0) j97 = 97
         c = c-cd
         if (c.lt.0.) c = c + cm
         uni = uni-c
         if (uni.lt.0.) uni = uni + 1.
         rvec(ivec) = uni
 100     continue
         return
         end
         

