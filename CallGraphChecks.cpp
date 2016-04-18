#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Value.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/ADT/StringRef.h"

#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

namespace {
    /*
     * Class Function_data keeps track of function name and its count of call
     */


     class function_data{
     public:
         llvm::StringRef fun_name;  //Name
         unsigned int numOfParams ; //Number of Paraments Parameters
         std::vector<llvm::Type::TypeID> params;        
     };


    class  CallGraphCheck : public CallGraphSCCPass{
    public:
        static char ID;
        CallGraphCheck() : CallGraphSCCPass(ID){}

        StringRef getTypeInString(Type *type){
            StringRef requiredParamType = "";
            switch(type->getTypeID()){
            case 0 :
              requiredParamType = "void";
              break;
            
            case 1:
            case 2:
              requiredParamType = "float";
              break;

            case 3:
            case 4:
            case 5:
            case 6:
              requiredParamType = "double";
              break;

            case 7:
               requiredParamType = "label";
               break;

            case 10:
                if(type->getIntegerBitWidth()==8){
                    requiredParamType = "char";
                }else{
                    requiredParamType = "int";
                }
                break;

            case 11:
                requiredParamType = "function";
                break;

            case 12:
                requiredParamType = "struct";
                break;

            case 13:
                requiredParamType = "array";
                break;

            case 14:
                requiredParamType = "pointer";
                break;   

            default :
                requiredParamType = "default";
                break;

         }
         return requiredParamType;

        }
        
