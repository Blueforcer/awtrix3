#define VAR_NAME(name) #name

#include <ArduinoJson.h>
#include <FS.h>

#ifndef linked_list_H
#define linked_list_H


#if DEBUG_MODE
#define DebugPrint(x) DBG_OUTPUT_PORT.print(x)
#define DebugPrintln(x) DBG_OUTPUT_PORT.println(x)
#define DebugPrintf(fmt, ...) DBG_OUTPUT_PORT.printf(fmt, ##__VA_ARGS__)
#define DebugPrintf_P(fmt, ...) DBG_OUTPUT_PORT.printf_P(fmt, ##__VA_ARGS__)
#else
#define DebugPrint(x)
#define DebugPrintln(x)
#define DebugPrintf(x, ...)
#define DebugPrintf_P(x, ...)
#endif


enum { CHAR, UCHAR, CHAR_PT, BOOL, INT, LONG, UINT, ULONG, FLOAT, DOUBLE, STRING};

////////////////////////////////////////////////////////////////
struct VarNode
{
  uint8_t type;
  void* var;
  const char* key;
  VarNode* next = nullptr;
};
////////////////////////////////////////////////////////////////
class VariableList
{
  private:
    VarNode* first = nullptr;
    VarNode* last = nullptr;

  public:
    VariableList(){;}

    template <class T>
    void addItem(T& var, const char* key)
    {
      VarNode* node = new VarNode();
      node->var = &var;
      // node->key = VAR_NAME(var);
      node->key = key;
      node->type = TypeOf(var);

      if (first == nullptr) {
        first = node;
        last = node;
        return;
      }
      last->next = node;
      last = node;
    }

    VarNode* getNode(const char* key)
    {
      VarNode* current = first;
      while ( current != nullptr )
      {
        if (strcmp(current->key, key) == 0) {
          return current;
        }
        current = current->next;
      }
      return nullptr;
    }


    bool saveValues(fs::FS *fs, const char* filename) {
      File file = fs->open("/config.json", "w");
      int sz = file.size() * 1.33;
      int docSize = max(sz, 2048);
      DynamicJsonDocument root((size_t)docSize);
      if (!file)
      {
        DebugPrintln(F("File not found, will be created new configuration file"));
        return false;
      }

      VarNode* current = first;
      while ( current != nullptr )
      {
        switch (current->type) {
          case BOOL:  root[current->key] = *(bool*)current->var; break;
          case CHAR:  root[current->key] = *(signed char*)current->var; break;
          case INT:   root[current->key] = *(int*)current->var; break;
          case LONG:  root[current->key] = *(long*)current->var; break;
          case UCHAR: root[current->key] = *(unsigned char*)current->var; break;
          case UINT:  root[current->key] = *(unsigned int*)current->var; break;
          case ULONG: root[current->key] = *(unsigned long*)current->var; break;
          case CHAR_PT: root[current->key] = (unsigned char*)current->var; break;
          case FLOAT:   root[current->key] = *(float*)current->var; break;
          case DOUBLE:  root[current->key] = *(double*)current->var; break;
          case STRING:  root[current->key] = *(String*)current->var; break;
        }
        current = current->next;
      }

      serializeJson(root, file);
      file.close();
      return true;
    }

    bool loadValues(fs::FS *fs, const char* filename) {
      File file = fs->open("/config.json", "r");
      DynamicJsonDocument doc(file.size() * 1.33);
      if (file)
      {
          DeserializationError error = deserializeJson(doc, file);
          if (error)
          {
              DebugPrintln(F("Failed to deserialize file, may be corrupted"));
              DebugPrintln(error.c_str());
              file.close();
              return false;
          }
          file.close();
      }
      else
          return false;

      VarNode* current = first;
      while ( current != nullptr )
      {
        switch (current->type) {
          case BOOL:    *(bool*)current->var = doc[current->key]; break;
          case CHAR:    *(signed char*)current->var = doc[current->key]; break;
          case INT:     *(int*)current->var = doc[current->key]; break;
          case LONG:    *(long*)current->var = doc[current->key]; break;
          case UCHAR:   *(unsigned char*)current->var = doc[current->key]; break;
          case UINT:    *(unsigned int*)current->var = doc[current->key]; break;
          case ULONG:   *(unsigned long*)current->var = doc[current->key]; break;
          case CHAR_PT: strcpy((char*)current->var, doc[current->key]); break;
          case FLOAT:   *(float*)current->var = doc[current->key]; break;
          case DOUBLE:  *(double*)current->var = doc[current->key]; break;
          case STRING:  *(String*)current->var = doc[current->key].as<String>(); break;
        }
        current = current->next;
      }

      return true;
    }


    virtual inline uint8_t TypeOf(const bool&)   { return BOOL; }
    virtual inline uint8_t TypeOf(const char&)   { return CHAR; }
    virtual inline uint8_t TypeOf(const char*)   { return CHAR_PT; }
    virtual inline uint8_t TypeOf(const double&) { return DOUBLE; }
    virtual inline uint8_t TypeOf(const float&)  { return FLOAT; }
    virtual inline uint8_t TypeOf(const int&)  { return INT; }
    virtual inline uint8_t TypeOf(const unsigned int&)  { return UINT; }
    virtual inline uint8_t TypeOf(const unsigned long&)  { return ULONG; }
    virtual inline uint8_t TypeOf(const unsigned char&)  { return UCHAR; }
    virtual inline uint8_t TypeOf(const String&)  { return STRING; }
};

#endif