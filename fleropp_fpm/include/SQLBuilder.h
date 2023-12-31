#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include "FleroppDB.hpp"
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <sstream>

/**
 * \namespace SQLBuilder
 * 
*/
namespace SQLBuilder {

/**
 * \class column
 * \brief column class for use in SQLBuilder
*/
class column;

/**
 * \class Param
 * \brief A class that represents a parameter
*/
class Param {
public:
/**
 * \brief Param constructor with parameter
 * \param[in] param The string representation of the parameter
*/
    Param (const std::string &param) : _param(param) {}
    /**
     * \brief Param constructor with char array parameter
     * \param[in] param The char array representation of the parameter
    */
    Param (const char *param) : _param(param) {}

public:
    /**
     * \brief Function that returns the parameter
     * \return The parameter as a string
    */
    std::string operator()() const { return param(); }
    /**
     * \brief Returns the parameter
     * \return The parameter as a string
    */
    inline std::string param() const { return _param; }

private:
    const std::string _param;
};


/**
 * \brief Converts data of type T to a string
 * 
 * \tparam T Type of data to be converted
 * \param[in] data The data to be converted
 * \return A string representation of the data
*/
template <typename T>
inline std::string to_value(const T& data) {
    return std::to_string(data);
}

/**
 * \brief Converts a character array to a string
 * 
 * \tparam N Size of the character array
 * \param[in] data The character array to be converted
 * \return A string representation of the character array
*/
template <size_t N>
inline std::string to_value(char const(&data)[N]) {
    std::string str;
    str.append(data);
    return str;
}

/**
 * \brief Copies a string from a string
 * 
 * \param[in] data The string to be copied
 * \return A string representation of the string copy
*/
template <>
inline std::string to_value<std::string>(const std::string& data) {
    std::string str;
    str.append(data);
    return str;
}

/**
 * \brief Converts a C-style string to a normal string
 * 
 * \param[in] data The C-style string to be converted
 * \return A string representation of the original C-style string
*/
template <>
inline std::string to_value<const char*>(const char* const& data) {
    std::string str;
    str.append(data);
    return str;
}

/**
 * \brief Converts a Param object to a string
 * 
 * \param[in] data The Param object to be converted to a string
 * \return A string representation of the Param object
*/
template <>
inline std::string to_value<Param>(const Param& data) {
    return data();
}

/**
 * \brief Converts a column object to a string
 * 
 * \param[in] data The column object to be converted
 * \return A string representation of the column object
*/
template <>
inline std::string to_value<column>(const column& data);

/**
 * \brief Converts a time_t object to a string with the format "YYYY-MM-DD HH:MM:SS"
 * 
 * \param data[in] The time_t object to be converted
 * \return A string representation of the time_t object
*/
template <>
inline std::string to_value<time_t>(const time_t& data) {
    char buff[128] = {0};
    struct tm* ttime = localtime(&data);
    std::string str;
    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", ttime);
    str.append(buff);
    return str;
}

/**
 * \brief Concatenates the elements of a vector into a string, separated by chosen delimiter
 * 
 * \tparam T The type of the vector elements
 * \param[out] result The output string to store the concatenated elements
 * \param[in] vec The input vector containing the elements to be concatenated
 * \param[in] sep The delimiter to be used to separate the elements
*/
template <typename T>
void join_vector(std::string& result, const std::vector<T>& vec, const char* sep) {
    size_t size = vec.size();
    for(size_t i = 0; i < size; ++i) {
        if(i < size - 1) {
            result.append(vec[i]);
            result.append(sep);
        } else {
            result.append(vec[i]);
        }
    }
}

/**
 * \brief Function that combines two vectors into a single vector
 * 
 * \tparam T Type of vector elements
 * \param[out] baseVec The output vector in which the combined elements are stored
 * \param[in] newVec The input vector containing the elements to be added to baseVec
*/
template <typename T>
void combine_vector(std::vector<T>& baseVec, const std::vector<T>& newVec) {
    for (const auto& val : newVec) {
        baseVec.push_back(val);
    }
}

/**
 * \class column
 * \brief Represents a SQL column in a query
*/
class column
{
public:
    /**
     * \brief Constructs a column object with a given SQL column name
     * 
     * \param[in] column The name of the SQL column
    */
    column(const std::string& column) {
        _cond = column;
    }

