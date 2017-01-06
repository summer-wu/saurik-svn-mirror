#!/usr/bin/env bash
EXE="$1"
if [[ -z ${EXE:+@} ]]; then
    EXE=$(grep EXEPATH build-gnu.d/$(cat project.tmp).mk | sed -e "s/\(.*\) := \(.*\)/\2/")
fi
sed -e "s/.*\(0x[0-9a-fA-F]*\).*/\1/" | addr2line -Cfe $EXE | sed -e "
    s/ext::SharedBlock<\([^<>]*\), ext::NewAllocator>/ext::SharedBlock<\1>/g
    s/ext::StringVector<ext::SharedBlock<char> >/ext::String/g
    s/ext::Block<\([^<>]*\), ext::TypedAllocator<ext::NewAllocator> >/ext::Block<\1>/g
    s/ext::Vector<\([^<>]*\), ext::Block<\1> >/ext::Vector<\1>/g
    s/ext::Vector<ext::String>/ext::StringList/g
    s/etl::be::Bind<0, \([^<>]*\) ()(\([^<>,)]*\)\([^<>)]*\)), \2, [^<>]*>/etl::be::Bind<0, .., \2>/g
    s/etl::Cons<\([^<>]*\), etl::Null>/etl::Cons<\1>/g
    s/etl::Cons<\([^<>]*\), etl::Cons<\([^<>]*\)> > /etl::List<\1, \2>::Result/g
    s/ext::Handle<\([^<>]*\), ext::ReferenceManager<\1, etl::Bind1<ext::ReferenceAllocator> > >/ext::Handle<\1>/g
    s/ext::Map<ext::RedBlackSet<etl::Pair<\([^<>]*\), \([^<>]*\)>, \1, ext::First, ext::LessThan>, \1, \2>/ext::RedBlackMap<\1, \2>/g

    s/ext::Handle/_H/g
    s/ext::Vector/_L/g
    s/ext::StackReference/_S/g

    s/etl::ArgStruct<etl::List<[^<>]*>::Result>/etl::ArgStruct<..>/g
    s/etl::ArgStruct<etl::Cons<[^<>]*> >/etl::ArgStruct<..>/g
    s/etl::ArgStruct<[^<>]*>/etl::ArgStruct<..>/g

    s/\(.\)etl::\(CallThrough\|Function\)<[^<>]*>/\1etl::\2<..>/g
    s/etl::FunctionInvoke<etl::Function<..>, \([^<>]*\), _H<etl::FunctionValue<\1>, etl::FunctionManager<\1> > >/etl::FunctionInvoke<..>/g

    s/InvokeBind(etl::Function<..> const\*, etl::ArgStruct<..>)/InvokeBind(..)/g
    s/InvokeReference(etl::ArgStruct<..> const&)/InvokeReference(..)/g
    s/InvokeValue(etl::ArgStruct<..>)/InvokeValue(..)/g
    s/InvokeMember_(etl::Function<..> const\*, etl::ArgStruct<..>)/InvokeMember_(..)/g

    s/MemberCall(.*)/MemberCall(..)/g
"
