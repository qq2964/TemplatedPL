#include "evaluator.hpp"
#include "utils.hpp"


using namespace TPL;
using namespace CYTL;
using UTIL::StaticCheckEQ;
using UTIL::ComileTimeCheck;


int main()
{
    // -------------------------------------------------------
    // Basic Type Testing
    StaticCheckEQ< Eval< Int<4> >, Int<4> >;
    StaticCheckEQ< Eval< Bool<true> >, Bool<true> >;
    StaticCheckEQ< Eval< Var<4> >, Var<4> >;
    StaticCheckEQ< Eval< Unit >, Unit >;

    // -------------------------------------------------------
    // Add Testing
    StaticCheckEQ< Eval< Add< Add< Int<1>, Int<3> >, Int<4> > >::value, Int<8> >();
    StaticCheckEQ< Eval< Add< Int<-9>, Int<45> > >::value, Int<36> >();


    // -------------------------------------------------------
    // Comparing Testing
    StaticCheckEQ< Eval< IsGreater< Add< Int<1>, Int<4> >, Int<8> > >::value, Bool<false> >();
    StaticCheckEQ< Eval< IsLess< Add< Int<1>, Int<4> >, Int<8> > >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsEqual< Add< Int<4>, Int<4> >, Int<8> > >::value, Bool<true> >();

    StaticCheckEQ< Eval< IsUnit<Unit> >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsUnit< Add< Int<2>, Int<2> > > >::value, Bool<false> >();
    //StaticCheckEQ< Eval< IsUnit< List< Int<9>, Int<2> >::value > >::value, Bool<false> >();


    // -------------------------------------------------------
    // Pair Testing
    typedef Pair< Pair< Int<4>, Bool<true> >, Pair< Pair<Int<2>, Unit>, Bool<false> > > P;
    StaticCheckEQ< Eval<P>::value, P >();
    StaticCheckEQ< Eval< Fst< Fst<P> > >::value, Int<4> >();
    StaticCheckEQ< Eval< Snd< Fst< Snd<P> > > >::value, Unit >();
    StaticCheckEQ< Eval< Snd< Snd<P> > >::value, Bool<false> >();


    // -------------------------------------------------------
    // List Testing
    typedef List< List< Int<2>, Bool<false> >, 
                  List< Int<4>, Bool<true> >, 
                  List< Int<6> > >
            L1;
    typedef Pair< Pair< Int<2>, Pair<Bool<false>, Unit> >, 
                  Pair< Pair< Int<4>, Pair<Bool<true>, Unit> >,
                        Pair< Pair<Int<6>, Unit>,
                              Unit > > >
            P1;
    StaticCheckEQ< Eval<L1>::value, Eval<P1>::value >();
    StaticCheckEQ< Eval< List< Int<5> > >::value, Pair< Int<5>, Unit > >();

    // IsList
    StaticCheckEQ< Eval< IsList<L1> >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsList<P1> >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsList<Unit> >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsList< Snd<Snd<P1> > > >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsList<P> >::value, Bool<false> >();

    //// List.N
    typedef List< Int<0>, Int<1>, Int<2>, Int<3>, Int<4> > L3;
    StaticCheckEQ< Eval< ListRef< L3, Add< Int<2>, Int<0> > > >::value, Int<2> >();
    StaticCheckEQ< Eval< ListRef< ListRef< L1, Add< Int<0>, Int<0> > >, 
                                  Add< Int<-2>, Int<2> > > >::value, 
                   Int<2> >(); 

    // ListAppend
    typedef List< Int<0>, Int<1>, Int<2>, Int<3>, Int<4>, Int<5> > L4;
    StaticCheckEQ< Eval< ListAppend< L3, Int<5> > >::value, Eval<L4>::value >();
    StaticCheckEQ< Eval< ListAppend< ListAppend< L3, Add< Int<1>, Int<4> > >, 
                                     List< Int<9> > > >::value, 
                   Eval< ListAppend< L4, List< Int<9> > > >::value >();


    // -------------------------------------------------------
    // if-then-else Testing
    StaticCheckEQ< Eval< If_Then_Else< IsGreater< Int<5>, Int<8> >,
                                       L1,
                                       P > >::value,
                    P >();
    StaticCheckEQ< Eval< If_Then_Else< IsLess< Int<5>, Int<8> >,
                                       L1,
                                       P> >::value,
                   Eval<L1>::value >();


    // -----------------------------------------------------
    // VarValList
    typedef VarValList< Var<0>, Int<0>,
                        VarValList< Var<1>, Int<1>, EmptyVarValList > >
            VarValL0;
    typedef VarValList< Var<2>, Int<2>, 
                        VarValList< Var<0>, Int<0>,
                                    VarValList< Var<1>, Int<1>, 
                                                EmptyVarValList > > >
            VarValL1;
    // VarValListExtend
    StaticCheckEQ< VarValListExtend< Var<2>, Int<2>, VarValL0 >::value,
                   VarValL1 >;

    // VarValListLookup
    StaticCheckEQ< VarValListLookup<Var<2>, VarValL1>::value, Int<2> >();
    StaticCheckEQ< VarValListLookup<Var<0>, VarValL1>::value, Int<0> >();
    StaticCheckEQ< VarValListLookup<Var<1>, VarValL1>::value, Int<1> >();
    StaticCheckEQ< VarValListLookup<Var<3>, VarValL1>::value, UTIL::EmptyType >();


    // -----------------------------------------------------
    // Env
    typedef VarValList< Var<8>, Int<8>,
                        VarValList< Var<9>, Int<9>, EmptyVarValList > >
            VarValL2;
    typedef VarValList< Var<5>, Int<5>,
                        VarValList< Var<6>, Int<6>,
                                    VarValList< Var<7>, Int<7>,
                                                EmptyVarValList > > >
            VarValL3;

    typedef Env< VarValL2,
                 Env< VarValL3, EmptyEnv > >
            E0;
    typedef Env< VarValL0, 
                 Env< VarValL1,
                      Env< VarValL2,
                           Env< VarValL3, EmptyEnv > > > >
            E1;
    // EnvExtend
    StaticCheckEQ< EnvExtend<VarValL0, EnvExtend<VarValL1, E0>::value>::value, E1 >();

    // EnvLookup
    StaticCheckEQ< EnvLookup<Var<0>, E1>::value, Int<0> >();
    StaticCheckEQ< EnvLookup<Var<0>, E0>::value, UTIL::EmptyType >();
    StaticCheckEQ< EnvLookup<Var<8>, E1>::value, Int<8> >();
    StaticCheckEQ< EnvLookup<Var<7>, E1>::value, Int<7> >();
    StaticCheckEQ< EnvLookup<Var<6>, E0>::value, Int<6> >();

    return 0;
}