    /**
     * \brief Constructs a column object after comparing the column's value to the given value
     * 
     * \param[in] column The name of the SQL column
     * \param[in] op The comparison operator to be used
     * \param[in] val The value to which the column value will be compared
    */
    column(const std::string& column, const std::string& op, const std::string& val) {
        _cond = column + " " + op + " ?";
        _binding.push_back(to_value(val));
    }
    virtual ~column() {}

    column& as(const std::string& s) {
        _cond.append(" as ");
        _cond.append(s);

        return *this;
    }

    column& is_null() {
        _cond.append(" is null");
        return *this;
    }

    column& is_not_null() {
        _cond.append(" is not null");
        return *this;
    }

    template <typename T>
    column& in(const std::vector<T>& args) {
        size_t size = args.size();
        if(size == 1) {
            _cond.append(" = ?");
            _binding.push_back(to_value(args[0]));
        } else {
            _cond.append(" in (");
            for(size_t i = 0; i < size; ++i) {
                _cond.append("?");
                _binding.push_back(to_value(args[i]));

                if (i < size - 1) {
                    _cond.append(", ");
                }
            }
            _cond.append(")");
        }
        return *this;
    }

    template <typename T>
    column& not_in(const std::vector<T>& args) {
        size_t size = args.size();
        if(size == 1) {
            _cond.append(" != ?");
            _binding.push_back(to_value(args[0]));
        } else {
            _cond.append(" not in (");
            for(size_t i = 0; i < size; ++i) {
                _cond.append("?");
                _binding.push_back(to_value(args[i]));

                if(i < size - 1) {
                    _cond.append(", ");
                }
            }
            _cond.append(")");
        }
        return *this;
    }

    column& operator &&(const column& condition) {
        std::string str("(");
        str.append(_cond);
        str.append(") and (");
        str.append(condition._cond);
        combine_vector(_binding, condition._binding);
        str.append(")");
        _cond = str;
        return *this;
    }

    column& operator ||(const column& condition) {
        std::string str("(");
        str.append(_cond);
        str.append(") or (");
        str.append(condition._cond);
        combine_vector(_binding, condition._binding);
        str.append(")");
        _cond = str;
        
        return *this;
    }

    // column& operator &&(const std::string& condition) {
    //     _cond.append(" and ");
    //     _cond.append(condition);
    //     return *this;
    // }

    // column& operator ||(const std::string& condition) {
    //     _cond.append(" or ");
    //     _cond.append(condition);
    //     return *this;
    // }

    // column& operator &&(const char* condition) {
    //     _cond.append(" and ");
    //     _cond.append(condition);
    //     return *this;
    // }

    // column& operator ||(const char* condition) {
    //     _cond.append(" or ");
    //     _cond.append(condition);
    //     return *this;
    // }

    template <typename T>
    column& operator ==(const T& data) {
        _cond.append(" = ?");
        _binding.push_back(to_value(data));
        return *this;
    }

    template <typename T>
    column& operator !=(const T& data) {
        _cond.append(" != ?");
        _binding.push_back(to_value(data));
        return *this;
    }

    template <typename T>
    column& operator >=(const T& data) {
        _cond.append(" >= ?");
        _binding.push_back(to_value(data));
        return *this;
    }

    template <typename T>
    column& operator <=(const T& data) {
        _cond.append(" <= ?");
        _binding.push_back(to_value(data));
        return *this;
    }

    template <typename T>
    column& operator >(const T& data) {
        _cond.append(" > ?");
        _binding.push_back(to_value(data));
        return *this;
    }

