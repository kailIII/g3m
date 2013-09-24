//
//  GPUUniform.h
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 05/04/13.
//
//

#ifndef G3MiOSSDK_GPUUniform_h
#define G3MiOSSDK_GPUUniform_h

#include "GL.hpp"
#include "GLConstants.hpp"
#include "IGLUniformID.hpp"
#include "IStringBuilder.hpp"
#include "GPUVariable.hpp"

#include "RCObject.hpp"

#include "Matrix44DProvider.hpp"

class GPUUniform;

class GPUUniformValue: public RCObject{
  const int _type;

public:
  GPUUniformValue(int type):_type(type)
  {}

  virtual ~GPUUniformValue() {
    //    ILogger::instance()->logInfo("Deleting Uniform Value");
#ifdef JAVA_CODE
    super.dispose();
#endif
  }


  int getType() const { return _type;}
  virtual void setUniform(GL* gl, const IGLUniformID* id) const = 0;
  virtual bool isEquals(const GPUUniformValue* v) const = 0;

  virtual std::string description() const = 0;
};


class GPUUniform: public GPUVariable {
private:
  const IGLUniformID* _id;

  bool _dirty;
#ifdef C_CODE
  const GPUUniformValue* _value;
#endif
#ifdef JAVA_CODE
  private GPUUniformValue _value;
#endif
  const int _type;

  const GPUUniformKey _key;

public:

  virtual ~GPUUniform() {
    delete _id;
    if (_value != NULL) {
      _value->_release();
    }

#ifdef JAVA_CODE
    super.dispose();
#endif

  }

  GPUUniform(const std::string& name,
             IGLUniformID* id,
             int type) :
  GPUVariable(name, UNIFORM),
  _id(id),
  _dirty(false),
  _value(NULL),
  _type(type),
  _key(getUniformKey(name))
  {
  }

  const std::string getName() const { return _name; }
  const IGLUniformID* getID() const { return _id; }
  int getType() const { return _type; }
  bool wasSet() const { return _value != NULL; }
  const GPUUniformValue* getSetValue() const { return _value; }
  GPUUniformKey getKey() const { return _key;}


  int getIndex() const {
#ifdef C_CODE
    return _key;
#endif
#ifdef JAVA_CODE
    return _key.getValue();
#endif
  }

  void unset();

  void set(const GPUUniformValue* v) {
    if (_type == v->getType()) { //type checking
      if (_value == NULL || !_value->isEquals(v)) {
        _dirty = true;
        v->_retain();
        if (_value != NULL) {
          _value->_release();
        }
        _value = v;
      }
    }
    else {
      ILogger::instance()->logError("Attempting to set uniform " + _name + " with invalid value type.");
    }
  }

  void applyChanges(GL* gl);

};


class GPUUniformValueBool:public GPUUniformValue{
public:
  const bool _value;

  GPUUniformValueBool(bool b):GPUUniformValue(GLType::glBool()),_value(b) {}

  void setUniform(GL* gl, const IGLUniformID* id) const{
    if (_value) {
      gl->uniform1i(id, 1);
    } else{
      gl->uniform1i(id, 0);
    }
  }
  bool isEquals(const GPUUniformValue* v) const{
    return _value == ((GPUUniformValueBool*)v)->_value;
  }

  std::string description() const{
    IStringBuilder* isb = IStringBuilder::newStringBuilder();
    isb->addString("Uniform Value Boolean: ");
    isb->addBool(_value);
    std::string s = isb->getString();
    delete isb;
    return s;
  }
};


class GPUUniformBool: public GPUUniform{
public:
  GPUUniformBool(const std::string&name, IGLUniformID* id):GPUUniform(name,id, GLType::glBool()) {}
};


class GPUUniformValueVec2Float:public GPUUniformValue{
public:
  const float _x, _y;

  GPUUniformValueVec2Float(float x, float y):GPUUniformValue(GLType::glVec2Float()), _x(x),_y(y) {}

  void setUniform(GL* gl, const IGLUniformID* id) const{
    gl->uniform2f(id, _x, _y);
  }
  bool isEquals(const GPUUniformValue* v) const{
    GPUUniformValueVec2Float *v2 = (GPUUniformValueVec2Float *)v;
    return (_x == v2->_x) && (_y == v2->_y);
  }

  std::string description() const{
    IStringBuilder* isb = IStringBuilder::newStringBuilder();
    isb->addString("Uniform Value Vec2Float: x:");
    isb->addDouble(_x);
    isb->addString("y:");
    isb->addDouble(_y);
    std::string s = isb->getString();
    delete isb;
    return s;
  }
};


class GPUUniformVec2Float: public GPUUniform{
public:
  GPUUniformVec2Float(const std::string&name, IGLUniformID* id):GPUUniform(name,id, GLType::glVec2Float()) {}
};
////////////////////////////////////////////////////////////
class GPUUniformValueVec3Float:public GPUUniformValue{
public:
  const float _x, _y, _z;

  GPUUniformValueVec3Float(float x, float y, float z):
  GPUUniformValue(GLType::glVec3Float()),_x(x),_y(y), _z(z){}

  void setUniform(GL* gl, const IGLUniformID* id) const{
    gl->uniform3f(id, _x, _y, _z);
  }
  bool isEquals(const GPUUniformValue* v) const{
    GPUUniformValueVec3Float *v2 = (GPUUniformValueVec3Float *)v;
    return (_x == v2->_x) && (_y == v2->_y) && (_z == v2->_z);
  }

  //  GPUUniformValue* copyOrCreate(GPUUniformValue* value) const {
  //    if (value != NULL){
  //      delete value;
  //    }
  //      return new GPUUniformValueVec4Float(_x,_y,_z,_w);
  //  }

