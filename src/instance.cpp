//
// Created by fortwoone on 09/04/2025.
//

#include "instance.hpp"

namespace lox::inst{
    LoxInstance::LoxInstance(const ClassPtr& klass): cls(klass){}

    VarValue LoxInstance::get_attr(const string& name){
        if (fields.contains(name)){
            return fields.at(name);
        }

        auto meth = cls->find_meth(name);
        if (meth != nullptr){
            return meth->bind(shared_from_this());
        }

        throw runtime_error("Undefined property '" + name + "' for " + to_string() + ".");
    }

    void LoxInstance::set_attr(const string& name, VarValue val){
        fields.insert_or_assign(name, val);
    }

    namespace for_callable{
        shared_ptr<LoxInstance> create_inst(const ClassPtr& cls){
            return make_shared<LoxInstance>(cls);
        }
    }
}
