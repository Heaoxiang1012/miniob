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
// Created by Meiyi & Longda on 2021/4/13.
//

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/lang/defer.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "sql/expr/tuple.h"
#include "sql/operator/table_scan_operator.h"
#include "sql/operator/index_scan_operator.h"
#include "sql/operator/predicate_operator.h"
#include "sql/operator/delete_operator.h"
#include "sql/operator/update_operator.h"
#include "sql/operator/project_operator.h"
#include "sql/stmt/stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/table.h"
#include "storage/common/field.h"
#include "storage/index/index.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"
#include "util/date.h"

using namespace common;

void dfs(std::vector<std::vector<std::string>> &tables_record,int count,std::vector<std::string> &ans_record,std::string temp){
  std::vector<std::string> current = tables_record[count];
  for(auto str : current){
    if(count + 1 < tables_record.size()){
      dfs(tables_record, count + 1, ans_record, temp  +str+ "|");
    } else {
      ans_record.push_back(temp  + str );
    }
  }
}

//RC create_selection_executor(
//   Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag)
{}

//! Destructor
ExecuteStage::~ExecuteStage()
{}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag)
{
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties()
{
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize()
{
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup()
{
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event)
{
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context)
{
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event)
{
  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);
  SessionEvent *session_event = sql_event->session_event();
  Stmt *stmt = sql_event->stmt();
  Session *session = session_event->session();
  Query *sql = sql_event->query();

  if (stmt != nullptr) {
    switch (stmt->type()) {
    case StmtType::SELECT: {
      do_select(sql_event);
    } break;
    case StmtType::INSERT: {
      do_insert(sql_event);
    } break;
    case StmtType::UPDATE: {
      do_update(sql_event);
      // do_update((UpdateStmt *)stmt, session_event);
    } break;
    case StmtType::DELETE: {
      do_delete(sql_event);
    } break;
    }
  } else {
    switch (sql->flag) {
    case SCF_HELP: {
      do_help(sql_event);
    } break;
    case SCF_CREATE_TABLE: {
      do_create_table(sql_event);
    } break;
    case SCF_CREATE_INDEX: {
      do_create_index(sql_event);
    } break;
    case SCF_SHOW_TABLES: {
      do_show_tables(sql_event);
    } break;
    case SCF_DESC_TABLE: {
      do_desc_table(sql_event);
    } break;
    case SCF_DROP_TABLE: {
      do_drop_table(sql_event);
    } break;
    case SCF_DROP_INDEX: {
      do_drop_index(sql_event);
    } break;
    case SCF_LOAD_DATA: {
      default_storage_stage_->handle_event(event);
    } break;
    case SCF_SYNC: {
      RC rc = DefaultHandler::get_default().sync();
      session_event->set_response(strrc(rc));
    } break;
    case SCF_BEGIN: {
      session_event->set_response("SUCCESS\n");
    } break;
    case SCF_COMMIT: {
      Trx *trx = session->current_trx();
      RC rc = trx->commit();
      session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
    } break;
    case SCF_ROLLBACK: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->rollback();
      session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
    } break;
    case SCF_EXIT: {
      // do nothing
      const char *response = "Unsupported\n";
      session_event->set_response(response);
    } break;
    default: {
      LOG_ERROR("Unsupported command=%d\n", sql->flag);
    }
    }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right)
{
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}

void print_tuple_header(std::ostream &os, const ProjectOperator &oper,bool is_mul=false)
{
  const int cell_num = oper.tuple_cell_num();
  const TupleCellSpec *cell_spec = nullptr;
  for (int i = 0; i < cell_num; i++) {
    oper.tuple_cell_spec_at(i, cell_spec);
    if (i != 0) {
      os << " | ";
    }
    if (cell_spec->alias()) {
      os << cell_spec->alias();
    }
  }

  if (cell_num > 0) {
    os << '\n';
  }
}

std::string tuple_to_string(const Tuple &tuple)
{
  std::string str = "";
  TupleCell cell;
  RC rc = RC::SUCCESS;
  bool first_field = true;
  for (int i = 0; i < tuple.cell_num() ; ++i) {
    rc = tuple.cell_at(i, cell);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch field of cell. index=%d, rc=%s", i, strrc(rc));
      break;
    }
    if (!first_field) {
      str += " ";
    } else {
      first_field = false;
    }
    cell.to_string(str);
  }
  return str;
}

void tuple_to_string(std::ostream &os, const Tuple &tuple)
{
  TupleCell cell;
  RC rc = RC::SUCCESS;
  bool first_field = true;
  for (int i = 0; i < tuple.cell_num(); i++) {
    rc = tuple.cell_at(i, cell);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch field of cell. index=%d, rc=%s", i, strrc(rc));
      break;
    }

    if (!first_field) {
      os << " | ";
    } else {
      first_field = false;
    }
    cell.to_string(os);
  }
}

