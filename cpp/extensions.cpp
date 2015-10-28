#include <iostream>
#include "rocksdb/db.h"
#include "rocksdb/utilities/backupable_db.h"
#include "extensions.h"

using namespace std;
using rocksdb::DB;
using rocksdb::ColumnFamilyHandle;
using rocksdb::ReadOptions;
using rocksdb::Slice;
using rocksdb::Status;
using rocksdb::Iterator;
using rocksdb::BackupEngine;

// g++ -std=c++11 -shared -I/home/me/rocksdb/include/ -L/home/me/rocksdb -fpic extensions.cpp -lrocksdb -lbz2 -lpthread -lsnappy -lz -o libextensions.so

extern "C" {
  // all c structs copied as is from rocksdb source
  struct rocksdb_t{DB* rep;};
  struct rocksdb_readoptions_t{
    ReadOptions rep;
    Slice upper_bound; // stack variable to set pointer to in ReadOptions
  };
  struct rocksdb_column_family_handle_t{ColumnFamilyHandle* rep;};
  struct rocksdb_iterator_t{Iterator* rep;};
  struct rocksdb_backup_engine_t{BackupEngine* rep;};
}

// copied from c.cc rocksdb
static bool SaveError(char** errptr, const Status& s) {
  assert(errptr != nullptr);
  if (s.ok()) {
    return false;
  } else if (*errptr == nullptr) {
    *errptr = strdup(s.ToString().c_str());
  } else {
    // TODO(sanjay): Merge with existing error?
    // This is a bug if *errptr is not created by malloc()
    free(*errptr);
    *errptr = strdup(s.ToString().c_str());
  }
  return true;
}

void getIterators(
    rocksdb_readoptions_t* opts,
    rocksdb_t *db,
    rocksdb_column_family_handle_t** cfs,
    rocksdb_iterator_t** iters,
    int num,
    char** errptr) {
  vector<ColumnFamilyHandle*> cfs_vec(num);
  for (int i = 0; i < num; i++) {
    cfs_vec.push_back(cfs[i]->rep);
  }
  vector<Iterator*> res;

  Status status = db->rep->NewIterators(opts->rep,
      cfs_vec,
      &res);
  assert(res.size() == num);
  SaveError(errptr, status);
  if (*errptr != nullptr) {
    return;
  }

  for (int i = 0; i < num; i++) {
    iters[i] = new rocksdb_iterator_t;
    iters[i]->rep = res[i];
  }
}

void purge_old_backups(rocksdb_backup_engine_t* backup_engine,
    uint32_t num_backups_to_keep,
    char** errptr) {
  Status status = backup_engine->rep->PurgeOldBackups(num_backups_to_keep);
  SaveError(errptr, status);
}