    template <typename T>
    column& operator <(const T& data) {
        _cond.append(" < ?");
        _binding.push_back(to_value(data));
        return *this;
    }

    const std::string& str() const {
        return _cond;
    }

    const std::vector<std::string>& bindings() const {
        return _binding;
    }

    // operator bool() {
    //     return true;
    // }
private:
    std::string _cond;
    std::vector<std::string> _binding;
};

template <>
inline std::string to_value<column>(const column& data) {
    return data.str();
}


class SqlModel 
{
public:
    SqlModel() {}
    virtual ~SqlModel() {}

    virtual const std::string& str() = 0;
    const std::string& last_sql() {
        return _sql;
    }
private:
    SqlModel(const SqlModel& m) = delete;
    SqlModel& operator =(const SqlModel& data) = delete;
protected:
    std::string _sql;
};

class SelectModel : public SqlModel
{
public:
    SelectModel() : _distinct(false) {}
    virtual ~SelectModel() {}

    template <typename... Args>
    SelectModel& select(const std::string& str, Args&&... columns) {
        _select_columns.push_back(str);

        select(columns...);

        return *this;
    }

    template <typename... Args>
    SelectModel& select(const std::unordered_map<std::string, std::string> map, Args&&... columns) {
        for (const auto& p : map) {
            std::string str = p.first + " as " + p.second;
            // a as b

            select(str);
        }

        select(columns...);

        return *this;
    }

    // for recursion
    SelectModel& select() {
        return *this;
    }

    SelectModel& distinct() {
        _distinct = true;
        return *this;
    }

    template <typename... Args>
    SelectModel& from(const std::string& table_name, Args&&... tables) {
        if(_table_name.empty()) {
            _table_name = table_name;
        } else {
            _table_name.append(", ");
            _table_name.append(table_name);
        }
        from(tables...);
        return *this;
    }
    
    // for recursion
    SelectModel& from() {
        return *this;
    }

    SelectModel& join(const std::string& table_name) {
        _join_type = "join";
        _join_table = table_name;
        return *this;
    }

    SelectModel& left_join(const std::string& table_name) {
        _join_type = "left join";
        _join_table = table_name;
        return *this;
    }

    SelectModel& left_outer_join(const std::string& table_name) {
        _join_type = "left outer join";
        _join_table = table_name;
        return *this;
    }

    SelectModel& right_join(const std::string& table_name) {
        _join_type = "right join";
        _join_table = table_name;
        return *this;
    }

    SelectModel& right_outer_join(const std::string& table_name) {
        _join_type = "right outer join";
        _join_table = table_name;
        return *this;
    }

    SelectModel& full_join(const std::string& table_name) {
        _join_type = "full join";
        _join_table = table_name;
        return *this;
    }

    SelectModel& full_outer_join(const std::string& table_name) {
        _join_type = "full outer join";
        _join_table = table_name;
        return *this;
    }

    SelectModel& on(const std::string& condition) {
        _join_on_condition.push_back(condition);
        return *this;
    }

    SelectModel& on(const column& condition) {
        _join_on_condition.push_back(condition.str());
        return *this;
    }

    SelectModel& where(const std::string& column, const std::string& op, const std::string& val) {
        where(SQLBuilder::column{column, op, val});

        return *this;
    }

    SelectModel& where(const column& condition) {
        _where_condition.push_back(condition.str());
        combine_vector(_where_condition_binding, condition.bindings());
        return *this;
    }

    SelectModel& where(const std::string& column, const std::string& val) {
        where({column, "=", val});

        return *this;
    }

    template <typename... Args>
    SelectModel& group_by(const std::string& str, Args&&...columns) {
        _groupby_columns.push_back(str);
        group_by(columns...);
        return *this;
    }

    // for recursion
    SelectModel& group_by() {
        return *this;
    }