IndexScanOperator *try_to_create_index_scan_operator(FilterStmt *filter_stmt)
{
  const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();
  if (filter_units.empty() ) {
    return nullptr;
  }

  // 在所有过滤条件中，找到字段与值做比较的条件，然后判断字段是否可以使用索引
  // 如果是多列索引，这里的处理需要更复杂。
  // 这里的查找规则是比较简单的，就是尽量找到使用相等比较的索引
  // 如果没有就找范围比较的，但是直接排除不等比较的索引查询. (你知道为什么?)
  const FilterUnit *better_filter = nullptr;
  for (const FilterUnit * filter_unit : filter_units) {
    if (filter_unit->comp() == NOT_EQUAL) {
      continue;
    }

    Expression *left = filter_unit->left();
    Expression *right = filter_unit->right();
    if (left->type() == ExprType::FIELD && right->type() == ExprType::VALUE) {
    } else if (left->type() == ExprType::VALUE && right->type() == ExprType::FIELD) {
      std::swap(left, right);
    }
    FieldExpr &left_field_expr = *(FieldExpr *)left;
    const Field &field = left_field_expr.field();
    const Table *table = field.table();
    Index *index = table->find_index_by_field(field.field_name());
    if (index != nullptr) {
      if (better_filter == nullptr) {
        better_filter = filter_unit;
      } else if (filter_unit->comp() == EQUAL_TO) {
        better_filter = filter_unit;
    	break;
      }
    }
  }

  if (better_filter == nullptr) {
    return nullptr;
  }

  Expression *left = better_filter->left();
  Expression *right = better_filter->right();
  CompOp comp = better_filter->comp();
  if (left->type() == ExprType::VALUE && right->type() == ExprType::FIELD) {
    std::swap(left, right);
    switch (comp) {
    case EQUAL_TO:    { comp = EQUAL_TO; }    break;
    case LESS_EQUAL:  { comp = GREAT_THAN; }  break;
    case NOT_EQUAL:   { comp = NOT_EQUAL; }   break;
    case LESS_THAN:   { comp = GREAT_EQUAL; } break;
    case GREAT_EQUAL: { comp = LESS_THAN; }   break;
    case GREAT_THAN:  { comp = LESS_EQUAL; }  break;
    default: {
    	LOG_WARN("should not happen");
    }
    }
  }


  FieldExpr &left_field_expr = *(FieldExpr *)left;
  const Field &field = left_field_expr.field();
  const Table *table = field.table();
  Index *index = table->find_index_by_field(field.field_name());
  assert(index != nullptr);

  ValueExpr &right_value_expr = *(ValueExpr *)right;
  TupleCell value;
  right_value_expr.get_tuple_cell(value);

  // LOG_WARN("[BUG]: right_value_expr , value : %s,%d",value.data(),value.attr_type());

  int32_t right_value_string_to_int32 = -1;
  string_to_date(value.data(),right_value_string_to_int32);

  value.set_data((char *)&right_value_string_to_int32);
  value.set_type(AttrType::DATES);

  const TupleCell *left_cell = nullptr;
  const TupleCell *right_cell = nullptr;
  bool left_inclusive = false;
  bool right_inclusive = false;

  switch (comp) {
  case EQUAL_TO: {
    left_cell = &value;
    right_cell = &value;
    left_inclusive = true;
    right_inclusive = true;
  } break;

  case LESS_EQUAL: {
    left_cell = nullptr;
    left_inclusive = false;
    right_cell = &value;
    right_inclusive = true;
  } break;

  case LESS_THAN: {
    left_cell = nullptr;
    left_inclusive = false;
    right_cell = &value;
    right_inclusive = false;
  } break;

  case GREAT_EQUAL: {
    left_cell = &value;
    left_inclusive = true;
    right_cell = nullptr;
    right_inclusive = false;
  } break;

  case GREAT_THAN: {
    left_cell = &value;
    left_inclusive = false;
    right_cell = nullptr;
    right_inclusive = false;
  } break;

  default: {
    LOG_WARN("should not happen. comp=%d", comp);
  } break;
  }

  IndexScanOperator *oper = new IndexScanOperator(table, index,
       left_cell, left_inclusive, right_cell, right_inclusive);

  LOG_INFO("use index for scan: %s in table %s", index->index_meta().name(), table->name());
  return oper;
}

