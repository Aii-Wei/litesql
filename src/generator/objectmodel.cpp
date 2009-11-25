// include LiteSQL's header file and generated header file
#include <iostream>
#include "xmlparser.hpp"
#include "objectmodel.hpp"
#include <string.h>
using namespace std;
using namespace xml;

#define xmlStrcasecmp(s1,s2)  ((s1==NULL) ? (s2!=NULL) : strcmp(s1,s2))
#define xmlStrEqual(s1,s2)   (!strcmp(s1,s2))

AT_field_type field_type(const XML_Char* value)
{
  AT_field_type t;

  if (!xmlStrcasecmp(value,(XML_Char*)"boolean"))
  {
    t = A_field_type_boolean;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"integer"))
  {
    t = A_field_type_integer;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"string"))
  {
    t = A_field_type_string;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"float"))
  {
    t = A_field_type_float;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"time"))
  {
    t = A_field_type_time;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"date"))
  {
    t = A_field_type_date;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"datetime"))
  {
    t = A_field_type_datetime;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"blob"))
  {
    t = A_field_type_blob;
  }
  else
  {
    t = AU_field_type;   
  }
  return t;
}

AT_field_unique field_unique(const XML_Char* value)
{
  AT_field_unique t;
  if (!xmlStrcasecmp(value,(XML_Char*)"true"))
  {
    t = A_field_unique_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"false"))
  {
    t = A_field_unique_false;
  }
  else
  {
    t = AU_field_unique;
  }
  return t;
}

AT_field_indexed field_indexed(const XML_Char* value)
{
  AT_field_indexed t;
  if (!xmlStrcasecmp(value,(XML_Char*)"true"))
  {
    t = A_field_indexed_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"false"))
  {
    t = A_field_indexed_false;
  }
  else
  {
    t = AU_field_indexed;
  }
  return t;
}

AT_relation_unidir relation_unidir(const XML_Char* value)
{
  AT_relation_unidir t;
  if (!xmlStrcasecmp(value,(XML_Char*)"true"))
  {
    t = A_relation_unidir_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"false"))
  {
    t = A_relation_unidir_false;
  }
  else
  {
    t = AU_relation_unidir;
  }
  return t;
}

AT_relate_unique relate_unique(const XML_Char* value)
{
  AT_relate_unique t;
  if (!xmlStrcasecmp(value,(XML_Char*)"true"))
  {
    t = A_relate_unique_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"false"))
  {
    t = A_relate_unique_false;
  }
  else
  {
    t = AU_relate_unique;
  }
  return t;
}

AT_relate_limit relate_limit(const XML_Char* value)
{
  AT_relate_limit t;
  if (!xmlStrcasecmp(value,(XML_Char*)"one"))
  {
    t = A_relate_limit_one;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*)"many"))
  {
    t = A_relate_limit_many;
  }
  else
  {
    t = A_relate_limit_many;
  }
  return t;

}

class LitesqlParser : public XmlParser {
public:
  LitesqlParser(ObjectModel* model)
    : m_pObjectModel(model),
      m_parseState(ROOT) {};

protected:
  void onStartElement(const XML_Char *fullname,
    const XML_Char **atts);

  void onEndElement(const XML_Char *fullname);
  /** ROOT->DATABASE; 
  *
  *    DATABASE->OBJECT;
  *      OBJECT->FIELD;
  *      OBJECT->METHOD;
  *      FIELD->OBJECT;
  *      METHOD->OBJECT;
  *
  *    DATABASE->RELATION;
  *    RELATION->DATABASE;
  *
  *  DATABASE->ROOT;
  * ERROR;
  */
  enum ParseState { ROOT,
    DATABASE,
    OBJECT,
    FIELD,
    METHOD,
    RELATION,
    INCLUDE,
    UNKNOWN,
    ERROR
  };

private:
  ObjectModel* m_pObjectModel;

  Object * obj;
  Relation * rel;
  Field * fld;
  Method * mtd;
  Index * idx;

  ParseState m_parseState;
  vector<ParseState> history;
};

