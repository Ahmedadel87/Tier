#pragma once

#include "AST.hpp"
#include "SemanticAnalysis.hpp"
#include "MemoryManager.hpp"

namespace Sema
{
    class Scope;
    class LookUpTable;

    struct Decl
    {
        const Scope* scope;
        const AST::Type* type;
    };

    enum LookupResultType
    {
        NotFound,
        Found
    };

    struct LookUpResult
    {
        LookupResultType result;
        std::vector<Decl*> decl;
    };

    enum ScopeType
    {
        Fn, // func scope
        Blck, // regular {} scopes
        Program,
    };

    // assumes top-level scopes paren_scope to be nullptr
    class Scope
    {
        private:
            ScopeType type;

            Scope* parent_scope;

            AST::Block *block;

            std::unordered_map<std::string, std::vector<Decl*>> lookup_table;

        public:
            Scope(
                ScopeType type,
                AST::Block* block,
                Scope* parent_scope
            );

            const std::unordered_map<std::string, std::vector<Decl*>>& get_lookup_table() const
            {
                return lookup_table;
            }

            const Scope* get_parent_scope() const
            {
                return parent_scope;
            }

            void add_symbol(std::string& symbol, Decl* decl)
            {
                lookup_table[symbol].emplace_back(decl);
            }

            ScopeType get_type() const 
            {
                return type;
            }

            const AST::Block* get_block() 
            {
                return block;
            }
    };

    class LookUpTable
    {
        private:
            Mem::Arena<1024> arena;

        public:
            void fill_lookup_table(Scope* scope) 
            {
                for(const auto& stmt : scope->get_block()->nodes) 
                {
                    if(auto let_dec = std::get_if<AST::LetDec*>(&stmt))
                    {
                        scope->add_symbol((*let_dec)->identifier, arena.make<Decl>(scope, (*let_dec)->type));
                    }
                }
            }

            LookUpResult lookup_symbol(std::string symbol, const Scope* scope)
            {
                if(scope->get_lookup_table().contains(symbol))
                    return LookUpResult{LookupResultType::Found, scope->get_lookup_table().at(symbol)};
                
                if(scope->get_parent_scope() != nullptr)
                    return lookup_symbol(symbol, scope->get_parent_scope());
                
                return {LookupResultType::NotFound, {nullptr}};
            }
    };

    inline Scope::Scope(ScopeType type, AST::Block* block, Scope* parent_scope) 
        : type(type), block(block), parent_scope(parent_scope)
    {
        LookUpTable().fill_lookup_table(this);
    }
}