RC ExecuteStage::do_select(SQLStageEvent *sql_event)
{
  SelectStmt *select_stmt = (SelectStmt *)(sql_event->stmt());
  SessionEvent *session_event = sql_event->session_event();
  RC rc = RC::SUCCESS;
  std::stringstream ss;

  if (select_stmt->tables().size() != 1) {
    // 1.读出每张表的所有记录，并按string存放
    // 2.按条件删;

    for (int i = 0; i < select_stmt->query_fields().size() ; ++i) {
      const Field &field = select_stmt->query_fields()[i];
      if(i != 0){
          ss << " | ";
        }
      ss << field.table_name();
      ss << '.';
      ss << field.field_name();
    }
    ss << '\n';
    auto tables_size = select_stmt->tables().size();
    
    std::vector<std::vector<std::string>> table_records;
    std::vector<std::vector<char *>> all_table_records_data;

    for (int i = tables_size - 1; i >=0 ; --i) {
      Operator *scan_oper = nullptr;
      if (nullptr == scan_oper) {
        scan_oper = new TableScanOperator(select_stmt->tables()[i]);
      }
      PredicateOperator pred_oper(nullptr);//select_stmt->filter_stmt()
      pred_oper.add_child(scan_oper);
      ProjectOperator project_oper;
      project_oper.add_child(&pred_oper);
      
      auto table_meta = (select_stmt->tables()[i])->table_meta();
      for (int j = table_meta.sys_field_num() ; j < table_meta.field_metas()->size();++j) {
        const FieldMeta *field_meta = table_meta.field(j);
        project_oper.add_projection(select_stmt->tables()[i], field_meta); //read all records
      }

      rc = project_oper.open();
      std::vector<std::string> _temp;
      std::vector<char *> _temp_record;
      while ((rc = project_oper.next()) == RC::SUCCESS) {
        Tuple *tuple = project_oper.current_tuple();  // projectTuple
        if (nullptr == tuple) {
          rc = RC::INTERNAL;
          LOG_WARN("failed to get current record. rc=%s", strrc(rc));
          break;
        }
        _temp.push_back(tuple_to_string(*tuple));
        // read all record;
      }
      table_records.push_back(_temp);
      all_table_records_data.push_back(_temp_record);
    }

    // std::vector<int> table_offset(tables_size);
    // for (int i = tables_size - 1; i >= 0; --i) {
    //   if (i == tables_size - 1) table_offset[i] = 0;
    //   else {
    //     auto offset = (select_stmt->tables()[i])->table_meta().record_size_without_sys();
    //     table_offset[i] = table_offset[i+1] + offset;
    //   }
    // }

    

    // for (int i = table_offset.size() - 1; i >= 0; --i) {
    //   std::cout << table_offset[i] << std::endl;
    // }

    // for (int i = 0; i < table_records.size();++i) {
    //   for (int j = 0; j < table_records[i].size();++j) {
    //     std::cout << table_records[i][j] << std::endl;
    //   }
    // }

    std::vector<std::string> total_record;
    int tables_record_size = table_records.size();
    dfs(table_records, 0, total_record, "");

    CompositeConditionFilter compositeConditionFilter;
    // std::vector<DefaultConditionFilter*> defaultConditionFilters;
    const ConditionFilter **defaultConditionFilters = new const ConditionFilter*[select_stmt->filter_stmt()->filter_units().size()];

    int _temp = 0;
    for (const FilterUnit *filter_unit :
         select_stmt->filter_stmt()->filter_units()) {
      DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
      ConDesc left, right;
      AttrType attrtype = AttrType::UNDEFINED;
      if (FieldExpr *p_left = dynamic_cast<FieldExpr *>(filter_unit->left())) {
        left.is_attr = true;

        int index = 0;
        while(index < select_stmt->tables().size() && strcmp(p_left->field().table_name(),(select_stmt->tables()[index])->name()) != 0)
          index++;
        
        auto table_metas = (select_stmt->tables()[index])->table_meta();
        int attr_index = table_metas.sys_field_num();
        while (attr_index < table_metas.field_num() && strcmp(p_left->field().field_name(), (table_metas.field(attr_index))->name()) != 0) 
          attr_index++;

        index = select_stmt->tables().size() - index - 1;
        attr_index = attr_index - table_metas.sys_field_num(); // 减去sys field num 才是真实的 第几个 空格前的数据（字符串）

        left.attr_length = index; //表示找到第几个'|' 
        left.attr_offset = attr_index; // 表示找到第几个空格 第0个空格表示第1个attr
        attrtype = p_left->field().attr_type();
        
    

      } else if (ValueExpr *p_left_v =
                     dynamic_cast<ValueExpr *>(filter_unit->left())) {
        // left.value = (void *)(p_left_v->get_tuple_cell_data());
        left.value = p_left_v->get_tuple_value();
      }

      if(FieldExpr *p_right = dynamic_cast<FieldExpr*>(filter_unit->right())){
        right.is_attr = true;
        int index = 0;
        while(index < select_stmt->tables().size() && strcmp(p_right->field().table_name(),(select_stmt->tables()[index])->name()) != 0)
          index++;
        
        auto table_metas = (select_stmt->tables()[index])->table_meta();
        int attr_index = table_metas.sys_field_num();

        while (attr_index < table_metas.field_num() && strcmp(p_right->field().field_name(), (table_metas.field(attr_index))->name()) != 0) 
          attr_index++;

        index = select_stmt->tables().size() - index - 1;
        attr_index = attr_index - table_metas.sys_field_num(); // 减去sys field num 才是真实的 第几个 空格前的数据（字符串）

        right.attr_length = index; //表示找到第几个'|' 
        right.attr_offset = attr_index; // 表示找到第几个空格 第0个空格表示第1个attr
        
        // LOG_WARN("right.attr_length = %d,right.attr_offset = %d", index,
        //          attr_index);
        attrtype = p_right->field().attr_type();
      } else if(ValueExpr *p_right_v = dynamic_cast<ValueExpr *>(filter_unit->right())){
          right.value = p_right_v->get_tuple_value();
      }
      condition_filter->init(left, right, attrtype, filter_unit->comp()); // 是否要用CHARS
      // defaultConditionFilters.push_back(condition_filter);
      defaultConditionFilters[_temp++] = condition_filter;
    }

    
    compositeConditionFilter.init(
        defaultConditionFilters,
        select_stmt->filter_stmt()->filter_units().size());
    // LOG_WARN("composite filter init finish .");

    std::vector<std::string> ans_records;
    for (auto record_string : total_record) {
      
      if (compositeConditionFilter.filter(record_string)) {
        ans_records.push_back(record_string);
      }
    }

    // do output ;
    std::vector<std::pair<int, int>> indice;

    for (int i = 0; i < select_stmt->query_fields().size(); ++i) {
      const Field &field = select_stmt->query_fields()[i];
      std::vector<int> _index;

      int table_index = 0;
      while(table_index < select_stmt->tables().size() && strcmp(field.table_name(),(select_stmt->tables()[table_index])->name()) != 0)
          table_index++;
        
      auto table_metas = (select_stmt->tables()[table_index])->table_meta();
      int attr_index = table_metas.sys_field_num();
      while (attr_index < table_metas.field_num() && strcmp(field.field_name(), (table_metas.field(attr_index))->name()) != 0) 
        attr_index++;

      table_index = select_stmt->tables().size() - table_index - 1;
      attr_index = attr_index - table_metas.sys_field_num(); // 减去sys field num 才是真实的 第几个 空格前的数据（字符串）

      indice.push_back(std::make_pair(table_index,attr_index));
    }

    // for(auto it:indice){
    //   LOG_WARN("TABLE_OFFSET : %d , ATTR_OFFSET : %d", it.first, it.second);
    // }
    // LOG_WARN("RECORDS SIZE : %d", ans_records.size());

    std::vector<std::string> output_records;
    for (auto item : ans_records) {
      std::string output_record = "";
      int end_indice = 0;
      for (auto it : indice) {
        int index = 0;
        int table_count = 0;
        while(index < item.size() && table_count != it.first)
        {
          if (item[index] == '|') table_count++;
          index++;
        }
        int attr_cout = 0;
        while(index < item.size() && attr_cout != it.second)
        {
          if (item[index] == ' ') attr_cout++;
          index++;
        }

        while(index < item.size() && item[index] != ' ' && item[index] != '|')
        {
          output_record += item[index];
          index++;
        }

        if(end_indice != indice.size() - 1) 
          output_record += " | ";
        end_indice++;
      }
      output_records.push_back(output_record);
    }

    for (auto item : output_records) {
      ss << item; 
      ss << '\n';
    }
    
    // for (auto item : ans_records) {
    //   ss << item;
    // }

    // LOG_WARN("select more than 1 tables is not supported");
    // rc = RC::UNIMPLENMENT;
    // return rc;

    // Operator *scan_oper = try_to_create_index_scan_operator(nullptr); FIX BUG TODO 
    
    // for (const Field &field : select_stmt->query_fields()) {  
    //     project_oper.add_projection(field.table(), field.meta());
    // }

    // rc = project_oper.open();
    // if (rc != RC::SUCCESS) {
    //   LOG_WARN("failed to open operator");
    //   return rc;
    // }

    // // print_tuple_header(ss, project_oper);
    // // output tuple head : (冗余度高)

    // while ((rc = project_oper.next()) == RC::SUCCESS) {

    //   Tuple *tuple = project_oper.current_tuple();  // 

    //   if (nullptr == tuple) {
    //     rc = RC::INTERNAL;
    //     LOG_WARN("failed to get current record. rc=%s", strrc(rc));
    //     break;
    //   }

    //   tuple_to_string(ss, *tuple);
    //   ss << std::endl;
    // }

    // if (rc != RC::RECORD_EOF) {
    //   LOG_WARN("something wrong while iterate operator. rc=%s", strrc(rc));
    //   project_oper.close();
    // } else {
    //   rc = project_oper.close();
    // }


    // for (int i = select_stmt->tables().size() -1 ; i >= 0 ; --i) {
      
      
      
    //   auto table_meta = (select_stmt->tables()[i])->table_meta();

    //   for (int j = table_meta.sys_field_num() ; j < table_meta.field_metas()->size();++j) {
    //     const FieldMeta *field_meta = table_meta.field(j);
    //     project_oper.add_projection(select_stmt->tables()[i], field_meta);
    //   }
    //   // for (const Field &field : select_stmt->query_fields()) {
    //   //   if(strcmp(field.table_name(),(select_stmt->tables()[i])->name()))
    //   //   {
    //   //     project_oper.add_projection(field.table(), field.meta());
    //   //   }
    //   // }
    //   rc = project_oper.open();
    //   std::vector<std::string> _temp;
    //   while ((rc = project_oper.next()) == RC::SUCCESS) {
    //     Tuple *tuple = project_oper.current_tuple();  // projectTuple
    //     if (nullptr == tuple) {
    //       rc = RC::INTERNAL;
    //       LOG_WARN("failed to get current record. rc=%s", strrc(rc));
    //       break;
    //     }
    //     _temp.push_back(tuple_to_string(*tuple)); //
    //   }
    //   tuples_to_string.push_back(_temp);
    // }

    
    // if(select_stmt->filter_stmt()->filter_units().size() == 0){
    //   size_t max_len = 0;
    //   for(auto item:tuples_to_string){
    //     max_len = std::max(item.size(), max_len);
    //   }
    //   for (size_t j = 0; j < max_len;++j) {
    //     for (size_t k = 0; k < tuples_to_string.size();++k) {
    //       if (tuples_to_string[k].size() < j){

    //       } 
    //     }
    //   }
    //   ss << '\n';
    // }else {
    //   // TODO pick with condition ;

    // }

    // get all attr in all table ;
    // LOG_WARN("[tuples size : %d] ", tuples_to_string.size());
    // for (auto item : tuples_to_string) {
    //   std::cout << item << std::endl;
    // }

  } else {
    // Operator *scan_oper = try_to_create_index_scan_operator(select_stmt->filter_stmt());
    Operator *scan_oper = nullptr;
    if (nullptr == scan_oper) {
      scan_oper = new TableScanOperator(select_stmt->tables()[0]);
    }

    DEFER([&] () {delete scan_oper;}); //callback function

    PredicateOperator pred_oper(select_stmt->filter_stmt());
    pred_oper.add_child(scan_oper);
    ProjectOperator project_oper;
    project_oper.add_child(&pred_oper);
    for (const Field &field : select_stmt->query_fields()) {  //所有要查询的字段属性都通过add_projection放到project_oper里
      project_oper.add_projection(field.table(), field.meta());
    }
    rc = project_oper.open();
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to open operator");
      return rc;
    }

    
    print_tuple_header(ss, project_oper);

    while ((rc = project_oper.next()) == RC::SUCCESS) {
      // get current record
      // write to response

      Tuple *tuple = project_oper.current_tuple();  // 

      if (nullptr == tuple) {
        rc = RC::INTERNAL;
        LOG_WARN("failed to get current record. rc=%s", strrc(rc));
        break;
      }

      tuple_to_string(ss, *tuple);
      ss << std::endl;
    }
    if (rc != RC::RECORD_EOF) {
      LOG_WARN("something wrong while iterate operator. rc=%s", strrc(rc));
      project_oper.close();
    } else {
      rc = project_oper.close();
    }
  }

  session_event->set_response(ss.str());
  return rc;
}

