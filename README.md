# Log-Parser C/C++
  
  log-parser : a comman line of parser to extract dau, revenue, orderId from log files in multiple folders, developed by C/C++.
  
How to compile log-parser ?
==================
   Install libcurl 
   
  `tar xvzf curl-7.48.0.tar` 
   
  ` cd curl-7.48.0` 
   
  ` make ; make install `

   Compile log-parser with Release mode
   
  `cd  log-parser` 
   
   ` vim  Log-Parser_KPI-Web/log-parser/nbproject/Makefile-Release.mk `
    
   `vim nbproject/Makefile-impl.mk`
   
  ` DEFAULTCONF=Release`
    
    Add -I/usr/local/include -L/usr/local/lib -lcurl
   
   `${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/testcodec___1 ${OBJECTFILES} ${LDLIBSOPTIONS} -I/usr/local/include -L/usr/local/lib -lcurl`
    
   `$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp -I/usr/local/include -L/usr/local/lib -lcurl`
   
  ` make `