    SelectModel& having(const std::string& column, const std::string& op, const std::string& val) {
        having({column, op, val});

        return *this;
    }

    SelectModel& having(const column& condition) {
        _having_condition.push_back(condition.str());
        combine_vector(_having_condition_binding, condition.bindings());

        return *this;
    }

    SelectModel& order_by(const std::string& order_by) {
        _order_by = order_by;
        return *this;
    }

    template <typename T>
    SelectModel& offset(const T& offset) {
        _offset = "?";
        _offset_binding.push_back(to_value(offset));

        return *this;
    }

    template <typename T>
    SelectModel& limit(const T& limit_val) {
        _limit = "?";
        _limit_binding.push_back(to_value(limit_val));

        return *this;
    }
    template <typename T>
    SelectModel& limit(const T& offset_val, const T& limit_val) {
        limit(limit_val);
        offset(offset_val);
        
        return *this;
    }

    virtual const std::string& str() override {
        _bindings.clear();
        _sql.clear();
        _sql.append("select ");
        if(_distinct) {
            _sql.append("distinct ");
        }
        join_vector(_sql, _select_columns, ", ");
        _sql.append(" from ");
        _sql.append(_table_name);
        if(!_join_type.empty()) {
            _sql.append(" ");
            _sql.append(_join_type);
            _sql.append(" ");
            _sql.append(_join_table);
        }
        if(!_join_on_condition.empty()) {
            _sql.append(" on ");
            join_vector(_sql, _join_on_condition, " and ");
        }
        if(!_where_condition.empty()) {
            _sql.append(" where ");
            combine_vector(_bindings, _where_condition_binding);
            join_vector(_sql, _where_condition, " and ");
        }
        if(!_groupby_columns.empty()) {
            _sql.append(" group by ");
            join_vector(_sql, _groupby_columns, ", ");
        }
        if(!_having_condition.empty()) {
            _sql.append(" having ");
            combine_vector(_bindings, _having_condition_binding);
            join_vector(_sql, _having_condition, " and ");
        }
        if(!_order_by.empty()) {
            _sql.append(" order by ");
            _sql.append(_order_by);
        }
        if(!_limit.empty()) {
            _sql.append(" limit ");
            _sql.append(_limit);
            combine_vector(_bindings, _limit_binding);
        }
        if(!_offset.empty()) {
            _sql.append(" offset ");
            _sql.append(_offset);
            combine_vector(_bindings, _offset_binding);
        }

        // std::cout << "SQL with ?: " << _sql << std::endl;

        // std::cout << "Bindings: " << std::endl;

        // for (size_t i = 0; i < _bindings.size(); i++)
        // {
        //    std::cout << "[" << i << "] = " << _bindings.at(i) << std::endl;
        // }


        return _sql;
    }

    SelectModel& reset() {
        _select_columns.clear();
        _distinct = false;
        _groupby_columns.clear();
        _table_name.clear();
        _join_type.clear();
        _join_table.clear();
        _join_on_condition.clear();
        _where_condition.clear();
        _where_condition_binding.clear();
        _having_condition.clear();
        _having_condition_binding.clear();
        _order_by.clear();
        _limit.clear();
        _limit_binding.clear();
        _offset.clear();
        _offset_binding.clear();
        _bindings.clear();
        return *this;
    }
    friend inline std::ostream& operator<< (std::ostream& out, SelectModel& mod) {
        out<<mod.str();
        return out;
    }