RC ExecuteStage::do_help(SQLStageEvent *sql_event)
{
  SessionEvent *session_event = sql_event->session_event();
  const char *response = "show tables;\n"
                         "desc `table name`;\n"
                         "create table `table name` (`column name` `column type`, ...);\n"
                         "create index `index name` on `table` (`column`);\n"
                         "insert into `table` values(`value1`,`value2`);\n"
                         "update `table` set column=value [where `column`=`value`];\n"
                         "delete from `table` [where `column`=`value`];\n"
                         "select [ * | `columns` ] from `table`;\n";
  session_event->set_response(response);
  return RC::SUCCESS;
}

RC ExecuteStage::do_create_table(SQLStageEvent *sql_event)
{
  const CreateTable &create_table = sql_event->query()->sstr.create_table;
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  RC rc = db->create_table(create_table.relation_name,
			create_table.attribute_count, create_table.attributes);
  if (rc == RC::SUCCESS) {
    session_event->set_response("SUCCESS\n");
  } else {
    session_event->set_response("FAILURE\n");
  }
  return rc;
}
RC ExecuteStage::do_drop_table(SQLStageEvent *sql_event)
{
  const DropTable &drop_table = sql_event->query()->sstr.drop_table;
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();

  RC rc = db->drop_table(drop_table.relation_name);
  // RC rc =
  //     db->create_table(create_table.relation_name, create_table.attribute_count,
  //                      create_table.attributes);

  if (rc == RC::SUCCESS) {
    session_event->set_response("SUCCESS\n");
  } else {
    session_event->set_response("FAILURE\n");
  }
  return rc;
}

