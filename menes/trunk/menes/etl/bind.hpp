/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_ETL_TYPEBIND_HPP
#define MENES_ETL_TYPEBIND_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace etl {

template <template <typename> class Template_>
struct Bind1 {
    template <typename Arg0_>
    struct Template {
        typedef Template_<Arg0_> Result;
    };

    template <typename Arg0_>
    struct Arg0 {
        // XXX: might this should be named template?
        struct Type {
            typedef typename Template_<Arg0_>::Result Result;
        };
    };
};

template <template <typename, typename> class Template_>
struct Bind2 {
    template <typename Arg0_, typename Arg1_>
    struct Template {
        typedef Template_<Arg0_, Arg1_> Result;
    };

    template <typename Arg0_>
    struct Arg0 {
        template <typename Arg1_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_>::Result Result;
        };
    };

    template <typename Arg1_>
    struct Arg1 {
        template <typename Arg0_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_>::Result Result;
        };
    };
};

template <template <typename, typename, typename> class Template_>
struct Bind3 {
    template <typename Arg0_, typename Arg1_, typename Arg2_>
    struct Template {
        typedef Template_<Arg0_, Arg1_, Arg2_> Result;
    };

    template <typename Arg0_>
    struct Arg0 {
        template <typename Arg1_, typename Arg2_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_, Arg2_>::Result Result;
        };
    };

    template <typename Arg1_>
    struct Arg1 {
        template <typename Arg0_, typename Arg2_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_, Arg2_>::Result Result;
        };
    };

    template <typename Arg2_>
    struct Arg2 {
        template <typename Arg0_, typename Arg1_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_, Arg2_>::Result Result;
        };
    };
};

template <template <typename, typename, typename, typename> class Template_>
struct Bind4 {
    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_>
    struct Template {
        typedef Template_<Arg0_, Arg1_, Arg2_, Arg3_> Result;
    };

    template <typename Arg0_>
    struct Arg0 {
        template <typename Arg1_, typename Arg2_, typename Arg3_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_, Arg2_, Arg3_>::Result Result;
        };
    };

    template <typename Arg1_>
    struct Arg1 {
        template <typename Arg0_, typename Arg2_, typename Arg3_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_, Arg2_, Arg3_>::Result Result;
        };
    };

    template <typename Arg2_>
    struct Arg2 {
        template <typename Arg0_, typename Arg1_, typename Arg3_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_, Arg2_, Arg3_>::Result Result;
        };
    };

    template <typename Arg3_>
    struct Arg3 {
        template <typename Arg0_, typename Arg1_, typename Arg2_>
        struct Template {
            typedef typename Template_<Arg0_, Arg1_, Arg2_, Arg3_>::Result Result;
        };
    };
};

}

#endif//MENES_ETL_TYPEBIND_HPP