    std::vector<std::unordered_map<std::string, std::string>> run() {
        // We intialized the query variable instead of passing 'this->str()' directly
        // into the 'read_entry()' method, because the '_bindings' variable is initialized
        // with the 'this->str()' call. So we have to guarantee that '_bindings' is initialized
        // by the time it is passed to 'read_entry()'.
        std::string query(this->str());

        // std::cout << "Size of bindings in run: " << _bindings.size() << std::endl;

        std::vector<std::unordered_map<std::string, std::string>> result = 
        fleropp::db::db_handle->read_entry(query, _bindings);

        return result;
    }

protected:
    std::vector<std::string> _select_columns;
    bool _distinct;
    std::vector<std::string> _groupby_columns;
    std::string _table_name;
    std::string _join_type;
    std::string _join_table;
    std::vector<std::string> _join_on_condition;
    std::vector<std::string> _where_condition;
    std::vector<std::string> _where_condition_binding;
    std::vector<std::string> _bindings;
    std::vector<std::string> _having_condition;
    std::vector<std::string> _having_condition_binding;
    std::string _order_by;
    std::string _limit;
    std::vector<std::string> _limit_binding;
    std::string _offset;
    std::vector<std::string> _offset_binding;
};

class InsertModel : public SqlModel
{
public:
    InsertModel() {}
    virtual ~InsertModel() {}

    template <typename T>
    InsertModel& insert(const std::string& c, const T& data) {
        _columns.push_back(c);
        _values.push_back(to_value(data));
        return *this;
    }

    template <typename T>
    InsertModel& operator()(const std::string& c, const T& data) {
        return insert(c, data);
    }

    InsertModel& into(const std::string& table_name) {
        _table_name = table_name;
        return *this;
    }

    InsertModel& replace(bool var) {
        _replace = var;
        return *this;
    }

    virtual const std::string& str() override {
        _sql.clear();
        std::string v_ss;

        if (_replace) {
            _sql.append("insert or replace into ");
        }else {
            _sql.append("insert into ");
        }

        _sql.append(_table_name);
        _sql.append("(");
        v_ss.append(" values(");
        size_t size = _columns.size();
        for(size_t i = 0; i < size; ++i) {
            if(i < size - 1) {
                _sql.append(_columns[i]);
                _sql.append(", ");
                v_ss.append("?");
                v_ss.append(", ");
            } else {
                _sql.append(_columns[i]);
                _sql.append(")");
                v_ss.append("?");
                v_ss.append(")");
            }
        }
        _sql.append(v_ss);
        return _sql;
    }

    InsertModel& reset() {
        _table_name.clear();
        _columns.clear();
        _values.clear();
        return *this;
    }

    size_t run() {
        std::string query(this->str());

        size_t rows = fleropp::db::db_handle->create_entry(query, _values);

        return rows;
    }

    friend inline std::ostream& operator<< (std::ostream& out, InsertModel& mod) {
        out<<mod.str();
        return out;
    }

protected:
    bool _replace = false;
    std::string _table_name;
    std::vector<std::string> _columns;
    std::vector<std::string> _values;
};

template <>
inline InsertModel& InsertModel::insert(const std::string& c, const std::nullptr_t&) {
    _columns.push_back(c);
    _values.push_back("null");
    return *this;
}


class UpdateModel : public SqlModel
{
public:
    UpdateModel() {}
    virtual ~UpdateModel() {}

    UpdateModel& update(const std::string& table_name) {
        _table_name = table_name;
        return *this;
    }

    template <typename T>
    UpdateModel& set(const std::string& c, const T& data) {
        std::string str(c);
        str.append(" = ?");
        _set_value_binding.push_back(to_value(data));
        _set_columns.push_back(str);
        return *this;
    }

    template <typename T>
    UpdateModel& operator()(const std::string& c, const T& data) {
        return set(c, data);
    }

    UpdateModel& where(const std::string& column, const std::string& op, const std::string& val) {
        where(SQLBuilder::column{column, op, val});

        return *this;
    }

    UpdateModel& where(const column& condition) {
        _where_condition.push_back(condition.str());
        combine_vector(_where_condition_binding, condition.bindings());

        return *this;
    }

    UpdateModel& where(const std::string& column, const std::string& val) {
        where(SQLBuilder::column{column, "=", val});

        return *this;
    }

