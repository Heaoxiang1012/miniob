#pragma once
#include <vector>

#include "rc.h"
#include "sql/stmt/stmt.h"
#include "sql/parser/parse_defs.h"

class Table;
class FieldMeta;

class AggreateStmt : public Stmt
{
public:

  AggreateStmt(Table *table,std::vector<std::string> funcs,std::vector<std::vector<const FieldMeta *>> attrs):
    table_(table),funcs_(funcs),attrs_(attrs){};
  
  ~AggreateStmt() override {
    table_ = nullptr;
    funcs_.clear();
    attrs_.clear();
  };

  Table *table() const { return table_; }

  StmtType type() const override { return StmtType::AGGREATE ; }

  std::vector<std::string> &get_funcs() { return funcs_; }
  std::vector<std::vector<const FieldMeta *>>  &get_attrs() { return attrs_; }

 public:
  static RC create(Db *db, const Aggreates &aggreate_sql, Stmt *&stmt);

private:
  Table *table_ = nullptr;
  std::vector<std::string> funcs_;
  std::vector<std::vector<const FieldMeta *> > attrs_;
};
