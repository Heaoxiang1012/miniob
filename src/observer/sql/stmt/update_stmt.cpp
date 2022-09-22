/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/db.h"
#include "storage/common/table.h"

UpdateStmt::UpdateStmt(Table *table,FilterStmt *filter_stmt,const char *attribute_name, const Value value,const int value_amount)
  : table_ (table), filter_stmt_(filter_stmt),attribute_name_(attribute_name), value_(value), value_amount_(value_amount) {}

RC UpdateStmt::create(Db *db, const Updates &update_sql, Stmt *&stmt)
{
  if (nullptr == db){
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  } 

  const char *table_name = update_sql.relation_name;
  if(nullptr == table_name){
    LOG_WARN("invalid argument. table_name=%p", 
             table_name);
    return RC::INVALID_ARGUMENT;
  }

  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));

  // col val compare ?
  const Value value = update_sql.value ;
  const int value_amount = 1;
  const char *attribute_name = update_sql.attribute_name;

  // 是否更新不存在的列判断
  const FieldMeta *field_meta = table->table_meta().field(attribute_name);
  if (nullptr == field_meta) {
    LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), attribute_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  // 判断列与值是否能正确赋值
  for (int i = 0; i < value_amount; ++i) {
    const AttrType field_type = field_meta->type();
    const AttrType value_type = value.type;
    if (field_type != value_type) { // TODO try to convert the value type to field type
      LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d", 
               table_name, field_meta->name(), field_type, value_type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db, table, &table_map,
			     update_sql.conditions, update_sql.condition_num, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  stmt = new UpdateStmt(table,filter_stmt,attribute_name, value,value_amount);
  return rc;
}
