/* LiteSQL 
 * 
 * By Tero Laitinen 
 * 
 * See LICENSE for copyright information. */

#ifndef _litesql_persistent_hpp
#define _litesql_persistent_hpp
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include "litesql/utils.hpp"
#include "litesql/database.hpp"
#include "litesql/cursor.hpp"
#include "litesql/expr.hpp"
#include "litesql/selectquery.hpp"
#include "litesql/relation.hpp"
#include "litesql/operations.hpp"
/** \file persistent.hpp 
 * Persistent
*/

namespace litesql {

/** 
 * An abstract base class of persistents objects
 *
 * */
class Persistent {
protected:
    typedef map<string, vector<pair<FieldType, string> > > Updates;
    /** executed when object is created into database */    
    string Persistent::insert(Record& tables, 
                              Records& fieldRecs,
                              Records& values,
                              const string& sequence);
    void update(Updates& updates); 
    template <class T>
    void updateField(Updates& updates, string table, Field<T> fld) {
        if (fld.modified()) {
            updates[table].push_back(make_pair(fld.fieldType(), fld));
            fld.setModified(false);
        }
    }
    void prepareUpdate(Updates& updates, string table);
    void deleteFromTable(string table, string id);
    /** pointer to current Database Persistent is assigned to. 
        It is not a reference because Persistents must be assignable using
        operator= */
    const Database *db;
    /** flag that indicates that object is stored in some database. 
        It might be more accurate to use pointer in stead of boolean. */
    bool inDatabase;
    /** used to keep track of ID changes. When user changes Persistent 
        object's identifier (ID, field 0), record has to be upcasted 
        to change row id of all tables of object */
    int oldKey;
public:
    /** returns reference to Database the object is in */
    const Database & getDatabase() const {
        return *db;
    }
    /** returns true if object is in database */
    bool isInDatabase() const {
        return inDatabase;
    }
    /** copies values and keeps fields unchanged */
    const Persistent & operator=(const Persistent & p);
    /** copies values and registers Field-objects to fields */
    Persistent(const Persistent & p) 
        : db(p.db), inDatabase(p.inDatabase), oldKey(p.oldKey) {}
    Persistent(const Database & _db, const Record & row) 
        : db(&_db), inDatabase(true), oldKey(0) {
        if (row.size() > 0)
            oldKey = atoi(row[0]);
    }
    /** creates empty Persistent */
    Persistent(const Database & _db) 
        : db(&_db), inDatabase(false), oldKey(0) {}
    virtual void onDelete() {}
    /** empty */
    virtual ~Persistent() {}
    /** selectObjectQuery uses this to list tables used by Persistent 
        \param fdatas field information */
    static Split getTablesFromFieldTypes(const std::vector<FieldType> & fdatas);
    /** class adds own tables to tables 
        \param tables initially empty Split. Tables are inserted there */
    virtual void getTables(Split & tables) const {  }
};

/** for internal use only: used to define upcasters in Database.
  \param db database where to select upcasted Persistent
  \param id idenfitier of upcasted Persistent
*/
/*
template <class T>
Persistent * upcastHelper(const Database & db, Field & id) {
    return new T(select<T>(db, id.fieldType() == id.value()).one());
}
template <class T>
Persistent * upcastCopyHelper(Persistent & p) { 
    Persistent * np = new T(*(Database*)0); 
    *np = p; 
    return np; 
}
*/
}
#endif