RC ExecuteStage::do_create_index(SQLStageEvent *sql_event)
{
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  const CreateIndex &create_index = sql_event->query()->sstr.create_index;
  Table *table = db->find_table(create_index.relation_name);
  if (nullptr == table) {
    session_event->set_response("FAILURE\n");
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  RC rc = table->create_index(nullptr, create_index.index_name, create_index.attribute_name);
  sql_event->session_event()->set_response(rc == RC::SUCCESS ? "SUCCESS\n" : "FAILURE\n");
  return rc;
}

RC ExecuteStage::do_drop_index(SQLStageEvent *sql_event)
{
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  const DropIndex &drop_index = sql_event->query()->sstr.drop_index;
  //TODO
}

RC ExecuteStage::do_show_tables(SQLStageEvent *sql_event)
{
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  std::vector<std::string> all_tables;
  db->all_tables(all_tables);
  if (all_tables.empty()) {
    session_event->set_response("No table\n");
  } else {
    std::stringstream ss;
    for (const auto &table : all_tables) {
      ss << table << std::endl;
    }
    session_event->set_response(ss.str().c_str());
  }
  return RC::SUCCESS;
}

RC ExecuteStage::do_desc_table(SQLStageEvent *sql_event)
{
  Query *query = sql_event->query();
  Db *db = sql_event->session_event()->session()->get_current_db();
  const char *table_name = query->sstr.desc_table.relation_name;
  Table *table = db->find_table(table_name);
  std::stringstream ss;
  if (table != nullptr) {
    table->table_meta().desc(ss);
  } else {
    ss << "No such table: " << table_name << std::endl;
  }
  sql_event->session_event()->set_response(ss.str().c_str());
  return RC::SUCCESS;
}

RC ExecuteStage::do_insert(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  SessionEvent *session_event = sql_event->session_event();

  if (stmt == nullptr) {
    LOG_WARN("cannot find statement");
    return RC::GENERIC_ERROR;
  }

  InsertStmt *insert_stmt = (InsertStmt *)stmt;

  Table *table = insert_stmt->table();
  RC rc = table->insert_record(nullptr, insert_stmt->value_amount(), insert_stmt->values());
  if (rc == RC::SUCCESS) {
    session_event->set_response("SUCCESS\n");
  } else {
    session_event->set_response("FAILURE\n");
  }
  return rc;
}

RC ExecuteStage::do_delete(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  SessionEvent *session_event = sql_event->session_event();

  if (stmt == nullptr) {
    LOG_WARN("cannot find statement");
    return RC::GENERIC_ERROR;
  }

  DeleteStmt *delete_stmt = (DeleteStmt *)stmt;
  TableScanOperator scan_oper(delete_stmt->table());
  PredicateOperator pred_oper(delete_stmt->filter_stmt());
  pred_oper.add_child(&scan_oper);
  DeleteOperator delete_oper(delete_stmt);
  delete_oper.add_child(&pred_oper);

  RC rc = delete_oper.open();
  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
  } else {
    session_event->set_response("SUCCESS\n");
  }
  return rc;
}

RC ExecuteStage::do_update(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  SessionEvent *session_event = sql_event->session_event();

  if (stmt == nullptr) {
    LOG_WARN("cannot find statement");
    return RC::GENERIC_ERROR;
  }

  UpdateStmt *update_stmt = (UpdateStmt *)stmt;

  Table *table = update_stmt->table();
  // RC rc = table->update_record(nullptr,update_stmt->attribute_name(),&(update_stmt->values()),)
  TableScanOperator scan_oper(update_stmt->table());
  PredicateOperator pred_oper(update_stmt->filter_stmt());
  pred_oper.add_child(&scan_oper);
  UpdateOperator update_oper(update_stmt);
  update_oper.add_child(&pred_oper);

  RC rc = update_oper.open();

  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
  } else {
    session_event->set_response("SUCCESS\n");
  }
  return rc;

}