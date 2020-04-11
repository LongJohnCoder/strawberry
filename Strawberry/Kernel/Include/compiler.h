// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef COMPILER_H
#define COMPILER_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


#define likely(x)		__builtin_expect((x), 1)
#define unlikely(x)		__builtin_expect((x), 0)


//--------------------------------------------------------------------------------------------------//

#endif