        virtual bool runOnSCC(CallGraphSCC &scc) override{
            std::vector<function_data> functions;

            for(CallGraphSCC::iterator cg_itr = scc.begin(); cg_itr != scc.end(); ++cg_itr) {
                
                CallGraphNode *cgNode = *cg_itr;
                
                //Get the Function Corresponding to this CGNode
                Function *f = cgNode->getFunction();
                //If Function is non-null
                if(f!=0) {  
                    //If Function is Declared then add to the Vector
                    if(f->isDeclaration()) {
                        if(f->getName() == "llvm.dbg.declare"){
                            continue;
                        }
                        // errs() << "Declaration : " << f->getName() << " :: Address :: "<< f << "\n";
                        FunctionType *fType = f->getFunctionType();
                        
                        //Object to add in Vector
                        function_data fd ;
                        
                        //Get Function name
                        fd.fun_name = f->getName();
                        fd.numOfParams = fType->getNumParams();

                        for(llvm::FunctionType::param_iterator param_it = fType->param_begin(),
                            param_it_end = fType->param_end(); param_it != param_it_end ; ++param_it){                            
                            const Type* OriginalType = *param_it;
                            fd.params.push_back(OriginalType->getTypeID());
                            
                        }
                        functions.push_back(fd);
                        continue;
                    }
                    
                    // errs() << "in " << f->getName() << "\n";
 
                    
                    
                  //Iterator over Basic Block for the function
                    for(Function::iterator basicBlockit = f->begin() ,
                        basicBlockit_end = f->end(); basicBlockit != basicBlockit_end; ++basicBlockit) 
                    {

                        //Iterator over BasicBlock for Instructions in BB
                        for (BasicBlock::iterator II = basicBlockit->begin(), IE = basicBlockit->end(); II != IE; ++II) 
                        {                       
                            // //Get Instruction and see if it's a calling instruction;  
                            Instruction *inst = II;
                       
                            CallSite CS(cast<Value>(inst));

                            // //If callSite
                            if (CS) 
                            {
                            //  // errs () << CS.isCall() << " Calling inst " << inst->getDebugLoc().getLine() << "\n";
 
                                // const Function *fnxCalled = CS.getCalledFunction(); //Gived the Address of Function Declaration
                                // const CallInst *callInst = (CallInst *)inst;
                                CallInst &callInst = static_cast<CallInst&>(*inst);

                                const Function* fnxCalled = dyn_cast<const Function>(callInst.getCalledValue()->stripPointerCasts());
                                
                                // Random function that keeps on appearing
                                if(fnxCalled->getName() == "llvm.dbg.declare"){
                                    continue;
                                }
                                
                                if(fnxCalled->getName() == "printf"){
                                    continue;
                                }
//
                                
//                                errs () << fnxCalled << " <-Pointer \n";
                                if(!fnxCalled) {
                                    errs() << "Non-defined Function Call \n";
                                    continue;
                                }

//                                Function *fx = CS.getCalledFunction();
//                                if(fx->isDeclared()){
//                                    errs() <<"declared";
//                                }
//                                if(fx){
//                                    errs() << fx << " <- get called\n" ;
//                                }
//                                int flag = 0 ;
//                                if(fx && fx->isDeclaration()){
//                                    //Declared
//                                    errs () << "Decalred fxn " <<fx->getName() << "\n";
//                                    flag = 1;
//                                }
//                                if(!fx && (flag==0)){
//                                    errs() << "Called fxn null " <<fnxCalled->getName() << "\n";
//                                }
                                
                                
                               

                               //TODO : Remove for Debugging Only
                               //errs() << "Instruction address : " << inst << " :: Function Address :"  << fnxCalled << " :: Name : " <<fnxCalled->getName() <<"\n";  

                               unsigned int numOfArgs =  callInst.getNumArgOperands();
                               unsigned int numOfParams = fnxCalled->getFunctionType()->getNumParams();

                               //TODO : Remove -->For debugging ONLY
                               //errs() << numOfArgs << "<- arguments \n";
                               //errs() << numOfParams << "<-params \n";
                                
                                if(numOfParams==0){
                                    errs() << "Function " <<fnxCalled->getName() << " not defined \n";
                                    continue;
                                }
                               

                               if(numOfArgs != numOfParams) {
                                //Argument Mistach 
                                //Function ‘Z’ call on line ‘T’: expected ‘3’ arguments but ‘2’ are/is present.
                                 // errs() << "Function "<<fnxCalled->getName() << " call on line " << inst->getDebugLoc().getLine() << " 
                                 errs() << "Function " << fnxCalled->getName() << " call on line " << inst->getDebugLoc().getLine() << " " ;
                                 errs() << " : expected " << numOfParams << " arguments " << " but " << numOfArgs << " are/is present \n" ; 
                                 continue;
                               }

                               //Argument Number matched , Now Match Type of Params and passed args
                               FunctionType *fxnDecType = fnxCalled->getFunctionType();
                               // FunctionType *instCallType = ((Function *)callInst.getCalledValue())->getFunctionType();
                               
                               // llvm::FunctionType::param_iterator fxnCalled_it = fxnDecType->param_begin();                                             
                               // llvm::FunctionType::param_iterator fxnInst_it = instCallType->param_begin();


                               // for(unsigned int i=0; fxnCalled_it != fxnDecType->param_end() && i<numOfArgs ; i++ , fxnCalled_it++){
                               for(unsigned int i=0 ; i < numOfParams ; i++) {
                                 
                                 Type* paramType = fxnDecType->getParamType(i);
                                 
                                 Type* argType = callInst.getArgOperand(i)->getType();

                                 StringRef requiredParamType = "";
                                 StringRef foundArgType = "";

                                //TODO : Remove -->For debugging ONLY
                                 // errs() << "Args : " << argType->getTypeID() << " ";
                                 // errs() << "param : "<< paramType->getTypeID() << "\n";

                                
                                       
                                if(paramType->isVectorTy() || argType->isVectorTy()){
                                    if(paramType->isVectorTy() && argType->isVectorTy()){
                                        argType = argType->getScalarType();
                                        paramType = paramType->getScalarType();
                                    }else{           
                                        errs() << "Function " << fnxCalled->getName() << " call on line " << inst->getDebugLoc().getLine() << " " ;
                                        errs() << " argument type mismatch. Expected vector  " << getTypeInString(paramType) << " but argument is of type " << getTypeInString(argType) << "\n";
                                        break;
                                    }
                                }

                                if(paramType->isPointerTy() || argType->isPointerTy()) {
                                    if(paramType->isPointerTy() && argType->isPointerTy()){
                                        paramType = paramType->getPointerElementType();
                                        requiredParamType = "pointer";
                                        argType = argType->getPointerElementType();
                                        foundArgType = "pointer";
                                    }
                                    else{
                                        errs() << "Function " << fnxCalled->getName() << " call on line " << inst->getDebugLoc().getLine() << " " ;
                                        errs() << " argument type mismatch. Expected " << getTypeInString(paramType) << "* but argument is of type "<< getTypeInString(argType) <<" \n";
                                        break;   
                                    }
                                }
                            
                                /**
                                enum TypeID {
                                    // PrimitiveTypes - make sure LastPrimitiveTyID stays up to date.
                                    VoidTyID = 0,    ///<  0: type with no size
                                    HalfTyID,        ///<  1: 16-bit floating point type
                                    FloatTyID,       ///<  2: 32-bit floating point type
                                    DoubleTyID,      ///<  3: 64-bit floating point type
                                    X86_FP80TyID,    ///<  4: 80-bit floating point type (X87)
                                    FP128TyID,       ///<  5: 128-bit floating point type (112-bit mantissa)
                                    PPC_FP128TyID,   ///<  6: 128-bit floating point type (two 64-bits, PowerPC)
                                    LabelTyID,       ///<  7: Labels
                                    MetadataTyID,    ///<  8: Metadata
                                    X86_MMXTyID,     ///<  9: MMX vectors (64 bits, X86 specific)

                                    // Derived types... see DerivedTypes.h file.
                                    // Make sure FirstDerivedTyID stays up to date!
                                    IntegerTyID,     ///< 10: Arbitrary bit width integers
                                    FunctionTyID,    ///< 11: Functions
                                    StructTyID,      ///< 12: Structures
                                    ArrayTyID,       ///< 13: Arrays
                                    PointerTyID,     ///< 14: Pointers
                                    VectorTyID       ///< 15: SIMD 'packed' format, or other vector type
                                  };
                                */
                               
                                    if( (argType->getTypeID() !=  paramType->getTypeID()) || (argType->getIntegerBitWidth() !=  paramType->getIntegerBitWidth()) ) 
                                    {
                                     
                                        requiredParamType = getTypeInString(paramType);
                                        foundArgType = getTypeInString(argType);
                                        errs() << "Function " << fnxCalled->getName() << " call on line " << inst->getDebugLoc().getLine() << " " ;
                                        errs() << " argument type mismatch. Expected " << requiredParamType << " but argument is of type " << foundArgType << "\n";
                                        break;
                                                    
                                    } //If type not equals ends

                                }//For loop for params

                            } //if CallSite
                          
                        } // Basic Block's Iterator

                    }//Function's Iterator  

                }//If cgNode.function!=0

            }//CallGraphIterator

            // Print all Declared functions in the Global Vector for debugging
            // for(std::vector<function_data>::iterator function = functions.begin() ; function != functions.end(); ++function){
            //     errs() << " Name : " << function->fun_name<<"\n";
            //     errs() << " Num of Ops : " <<function->numOfParams << "\n";

            //     for(std::vector<Type::TypeID>::iterator type = function->params.begin() ; type != function->params.end(); ++type){
            //         errs() << *type << ", ";
            //     }

            //     errs() << "\n";
            //  }
            return false;
        }
            
    };
}

char CallGraphCheck::ID = 0;
static RegisterPass<CallGraphCheck> X("cgcheck","Hello Module Pass",false,false);
