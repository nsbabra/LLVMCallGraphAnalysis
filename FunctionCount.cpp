#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Value.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"

using namespace llvm;

namespace {

class  fncount : public ModulePass{
        
    public:
        static char ID;
        bool initflag;
        fncount() : ModulePass(ID){}
        
//        virtual void getAnalysisUsage(AnalysisUsage &au) const{
//            au.setPreservesCFG();
//            au.addRequired<LoopInfo>();
//            au.addRequired<ScalarEvolution>();
//        }
    
        virtual bool runOnModule(Module &M){
            bool flag=false;
            errs() << "List of function calls : \n";
            for (Module::iterator func_it = M.begin(), func_it_end = M.end();  func_it != func_it_end; ++func_it) {
                
//                fd.add_fun(func_it->getName(), 0 );
            
                Function *f = func_it;
                if(f->getName()== "printf" || f->getName() == "main" || f->getName() == "llvm.dbg.declare"){
                    continue;
                }
                
                //Print the fxn Name with starting bracket
                errs() << func_it->getName() << "(";
                FunctionType *fType = f->getFunctionType();
                
                std::string params = "";
                
                for(llvm::FunctionType::param_iterator param_it = fType->param_begin(),
                    param_it_end = fType->param_end(); param_it != param_it_end ; ++param_it){
                    
                    Type* OriginalType = *param_it;
                    
                    Type* type = OriginalType;
                    
                    if(type->isVectorTy()){
                        type = type->getScalarType();
                    }
                    
                    if(type->isPointerTy()){
                        type = type -> getPointerElementType();
                    }
                    
                    if(OriginalType->isStructTy()){
                        params +=  "strcut ";
                        params += OriginalType->getStructName();
                        continue;
                    }
                    
                    if(type->isIntegerTy()){
                        unsigned bitWidth = type->getIntegerBitWidth();
                        if(bitWidth==8){
                            params += "char";
                        }else{
                         params += "int";
                        }
                    }
                    else if(type->isDoubleTy()){
                        params +=  "double";
                    }
                    else if(type->isFloatTy()){
                        params += "float";
                    }
                    else if(type->isVoidTy()){
                        params +=  "void";
                    }
                    else if(type->isLabelTy()){
                        params +=  "label";
                    }
                    else{
                        params += "unknown";
                    }
                    
                    if(OriginalType->isPointerTy()){
                        params += "*";
                    }
                    if(OriginalType->isArrayTy()){
                        params += "[]";
                    }
                   
                    params +=  ",";
                }
                
                params = params.substr(0, params.size()-1);
                
                errs() << params ;
                
                int funcCallCount = 0;
                
                for(Value::use_iterator i = func_it->use_begin() ; i != func_it->use_end(); ++i){
                    
                    funcCallCount++;
                    
                    
                    
                   
//                    if (Instruction *Inst = dyn_cast<Instruction>(*i)) {
//                        
//                        BasicBlock* BB = Inst->getParent();
//                        Function* Finst = BB->getParent();
//                        Function& Fin = *Finst;
//                        
//                        LoopInfo& LI = getAnalysis<LoopInfo>(Fin);
//                        ScalarEvolution *SE = &getAnalysis<ScalarEvolution>(Fin);
//                        
//                        if( LI.getLoopDepth(BB) == 0 ){
//                            errs() << "in getLoop depthd" ;
//                            errs() << "Function Call: " << func_it->getName()<<"\n";
//                            fd.add_fun(func_it->getName(),1);
//                        }else{
//                            errs() << "in getLoop depthd else"  ;
//
//                            int loopcnt = LI.getLoopDepth(BB);
//                            int tcnt = 1;
//                            
//                            Loop *inloop = LI.getLoopFor(BB);
//                            BasicBlock *LatchBlock = inloop->getLoopLatch();
//                            
//                            if (LatchBlock) {
//                                while(loopcnt--){
//                                    int temp= ((int) SE->getSmallConstantTripCount(inloop,LatchBlock) - 1);	
//                                    if(temp == -1){ //Loop is un predictable probably dependent on User Input
//                                        tcnt=0;
//                                        fd.set_unpredicatable_flag(func_it->getName());	
//                                        break;	
//                                    }else{	
//                                        tcnt*=temp;
//                                        inloop = inloop-> getParentLoop();
//                                    }
//                                }
//                                fd.add_fun(func_it->getName(),tcnt); 	
//                                errs() << "Function Call in Loop: " << func_it->getName()<<"  "<< tcnt<<" times\n";
//                            }
//                        }
//                    }
                    
                } // Loop Value Iterator khatam
                errs() << ") : " <<funcCallCount <<"\n";
                
            }
            return flag;
        }
        
    };
}

char fncount::ID = 0;
static RegisterPass<fncount> X("fncnt","Hello Module Pass",false,false);
