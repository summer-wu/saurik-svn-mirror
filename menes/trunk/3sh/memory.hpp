#ifndef MEMORY_HPP
#define MEMORY_HPP

#define Overhead_ 2
#define Blocks_ (1024 - Overhead_ - 1)
void **blocks_[Blocks_];

_finline void *finish_(void **base, size_t size) {
    *base++ = (void *) size;
    *base++ = (void *) 1;

    /*if (base == (void *) 0x43009420)
        _breakpoint();*/
    //writef("0x%x+\n", base);
    return base;
}

void *operator new(size_t size) throw() {
    size = (size + sizeof(void *) - 1) / sizeof(void *) - 1;

    if (size == _not(size_t))
        return NULL;

    if (size > Blocks_) {
        void **base = (void **) map_((size + Overhead_ + 1) * sizeof(void *));
        return finish_(base, size);
    }

    size_t i;
    for (i = size; i != Blocks_; ++i)
        if (blocks_[i] != NULL) segment: {
            void **base = blocks_[i];
            blocks_[i] = (void **) *base;

            if (i - size < 4)
                return finish_(base, i);
            else {
                void **segment = base + size + Overhead_ + 1;
                size_t oppose = i - size - (Overhead_ + 1);
                *segment = blocks_[oppose];
                blocks_[oppose] = segment;

                return finish_(base, size);
            }
        }

    void **block = (void **) map_((Blocks_ + Overhead_ + 1) * sizeof(void *));

    i = Blocks_ - 1;
    *block = NULL;
    blocks_[i] = block;

    goto segment;
}

void *allocate(size_t size, const char *file, unsigned line) {
    //writef("%s(%d): allocate(%d)\n", file, line, size);
    return operator new(size);
}

#define allocate(size) allocate(size, __FILE__, __LINE__)

void take(void *object) {
    if (!object)
        return;

    void **base = (void **) object - Overhead_;
    ++*((intptr_t *) base + 1);
}

void operator delete(void *object) {
    if (IsReservedPointer(object))
        return;

    void **base = (void **) object - Overhead_;
    intptr_t size = * (intptr_t *) base;

    if (size > Blocks_)
        _syscall(munmap(base, (size + Overhead_ + 1) * sizeof(void *)));
    else {
        * (void **) base = blocks_[size];
        blocks_[size] = base;
    }
}

void drop_(void *object, void (*finalizer)(void *object)) {
    if (IsReservedPointer(object))
        return;

    void **base = (void **) object - Overhead_;
    if (--*((intptr_t *) base + 1))
        return;

    //writef("0x%x-\n", object);

    if (finalizer)
        finalizer(object);
    operator delete(object);
}

template <typename Type_>
struct Deconstruct_ {
    _finline static void Execute(void *object) {
        reinterpret_cast<Type_ *>(object)->~Type_();
    }

    _finline static void (*Get())(void *) {
        return &Deconstruct_<Type_>::Execute;
    }
};

template <typename Type_>
struct Deconstruct_<Type_ *> {
_finline static void (*Get())(void *) {
    return NULL;
} };

template <>
struct Deconstruct_<void> {
_finline static void (*Get())(void *) {
    return NULL;
} };

template <typename Type_>
_finline void (*GetDeconstructor_(Type_ *))(void *) {
    return Deconstruct_<Type_>::Get();
}

#define drop(object) drop_(object, GetDeconstructor_(object))

#endif//MEMORY_HPP
