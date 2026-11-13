#pragma once

#include "SymbolTable.hpp"
#include "MemoryManager.hpp"
#include "Diagnostics.hpp"

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

            SourceManager::SourceManager& source_manager;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

            Block* populate_block(AST::Block* block, Scope* previous_scope)
            {
                Block* populated_block = arena.make<Block>
                (
                    block, 
                    std::vector<Block*>{},
                    arena.make<Scope>(ScopeType::Blck, block, previous_scope, source_manager)
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
            ScopeBuilder(AST::Program* program, SourceManager::SourceManager& source_manager) 
                : program(program), lookup_table(source_manager), source_manager(source_manager)
            {
                program_scope = arena.make<Scope>(ScopeType::Program, program, nullptr, source_manager);

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

    class SemanticAnalysis
    {
        private:
            AST::Program* program;
            Block* scoped_program;

            SourceManager::SourceManager& source_manager;

            ScopeBuilder scope_builder; //! MUST BE HERE, DUE TO IT OWNING THE SCOPED PROGRAM
            LookUpTable lookup_table;

            Diag::DiagnosticEngine& diag_engine;

            void diag(Diag::DiagnosticBuilder diagnostic)
            {
                diag_engine.report(diagnostic.build());
            }

            void name_resolution(AST::IdentifierExpr* identifier_expr, Scope* scope, SourceManager::SourceLocation begin_loc){
                const auto& identifier_tok = identifier_expr->identifier;

                const auto& identifier = source_manager.get_string(identifier_tok.location);

                LookUpResult result = lookup_table.lookup_symbol
                (
                    std::string(identifier),
                    scope, 
                    begin_loc.offset
                );

                if(result.result == LookupResultType::NotFound || result.result == LookupResultType::ForwardDec){
                    diag
                    (
                        Diag::DiagnosticBuilder{}
                            .begin_location(begin_loc)
                            .primary_location(identifier_tok.location)
                            .id(Diag::DiagnosticID::UnkownVariable)
                            .severity(Diag::Severity::Error)
                            .message(Diag::Message{}
                                .template_id(Diag::DiagnosticID::UnkownVariable)
                                .add_argument(std::string(identifier))
                            )
                            .add_higlight(Diag::Highlight{}
                                .primary()
                                .location(identifier_tok.location)
                                .message
                                (
                                    Diag::Message{}
                                        .template_id(Diag::DiagnosticID::UnkownVariable)
                                        .add_argument(std::string(identifier))
                                )
                            )
                    );

                    if(result.result == LookupResultType::ForwardDec){
                        diag
                        (
                            Diag::DiagnosticBuilder{}
                                .begin_location(result.decl->source_loc)
                                .primary_location(result.decl->source_loc)
                                .id(Diag::DiagnosticID::FutureDecOnLine)
                                .severity(Diag::Severity::Note)
                                .message(Diag::Message{}
                                    .template_id(Diag::DiagnosticID::FutureDecOnLine)
                                    .add_argument(std::string(identifier))
                                    .add_argument(source_manager.get_line_column(result.decl->source_loc).first + 1) //? We add +1 because internally it is 0-based ;)
                                )
                                .add_higlight(Diag::Highlight{}
                                    .secondary()
                                    .location(result.decl->org_dec->identifier->location)
                                    //! Check later if a no identifier let dec can reach here
                                )
                        );
                    }
                }
            }

            void expr_analysis(AST::Expr* expr, Scope* scope, SourceManager::SourceLocation begin_loc){
                if(expr->is(AST::Expr::Kind::BinaryExpr)){
                    auto* binary_expr = static_cast<AST::BinaryExpr*>(expr);

                    expr_analysis(binary_expr->lhs, scope, begin_loc);
                    expr_analysis(binary_expr->rhs, scope, begin_loc);

                    return;
                }

                if(expr->is(AST::Expr::Kind::UnaryExpr)){
                    auto* unary_expr = static_cast<AST::UnaryExpr*>(expr);

                    expr_analysis(unary_expr->expr, scope, begin_loc);

                    return;
                }
                
                if(expr->is(AST::Expr::Kind::IdentifierExpr)){
                    name_resolution(static_cast<AST::IdentifierExpr*>(expr), scope, begin_loc);
                }
            }

            void let_dec_analysis(AST::LetDec* let_dec, Scope* scope){
                expr_analysis(let_dec->expr, scope, let_dec->source_loc);
            }

            void block_analysis(Block* scoped_block){
                uint16_t current_inner_block_index = 0;

                for(const auto& stmt : scoped_block->block->nodes) 
                {
                    if(auto let_dec = std::get_if<AST::LetDec*>(&stmt))
                    {   
                        let_dec_analysis(*let_dec, scoped_block->scope);
                    }

                    if(auto block = std::get_if<AST::Block*>(&stmt)){
                        block_analysis(scoped_block->inner_blocks.at(current_inner_block_index++));
                    }
                }
            }
            
        public:
            SemanticAnalysis(SourceManager::SourceManager& source_manager, AST::Program* program, Diag::DiagnosticEngine& diag_engine)
                : source_manager(source_manager), 
                program(program), 
                scope_builder(program, source_manager), 
                diag_engine(diag_engine), 
                lookup_table(source_manager)
            {}

            void run(){
                scoped_program = scope_builder.build_scoped_program();

                block_analysis(scoped_program);
            }
    };
};