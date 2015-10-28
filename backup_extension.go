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

func PurgeOldBackups(engine *BackupEngine, num uint) error {
	var cErr *C.char
	C.purge_old_backups(engine.c, C.uint32_t(num), &cErr)
	if cErr == nil {
		return nil
	}

	C.free(unsafe.Pointer(cErr))
	return errors.New(C.GoString(cErr))
}
