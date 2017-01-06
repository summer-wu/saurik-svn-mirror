if [ "${CFG_EXECUTABLE_LLVM_CONFIG+@}" ]; then
    mak:build *

    c++:gcc-flags hi-ccxx-flags -DAPI_HAS_LLVM

    IFS=$' \t\n'
    c++:gcc-flags hi-ccxx-flags -I$(llvm-config --includedir)

    IFS=$' \t\n'
    llvm_libs=($(llvm-config --libs))

    llvm_ldflag=0
    llvm_ldflags=()

    llvm_object=0
    llvm_objects=()

    for lib in "${llvm_libs[@]}"; do
        if [[ $lib == -* ]]; then
            llvm_ldflags[llvm_ldflag++]="$lib"
        else
            llvm_objects[llvm_object++]="$lib"
        fi
    done

    #IFS=$' \t\n'
    #c++:gcc-flags hi-ld-flags $(llvm-config --ldflags) "${llvm_ldflags[@]}"

    #mak:build "${llvm_objects[@]}"
fi
