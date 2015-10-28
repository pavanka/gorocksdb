package gorocksdb

// #cgo LDFLAGS: -L./cpp -lextensions
// #cgo CFLAGS: -I./cpp
// #include <stdlib.h>
// #include "extensions.h"
import "C"

import (
	"errors"
	"unsafe"
)

func NewIterators(opts *ReadOptions, db *DB, cfs []*ColumnFamilyHandle) ([]*Iterator, error) {
	size := len(cfs)
	cfsC := make([]*C.rocksdb_column_family_handle_t, size)
	for _, cf := range cfs {
		cfsC = append(cfsC, cf.c)
	}

	iters := make([]*C.rocksdb_iterator_t, size)
	var cErr *C.char
	C.getIterators(
		opts.c,
		db.c,
		&cfsC[0],
		&iters[0],
		C.int(size),
		&cErr,
	)
	if cErr != nil {
		defer C.free(unsafe.Pointer(cErr))
		return nil, errors.New(C.GoString(cErr))
	}

	var iterators []*Iterator
	for _, iter := range iters {
		iterators = append(iterators, NewNativeIterator(iter))
	}
	return iterators, nil
}