  std::string description() const{
    IStringBuilder* isb = IStringBuilder::newStringBuilder();
    isb->addString("Uniform Value Vec4Float: x:");
    isb->addDouble(_x);
    isb->addString("y:");
    isb->addDouble(_y);
    isb->addString("z:");
    isb->addDouble(_z);
    std::string s = isb->getString();
    delete isb;
    return s;
  }
};


class GPUUniformVec3Float: public GPUUniform{
public:
  GPUUniformVec3Float(const std::string&name, IGLUniformID* id):GPUUniform(name,id, GLType::glVec3Float()){}
};
////////////////////////////////////////////////////////////

class GPUUniformValueVec4Float:public GPUUniformValue{
public:
  const float _x, _y, _z, _w;

  GPUUniformValueVec4Float(const Color& color):
  GPUUniformValue(GLType::glVec4Float()),_x(color.getRed()),_y(color.getGreen()), _z(color.getBlue()), _w(color.getAlpha()){}

  GPUUniformValueVec4Float(float x, float y, float z, float w):
  GPUUniformValue(GLType::glVec4Float()),_x(x),_y(y), _z(z), _w(w) {}

  void setUniform(GL* gl, const IGLUniformID* id) const{
    gl->uniform4f(id, _x, _y, _z, _w);
  }
  bool isEquals(const GPUUniformValue* v) const{
    GPUUniformValueVec4Float *v2 = (GPUUniformValueVec4Float *)v;
    return (_x == v2->_x) && (_y == v2->_y) && (_z == v2->_z) && (_w == v2->_w);
  }

  std::string description() const{
    IStringBuilder* isb = IStringBuilder::newStringBuilder();
    isb->addString("Uniform Value Vec4Float: x:");
    isb->addDouble(_x);
    isb->addString("y:");
    isb->addDouble(_y);
    isb->addString("z:");
    isb->addDouble(_z);
    isb->addString("w:");
    isb->addDouble(_w);
    std::string s = isb->getString();
    delete isb;
    return s;
  }
};


class GPUUniformVec4Float: public GPUUniform{
public:
  GPUUniformVec4Float(const std::string&name, IGLUniformID* id):GPUUniform(name,id, GLType::glVec4Float()) {}
};

/////////////////////


class GPUUniformValueMatrix4:public GPUUniformValue{
private:
//  const bool _ownsProvider;
#ifdef C_CODE
  const Matrix44DProvider* _provider;
  mutable const Matrix44D* _lastModelSet;
#endif
#ifdef JAVA_CODE
  protected Matrix44DProvider _provider = null;
  protected  Matrix44D _lastModelSet;
#endif
public:
  
  GPUUniformValueMatrix4(const Matrix44DProvider* providers[], int nMatrix):
  GPUUniformValue(GLType::glMatrix4Float()),
  _provider(new Matrix44DMultiplicationHolder( providers, nMatrix ) ),
  _lastModelSet(NULL)
//  _ownsProvider(true)
  {
  }

  GPUUniformValueMatrix4(const Matrix44DProvider* provider):
  GPUUniformValue(GLType::glMatrix4Float()),
  _provider(provider),
  _lastModelSet(NULL)
//  _ownsProvider(ownsProvider)
  {
    _provider->_retain();
  }

  GPUUniformValueMatrix4(const Matrix44D* m):
  GPUUniformValue(GLType::glMatrix4Float()),
  _provider(new Matrix44DHolder(m)),
  _lastModelSet(NULL)
//  _ownsProvider(true)
  {
  }

  ~GPUUniformValueMatrix4(){
//    if (_ownsProvider){
      _provider->_release();
//    }
    if (_lastModelSet != NULL){
      _lastModelSet->_release();
    }
  }

  void setUniform(GL* gl, const IGLUniformID* id) const{

    if (_lastModelSet != NULL){
      _lastModelSet->_release();
    }

    _lastModelSet = _provider->getMatrix();

    _lastModelSet->_retain();

    gl->uniformMatrix4fv(id, false, _lastModelSet);
  }

  bool isEquals(const GPUUniformValue* v) const{
    if (_lastModelSet == ((GPUUniformValueMatrix4 *)v)->_provider->getMatrix()){
      return true;
    }

    return false;
  }

  std::string description() const{
    IStringBuilder* isb = IStringBuilder::newStringBuilder();
    isb->addString("Uniform Value Matrix44D.");
    std::string s = isb->getString();
    delete isb;
    return s;
  }

  //  const Matrix44D* getMatrix() const { return _m;}
};


class GPUUniformMatrix4Float: public GPUUniform{
public:
  GPUUniformMatrix4Float(const std::string&name, IGLUniformID* id):GPUUniform(name,id, GLType::glMatrix4Float()) {}
};


class GPUUniformValueFloat:public GPUUniformValue{
public:
  const float _value;

  GPUUniformValueFloat(float d):GPUUniformValue(GLType::glFloat()),_value(d) {}

  void setUniform(GL* gl, const IGLUniformID* id) const{
    gl->uniform1f(id, _value);
  }
  bool isEquals(const GPUUniformValue* v) const{
    GPUUniformValueFloat *v2 = (GPUUniformValueFloat *)v;
    return _value == v2->_value;
  }

  std::string description() const{
    IStringBuilder* isb = IStringBuilder::newStringBuilder();
    isb->addString("Uniform Value Float: ");
    isb->addDouble(_value);
    std::string s = isb->getString();
    delete isb;
    return s;
  }
};


class GPUUniformFloat: public GPUUniform{
public:
  GPUUniformFloat(const std::string&name, IGLUniformID* id):GPUUniform(name,id, GLType::glFloat()) {}
};

#endif