void LitesqlParser::onStartElement(const XML_Char *fullname,
                              const XML_Char **atts)
{
  //   cout << "starting " <<fullname << endl;
  history.push_back(m_parseState);

  if (xmlStrEqual(fullname,(XML_Char*)"database"))
  {
    if (m_parseState!=ROOT)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_parseState = DATABASE;
      m_pObjectModel->db.name = safe((char*)xmlGetAttrValue(atts,"name"));
      m_pObjectModel->db.include = safe((char*)xmlGetAttrValue(atts,"include"));
      m_pObjectModel->db.nspace = safe((char*)xmlGetAttrValue(atts,"namespace"));
      cout << "database = " << m_pObjectModel->db.name << endl;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"object"))
  {
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_pObjectModel->objects.push_back(obj = new Object(    (char*)xmlGetAttrValue(atts,"name"), 
        safe((char*)xmlGetAttrValue(atts,"inherits"))));
      cout << "object = " << obj->name << endl;
      m_parseState = OBJECT;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"field"))
  {
    switch(m_parseState)
    {
    case OBJECT:
      cout << "field = " << endl;
      if (obj) {
        obj->fields.push_back(fld =new Field( (char*)xmlGetAttrValue(atts,"name"), 
          field_type(xmlGetAttrValue(atts,"type")),
          safe(  (char*)xmlGetAttrValue(atts,"default")),
          field_indexed(xmlGetAttrValue(atts,"indexed")),
          field_unique(xmlGetAttrValue(atts,"unique"))
          )
          );
      } 
      m_parseState = FIELD;
      break;

    case RELATION:
      if (rel) {
        rel->fields.push_back(fld =new Field(                 (char*)xmlGetAttrValue(atts,"name"), 
          field_type( xmlGetAttrValue(atts,"type") ),
          safe( (char*)xmlGetAttrValue(atts,"default") ),
          field_indexed( xmlGetAttrValue(atts,"indexed") ),
          field_unique( xmlGetAttrValue(atts,"unique") )
          )
          );
      }
      m_parseState = RELATION;
      break;

    default:
      m_parseState = ERROR;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"value"))
  {
    if (m_parseState!=FIELD)
    {
      m_parseState = ERROR;
    }
    else
    {
      if (fld) 
      {
        fld->value(Value((char*)xmlGetAttrValue(atts,"name"), (char*)xmlGetAttrValue(atts,"value")));
      }
      cout << "value = " << endl;
    }

  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"method"))
  {
    if (m_parseState!=OBJECT)
    {
      m_parseState = ERROR;
    }
    else
    {
      obj->methods.push_back(
        mtd = new Method( safe((char*)xmlGetAttrValue(atts,"name")), 
        safe((char*)xmlGetAttrValue(atts,"returntype")) 
        )
        );
      m_parseState= METHOD;
      cout << "method = " << endl;
    }

  }
  else if (xmlStrEqual(fullname,(XML_Char*)"param"))
  {
    if (m_parseState!=METHOD)
    {
      m_parseState = ERROR;
    }
    else
    {
      mtd->param(Param((char*)xmlGetAttrValue(atts,"name"),(char*)xmlGetAttrValue(atts,"type")));
    }
  }
  else if (xmlStrEqual(fullname,(XML_Char*)"relation"))
  {
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_pObjectModel->relations.push_back(rel = new Relation(  safe((char*)xmlGetAttrValue(atts,"id")), 
        safe((char*)xmlGetAttrValue(atts,"name")),
        relation_unidir(xmlGetAttrValue(atts,"unidir"))));
      cout << "relation = " << rel->getName() << endl;
      m_parseState = RELATION;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"relate"))
  {
    if (m_parseState!=RELATION)
    {
      m_parseState = ERROR;
    }
    else
    {
      rel->related.push_back(
        new Relate( safe((char*)xmlGetAttrValue(atts,"object")), 
        relate_limit(xmlGetAttrValue(atts,"limit")), 
        relate_unique(xmlGetAttrValue(atts,"unique")), 
        safe((char*)xmlGetAttrValue(atts,"handle"))));
      cout << "relate = " << endl;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"include"))
  {
    string filename((char*)xmlGetAttrValue(atts,"name"));
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "include " << '"' << filename << '"' << endl;
      ObjectModel includedModel;
      LitesqlParser parser(&includedModel);
      if (!parser.parseFile(filename)) 
      {
        cout << "error on parsing included file " << '"' << filename << '"' << endl;
      }
      m_pObjectModel->objects.insert(m_pObjectModel->objects.end(),includedModel.objects.begin(),includedModel.objects.end());
      m_pObjectModel->relations.insert(m_pObjectModel->relations.end(),includedModel.relations.begin(),includedModel.relations.end());
      m_parseState = INCLUDE;
    }
  } 
  else
  {
    m_parseState = UNKNOWN;
    cerr << "unknown = " << endl;
  } 
}

void LitesqlParser::onEndElement(const XML_Char *fullname)
{
  cout << "ending " <<fullname << endl; 
  if (xmlStrEqual(fullname,(XML_Char*)"database"))
  {
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "end database " << endl;
      m_parseState = ROOT;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"object"))
  {
    if (m_parseState!=OBJECT)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "end object " << endl;
      obj = NULL;
      m_parseState = DATABASE;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"field"))
  {
    if (m_parseState!=FIELD)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "end field" << endl;
      fld = NULL;
      m_parseState = history.back();
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"value"))
  {
    if (m_parseState!=FIELD)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "end value" << endl;
    }

  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"method"))
  {
    if (m_parseState!=METHOD)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_parseState = OBJECT;
      cout << "end method" << endl;
    }

  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"param"))
  {
    if (m_parseState!=METHOD)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "end param" << endl;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"relation"))
  {
    if (m_parseState!=RELATION)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "end relation " << endl;
      rel = NULL;
      m_parseState = DATABASE;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"relate"))
  {
    if (m_parseState!=RELATION)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "relate = " << endl;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)"include"))
  {
    if (m_parseState!=INCLUDE)
    {
      m_parseState = ERROR;
    }
    else
    {
      cout << "end include " << endl;
      m_parseState = DATABASE;
    }
  } 
  else 
  {
    m_parseState = history.back();
    cerr << "end unknown " << endl;
  }

  history.pop_back();
}

bool ObjectModel::loadFromFile(const std::string& filename)
{
  LitesqlParser parser(this);
  bool successfulParsed = parser.parseFile(filename);
  return successfulParsed;

/*
if (!successfulParsed)
  {
    return false;
  }
  else
  {
//    xml::init(db,objects,relations);
    return true;
  }
*/
}