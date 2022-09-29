#include "common/log/log.h"
#include "sql/stmt/aggreate_stmt.h"
#include "storage/common/db.h"
#include "storage/common/table.h"


RC AggreateStmt::create(Db *db, const Aggreates &aggreate_sql, Stmt *&stmt)
{
  const char *table_name = aggreate_sql.relation;
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", 
             db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  if(aggreate_sql.attr_num != aggreate_sql.aggreation_num){
    LOG_WARN("attr_num is no equal to aggreation_num!");
    return RC::GENERIC_ERROR;
  }

  const TableMeta &table_meta = table->table_meta();

  std::vector<std::string> funcs;
  std::vector<std::vector<const FieldMeta *>> attrs;

  for (int i = aggreate_sql.attr_num - 1 ; i >= 0; --i) {  //是否应该逆序？
    char *aggreate_attr = aggreate_sql.attributes[i];
    char *aggreate_func = aggreate_sql.aggreations[i];

    std::vector<const FieldMeta *> query_fields;
    
    if(0 == strcmp(aggreate_attr,"*") &&  strcmp(aggreate_func,"COUNT") != 0){
      LOG_WARN("attr is * but aggregate func is not count .");
      return RC::GENERIC_ERROR;
    }

    if (0 == strcmp(aggreate_attr, "*")) {
      const int field_num = table_meta.field_num();
      for (int i = table_meta.sys_field_num(); i < field_num; i++) {
        query_fields.push_back(table_meta.field(i));
      }
    } else {
      auto t = table_meta.field(aggreate_attr);
      if(t == nullptr){
        LOG_WARN("no such field .");
        return RC::GENERIC_ERROR;
      }
      query_fields.push_back(t);
    }

    funcs.push_back(aggreate_func);
    attrs.push_back(query_fields);
  }

  stmt = new AggreateStmt(table,funcs,attrs);
  return RC::SUCCESS;
}