    virtual const std::string& str() override {
        _sql.clear();
        _bindings.clear();
        _sql.append("update ");
        _sql.append(_table_name);
        _sql.append(" set ");
        join_vector(_sql, _set_columns, ", ");
        combine_vector(_bindings, _set_value_binding);
        size_t size = _where_condition.size();
        if(size > 0) {
            _sql.append(" where ");
            join_vector(_sql, _where_condition, " and ");
            combine_vector(_bindings, _where_condition_binding);
        }

        // std::cout << "Bindings from SET: " << std::endl;

        // for (size_t i = 0; i < _bindings.size(); i++) {
        //     std::cout << "[" << i << "] = " << _bindings.at(i) << std::endl;
        // }

            return _sql;
    }

    UpdateModel& reset() {
        _table_name.clear();
        _set_columns.clear();
        _set_value_binding.clear();
        _where_condition.clear();
        _where_condition_binding.clear();
        _bindings.clear();
        return *this;
    }
    friend inline std::ostream& operator<< (std::ostream& out, UpdateModel& mod) {
        out<<mod.str();
        return out;
    }

    size_t run() {
        std::string query = this->str();

        size_t rows = fleropp::db::db_handle->update_entry(query, _bindings);

        return rows;
    }

protected:
    std::vector<std::string> _set_columns;
    std::string _table_name;
    std::vector<std::string> _where_condition;
    std::vector<std::string> _where_condition_binding;
    std::vector<std::string> _set_value_binding;
    std::vector<std::string> _bindings;
};

template <>
inline UpdateModel& UpdateModel::set(const std::string& c, const std::nullptr_t&) {
    std::string str(c);
    str.append(" = null");
    _set_columns.push_back(str);
    return *this;
}


class DeleteModel : public SqlModel
{
public:
    DeleteModel() {}
    virtual ~DeleteModel() {}

    DeleteModel& _delete() {
        return *this;
    }

    template <typename... Args>
    DeleteModel& from(const std::string& table_name, Args&&... tables) {
        if(_table_name.empty()) {
            _table_name = table_name;
        } else {
            _table_name.append(", ");
            _table_name.append(table_name);
        }
        from(tables...);
        return *this;
    }
    
    // for recursion
    DeleteModel& from() {
        return *this;
    }

    DeleteModel& where(const std::string& column, const std::string& op, const std::string& val) {
        where(SQLBuilder::column{column, op, val});
        return *this;
    }

    DeleteModel& where(const column& condition) {
        _where_condition.push_back(condition.str());
        combine_vector(_where_condition_binding, condition.bindings());

        return *this;
    }

    DeleteModel& where(const std::string& column, const std::string& val) {
        where(SQLBuilder::column{column, "=", val});
        return *this;
    }

    virtual const std::string& str() override {
        _sql.clear();
        _bindings.clear();
        _sql.append("delete from ");
        _sql.append(_table_name);
        size_t size = _where_condition.size();
        if(size > 0) {
            _sql.append(" where ");
            join_vector(_sql, _where_condition, " and ");
            combine_vector(_bindings, _where_condition_binding);
        }

        // std::cout << "Bindings from DELETE: " << std::endl;

        // for (size_t i = 0; i < _bindings.size(); i++) {
        //     std::cout << "[" << i << "] = " << _bindings.at(i) << std::endl;
        // }

        return _sql;
    }

    size_t run() {
        std::string query = this->str();

        size_t rows = fleropp::db::db_handle->delete_entry(query, _bindings);

        return rows;
    }

    DeleteModel& reset() {
        _table_name.clear();
        _where_condition.clear();
        _where_condition_binding.clear();
        _bindings.clear();
        return *this;
    }
    friend inline std::ostream& operator<< (std::ostream& out, DeleteModel& mod) {
        out<<mod.str();
        return out;
    }

protected:
    std::string _table_name;
    std::vector<std::string> _where_condition;
    std::vector<std::string> _where_condition_binding;
    std::vector<std::string> _bindings;
};
}
