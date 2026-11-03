#pragma once

#include "SymbolTable.hpp"
#include "MemoryManager.hpp"

namespace Sema
{
    struct Block 
    {   AST::Block* block;
        std::vector<Block*> inner_blocks;
        Scope* scope;
    };

    class ScopeBuilder
    {
        private:
            Mem::Arena<1024> arena;

            LookUpTable lookup_table;

            AST::Program* program;
            
            Block* populated_program;

            Scope* program_scope;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

            Block* populate_block(AST::Block* block, Scope* previous_scope)
            {
                Block* populated_block = arena.make<Block>
                (
                    block, 
                    std::vector<Block*>{},
                    arena.make<Scope>(ScopeType::Blck, block, previous_scope)
                );

                for(const auto& stmt : block->nodes)
                {
                    if(auto inner_block = std::get_if<AST::Block*>(&stmt))
                    {
                        populated_block->inner_blocks.push_back(populate_block(*inner_block, populated_block->scope));
                    }
                }

                return populated_block;
            }

        public:
            ScopeBuilder(AST::Program* program) : program(program)
            {
                program_scope = arena.make<Scope>(ScopeType::Program, program, nullptr);

                populated_program = arena.make<Block>
                (    
                    program,
                    std::vector<Block*>{},
                    program_scope
                );
            }
            
            Block* build_scoped_program()
            {
                for(const auto& stmt : program->nodes)
                {
                    if(auto* block = std::get_if<AST::Block*>(&stmt))
                    {
                        populated_program->inner_blocks.push_back(populate_block((*block), program_scope));
                    }
                }

                return populated_program;
            }

    };
};