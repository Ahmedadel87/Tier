#pragma once

#include "AST.hpp"
#include "SemanticAnalysis.hpp"
#include "MemoryManager.hpp"

#include <variant>

namespace Sema
{
    class Scope;
    class LookUpTable;

    struct Decl
    {
        const Scope* scope;
        const AST::LetDec* org_dec;
        SourceManager::SourceLocation source_loc;
    };

    enum LookupResultType
    {
        NotFound,
        ForwardDec,
        Found,
    };

    struct LookUpResult
    {
        LookupResultType result;
        Decl* decl;
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
            Mem::Arena<1024> arena;

            ScopeType type;

            Scope* parent_scope;

            AST::Block *block;

            std::unordered_map<std::string, Decl*> lookup_table;

        public:
            Scope(
                ScopeType type,
                AST::Block* block,
                Scope* parent_scope,
                SourceManager::SourceManager& source_manager
            );

            const std::unordered_map<std::string, Decl*>& get_lookup_table() const
            {
                return lookup_table;
            }

            const Scope* get_parent_scope() const
            {
                return parent_scope;
            }

            void add_symbol(const std::string& symbol, Decl* decl)
            {
                lookup_table[symbol] = decl;
            }

            ScopeType get_type() const 
            {
                return type;
            }

            Mem::Arena<1024>& get_arena(){
                return arena;
            }

            const AST::Block* get_block() 
            {
                return block;
            }
    };

    class LookUpTable
    {
        private:
            SourceManager::SourceManager& source_manager;

        public:
            LookUpTable(SourceManager::SourceManager& source_manager)
                : source_manager(source_manager)
            {}

            void fill_lookup_table(Scope* scope) 
            {
                for(const auto& stmt : scope->get_block()->nodes) 
                {
                    if(auto let_dec = std::get_if<AST::LetDec*>(&stmt))
                    {
                        if((*let_dec)->identifier) 
                            scope->add_symbol
                            (
                                std::string(source_manager.get_string((*let_dec)->identifier->location)), 
                                scope->get_arena().make<Decl>
                                (
                                    scope, 
                                    (*let_dec),  
                                    (*let_dec)->source_loc)
                                );
                    }
                }
            }

            LookUpResult lookup_symbol
            (
                std::string symbol, 
                const Scope* scope, 
                uint32_t stmt_offset, 
                LookUpResult closest_find = {LookupResultType::NotFound, nullptr}
            )
            {
                const auto& lookup_table = scope->get_lookup_table();

                if(lookup_table.contains(symbol))
                {
                    if(stmt_offset > lookup_table.at(symbol)->source_loc.offset) 
                        return LookUpResult{LookupResultType::Found, scope->get_lookup_table().at(symbol)};

                    closest_find = {LookupResultType::ForwardDec, lookup_table.at(symbol)};
                }

                if(scope->get_parent_scope() != nullptr)
                    return lookup_symbol(symbol, scope->get_parent_scope(), stmt_offset, closest_find);
                
                return {closest_find};
            }
    };

    inline Scope::Scope
        (ScopeType type, AST::Block* block, Scope* parent_scope, SourceManager::SourceManager& source_manager) 
        : type(type), block(block), parent_scope(parent_scope)
    {
        LookUpTable(source_manager).fill_lookup_table(this);
    }
}