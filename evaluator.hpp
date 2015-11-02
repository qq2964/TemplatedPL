#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include "utils.hpp"

// Evaluator
namespace TPL
{
    namespace internal
    {
        using CYTL::UTIL::enable_if;
        using CYTL::UTIL::EmptyType;
        using CYTL::UTIL::Select;
        using CYTL::UTIL::IsEQ;

        // ------------------------------------------
        // VarValList
        struct EmptyVarValList;
        template<class Varible, class Val, class Tail> struct VarValList;
        template<class Varible, class Val>
        struct VarValList<Varible, Val, EmptyVarValList>;

        // VarValListExtend
        template<class Varible, class Val, class L>
        struct VarValListExtend
        { typedef VarValList<Varible, Val, L> value; };

        // VarValListLookup
        template<class V, class L> struct VarValListLookup;
        template<class V, class Varible, class Val, class Tail> // Search next Node
        struct VarValListLookup< V, VarValList<Varible, Val, Tail> >
        { typedef typename VarValListLookup<V, Tail>::value value; };
        template<class V, class Val, class Tail> // V found
        struct VarValListLookup< V, VarValList<V, Val, Tail> >
        { typedef Val value; };
        template<class V> // till the end, nothing is found
        struct VarValListLookup<V, EmptyVarValList>
        { typedef EmptyType value; };



        // -------------------------------------------------
        // Env
        struct EmptyEnv;
        template<class Head, class Tail> struct Env;
        template<class T> struct Env<T, EmptyEnv>;

        // EnvExtend
        template<class Head, class E>
        struct EnvExtend
        { typedef Env<Head, E> value; };

        // EnvLookup
        template<class V, class E> struct EnvLookup;

        template<class V, class L, class ETail> //
        struct EnvLookup< V, Env<L, ETail> >
        {
            typedef typename VarValListLookup<V, L>::value ResultType;
            typedef typename Select< IsEQ<ResultType, EmptyType>::value,
                                     typename EnvLookup<V, ETail>::value,
                                     ResultType >::value
                    value;
        };
        template<class V> // till the end, nothing is found
        struct EnvLookup<V, EmptyEnv>
        { typedef EmptyType value; };



        // ---------------------------------------------------------------------------
        // ApplyKont
        template<class Kont, class Val> struct ApplyKont;


        // ----------------------------------------------------------------------------
        // EndKont
        struct EndKont;

        template<class Val>
        struct ApplyKont<EndKont, Val>
        { typedef Val value; };



        //-------------------------------------------------------------------------------
        // Eval
        template<class Exp, class Environ, class Kont> struct EvalUnderEnvKont;

        template<class Exp>
        struct Eval
        { typedef typename EvalUnderEnvKont<Exp, EmptyEnv, EndKont>::value value; };



        // -------------------------------------------------------------------------------
        // Basic Data Type
        template<int N> struct Int{ static const int value = N; };
        template<bool B> struct Bool{ static const bool value = B; };
        template<int N> struct Var;
        struct Unit;

        template<int N, class Environ, class Kont> //
        struct EvalUnderEnvKont< Int<N>, Environ, Kont >
        { typedef typename ApplyKont< Kont, Int<N> >::value value; };

        template<bool B, class Environ, class Kont> //
        struct EvalUnderEnvKont< Bool<B>, Environ, Kont >
        { typedef typename ApplyKont< Kont, Bool<B> >::value value; };

        template<int N, class Environ, class Kont> //
        struct EvalUnderEnvKont< Var<N>, Environ, Kont >
        { typedef typename ApplyKont< Kont, EnvLookup< Var<N>, Environ > >::value value; };

        template<class Environ, class Kont> //
        struct EvalUnderEnvKont< Unit, Environ, Kont >
        { typedef typename ApplyKont<Kont, Unit>::value value; };



        // -----------------------------------------------------------------------------------
        // Add
        template<class T1, class T2> struct Add;

        // AddKont
        template<class AddendExp, class Environ, class Kont> struct AddKont1;
        template<class Addend, class Kont> struct AddKont2;

        // AddKont1
        template<class AddendExp, class Environ, class Kont, int N>
        struct ApplyKont< AddKont1<AddendExp, Environ, Kont>, Int<N> >
        { typedef typename EvalUnderEnvKont< AddendExp, Environ, AddKont2< Int<N>, Kont > >::value value; };

        // AddKont2
        template<int N1, class Kont, int N2>
        struct ApplyKont< AddKont2< Int<N1>, Kont >, Int<N2> >
        { typedef typename ApplyKont< Kont, Int<N1+N2> >::value value; };

        // Add Eval
        template<class T1, class T2, class Environ, class Kont> //
        struct EvalUnderEnvKont< Add<T1, T2>, Environ, Kont >
        { typedef typename EvalUnderEnvKont< T1, Environ, AddKont1<T2, Environ, Kont> >::value value; };

        // --------------------------------------------------------------------------------------
        // Mul
        template<class T1, class T2> struct Mul;

        // MulKont
        template<class MultiplierExp, class Environ, class Kont> struct MulKont1;
        template<class Multiplier, class Kont> struct MulKont2;

        // MulKont1
        template<class MultiplierExp, class Environ, class Kont, int N>
        struct ApplyKont< MulKont1<MultiplierExp, Environ, Kont>, Int<N> >
        { typedef typename EvalUnderEnvKont< MultiplierExp, Environ, MulKont2< Int<N>, Kont > >::value value; };

        // MulKont2
        template<int N1, class Kont, int N2>
        struct ApplyKont< MulKont2< Int<N1>, Kont >, Int<N2> >
        { typedef typename ApplyKont< Kont, Int<N1*N2> >::value value; };

        // Mul Eval
        template<class T1, class T2, class Environ, class Kont> //
        struct EvalUnderEnvKont< Mul<T1, T2>, Environ, Kont >
        { typedef typename EvalUnderEnvKont< T1, Environ, MulKont1<T2, Environ, Kont> >::value value; };

        // -----------------------------
        // Mod
        template<class T1, class T2> struct Mod;

        // ModKont
        template<class ModendExp, class Environ, class Kont> struct ModKont1;
        template<class Modend, class Kont> struct ModKont2;

        // MulKont1
        template<class ModendExp, class Environ, class Kont, int N>
        struct ApplyKont< ModKont1<ModendExp, Environ, Kont>, Int<N> >
        { typedef typename EvalUnderEnvKont< ModendExp, Environ, ModKont2< Int<N>, Kont > >::value value; };

        // MulKont2
        template<int N1, class Kont, int N2>
        struct ApplyKont< ModKont2< Int<N1>, Kont >, Int<N2> >
        { typedef typename ApplyKont< Kont, Int<N1%N2> >::value value; };

        // Mod Eval
        template<class T1, class T2, class Environ, class Kont> //
        struct EvalUnderEnvKont< Mod<T1, T2>, Environ, Kont >
        { typedef typename EvalUnderEnvKont< T1, Environ, ModKont1<T2, Environ, Kont> >::value value; };



        // -----------------------------------------------------------------------------------------------------------
        // Comparing

        //----------------------------------------
        // >
        template<class T1, class T2> struct IsGreater;

        // >Kont
        template<class GreaterExp, class Environ, class Kont> struct IsGreaterKont1;
        template<class Greaterend, class Kont> struct IsGreaterKont2;

        // >Kont1
        template<class GreaterExp, class Environ, class Kont, int N>
        struct ApplyKont< IsGreaterKont1<GreaterExp, Environ, Kont>, Int<N> >
        { typedef typename EvalUnderEnvKont< GreaterExp, Environ, IsGreaterKont2< Int<N>, Kont > >::value value; };

        // >Kont2
        template<int N1, class Kont, int N2>
        struct ApplyKont< IsGreaterKont2< Int<N1>, Kont >, Int<N2> >
        { typedef typename ApplyKont< Kont, Bool<(N1>N2)> >::value value; };

        // > Eval
        template<class T1, class T2, class Environ, class Kont> //
        struct EvalUnderEnvKont< IsGreater<T1, T2>, Environ, Kont >
        { typedef typename EvalUnderEnvKont< T1, Environ, IsGreaterKont1<T2, Environ, Kont> >::value value; };

        //----------------------------------------
        // <
        template<class T1, class T2> struct IsLess;

        // <Kont
        template<class LessExp, class Environ, class Kont> struct IsLessKont1;
        template<class Lessend, class Kont> struct IsLessKont2;

        // <Kont1
        template<class LessExp, class Environ, class Kont, int N>
        struct ApplyKont< IsLessKont1<LessExp, Environ, Kont>, Int<N> >
        { typedef typename EvalUnderEnvKont< LessExp, Environ, IsLessKont2< Int<N>, Kont > >::value value; };

        // <Kont2
        template<int N1, class Kont, int N2>
        struct ApplyKont< IsLessKont2< Int<N1>, Kont >, Int<N2> >
        { typedef typename ApplyKont< Kont, Bool<(N1<N2)> >::value value; };

        // < Eval
        template<class T1, class T2, class Environ, class Kont> //
        struct EvalUnderEnvKont< IsLess<T1, T2>, Environ, Kont >
        { typedef typename EvalUnderEnvKont< T1, Environ, IsLessKont1<T2, Environ, Kont> >::value value; };

        //----------------------------------------
        // ==
        template<class T1, class T2> struct IsEqual;

        // ==Kont
        template<class EqualExp, class Environ, class Kont> struct IsEqualKont1;
        template<class Equalend, class Kont> struct IsEqualKont2;

        // ==Kont1
        template<class EqualExp, class Environ, class Kont, int N>
        struct ApplyKont< IsEqualKont1<EqualExp, Environ, Kont>, Int<N> >
        { typedef typename EvalUnderEnvKont< EqualExp, Environ, IsEqualKont2< Int<N>, Kont > >::value value; };

        // ==Kont2
        template<int N1, class Kont, int N2>
        struct ApplyKont< IsEqualKont2< Int<N1>, Kont >, Int<N2> >
        { typedef typename ApplyKont< Kont, Bool<(N1==N2)> >::value value; };

        // == Eval
        template<class T1, class T2, class Environ, class Kont> //
        struct EvalUnderEnvKont< IsEqual<T1, T2>, Environ, Kont >
        { typedef typename EvalUnderEnvKont< T1, Environ, IsEqualKont1<T2, Environ, Kont> >::value value; };

        //----------------------------------------
        // IsUnit
        template<class T> struct IsUnit;

        // IsUnitKont
        template<class Kont> struct IsUnitKont;

        template<class Kont> //
        struct ApplyKont< IsUnitKont<Kont>, Unit >
        { typedef typename ApplyKont< Kont, Bool<true> >::value value; };

        template<class Kont, class Val> //
        struct ApplyKont< IsUnitKont<Kont>, Val >
        { typedef typename ApplyKont< Kont, Bool<false> >::value value; };

        // == Eval
        template<class T, class Environ, class Kont> //
        struct EvalUnderEnvKont< IsUnit<T>, Environ, Kont >
        { typedef typename EvalUnderEnvKont< T, Environ, IsUnitKont<Kont> >::value value; };



        // // ------------------------------------
        // // Pair type
        // template<class T1, class T2> struct Pair;

        // template<class T1, class T2, class Environ> //
        // struct EvalUnderEnv< Pair<T1, T2>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T1, Environ>::value T1Val;
        //     typedef typename EvalUnderEnv<T2, Environ>::value T2Val;
        //     typedef Pair<T1Val, T2Val> value;
        // };

        // // Pair.1
        // template<class T>struct Fst;

        // template<class T1, class T2, class Environ> //
        // struct EvalUnderEnv< Fst< Pair<T1, T2> >, Environ >
        // { typedef T1 value; };

        // template<class T, class Environ> //
        // struct EvalUnderEnv< Fst<T>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T, Environ>::value TVal;
        //     typedef typename EvalUnderEnv< Fst<TVal>, Environ >::value value;
        // };

        // // Pair.2
        // template<class T>struct Snd;

        // template<class T1, class T2, class Environ> //
        // struct EvalUnderEnv< Snd< Pair<T1, T2> >, Environ >
        // { typedef T2 value; };

        // template<class T, class Environ> //
        // struct EvalUnderEnv< Snd<T>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T, Environ>::value TVal;
        //     typedef typename EvalUnderEnv< Snd<TVal>, Environ >::value value;
        // };



        // // -----------------------------------
        // // List type
        // template<class... T_Rest> struct List;

        // template<class T, class... T_Rest, class Environ> //
        // struct EvalUnderEnv< List<T, T_Rest...>, Environ >
        // {
        //     static const int ArgsNum = sizeof...(T_Rest) + 1;
        //     typedef typename EvalUnderEnv<T, Environ>::value TVal;

        //     typedef typename Select< ArgsNum == 1,
        //                              Unit,
        //                              List<T_Rest...> >::value
        //             TailType;
        //     typedef Pair< TVal, typename EvalUnderEnv< TailType, Environ >::value > value;
        // };


        // //IsList
        // template<class T>
        // struct IsList
        // { static const bool MayBeList = false; };
        // template<class T1, class T2> //
        // struct IsList< Pair<T1, T2> >
        // { static const bool MayBeList = true; };
        // template<>  //
        // struct IsList<Unit>
        // { static const bool MayBeList = true; };

        // template<class T, class Environ> //
        // struct EvalUnderEnv< IsList<T>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T, Environ>::value TVal;
        //     typedef typename Select< IsList<TVal>::MayBeList,
        //                              TVal,
        //                              EmptyType >::value
        //             TmpType;
        //     typedef typename EvalUnderEnv< IsList<TmpType>, Environ >::value value;
        // };
        // template<class T1, class T2, class Environ> //
        // struct EvalUnderEnv< IsList< Pair<T1, T2> >, Environ >
        // {
        //     typedef typename EvalUnderEnv<T2, Environ>::value T2Val;
        //     typedef typename EvalUnderEnv< IsList<T2Val>, Environ >::value value;
        // };
        // template<class T, class Environ> //
        // struct EvalUnderEnv< IsList< Pair<T, Unit> >, Environ >
        // { typedef Bool<true> value; };
        // template<class Environ> //
        // struct EvalUnderEnv< IsList<Unit>, Environ >
        // { typedef Bool<true> value; };
        // template<class Environ> //
        // struct EvalUnderEnv< IsList< EmptyType >, Environ >
        // { typedef Bool<false> value; };


        // // List.N
        // template<class T, class N>
        // struct ListRef
        // { static const bool MayBeList = false; };
        // template<class T1, class T2, int N> //
        // struct ListRef< Pair<T1, T2>, Int<N> >
        // { static const bool MayBeList = true; };

        // template<class T, class N, class Environ> //
        // struct EvalUnderEnv< ListRef<T, N>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T, Environ>::value TVal;
        //     typedef typename EvalUnderEnv<N, Environ>::value Num;

        //     typedef typename Select< ListRef<TVal, Num>::MayBeList,
        //                              TVal,
        //                              EmptyType >::value
        //             TmpType;
        //     typedef typename EvalUnderEnv< ListRef<TmpType, Num>, Environ >::value value;
        // };
        // template<class T1, class T2, int N, class Environ> //
        // struct EvalUnderEnv< ListRef< Pair<T1, T2>, Int<N> >, Environ >
        // {
        //     typedef typename EvalUnderEnv<T2, Environ>::value T2Val;
        //     typedef typename EvalUnderEnv< ListRef< T2Val, Int<N - 1> >, Environ>::value value;
        // };
        // template<class T1, class T2, class Environ> //
        // struct EvalUnderEnv< ListRef< Pair<T1, T2>, Int<0> >, Environ >
        // {
        //     typedef typename EvalUnderEnv< IsList<T2>, Environ>::value IsT2List;
        //     typedef typename enable_if< IsEQ< IsT2List,
        //                                       Bool<true> >::value,
        //                                 IsT2List >::type
        //             ISLIST;

        //     typedef typename EvalUnderEnv<T1, Environ>::value value;
        // };
        // template<class N, class Environ> //
        // struct EvalUnderEnv< ListRef< EmptyType, N >, Environ >
        // { typedef Bool<false> value; };


        // // ListAppend
        // template<class L, class NewT> struct ListAppend;

        // template<class T, class NewT, class Environ> //
        // struct EvalUnderEnv< ListAppend<T, NewT>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T, Environ>::value TVal;
        //     typedef typename EvalUnderEnv<NewT, Environ>::value NewTVal;
        //     typedef typename EvalUnderEnv< ListAppend<TVal, NewTVal>, Environ >::value value;
        // };

        // template<class H, class T, class NewT, class Environ> //
        // struct EvalUnderEnv< ListAppend<Pair<H, T>, NewT>, Environ >
        // {
        //     typedef typename EvalUnderEnv<H, Environ>::value HVal;
        //     typedef typename EvalUnderEnv<T, Environ>::value TVal;
        //     typedef typename EvalUnderEnv<NewT, Environ>::value NewTVal;
        //     typedef Pair< HVal,
        //                   typename EvalUnderEnv< ListAppend<TVal, NewTVal>, Environ >::value >
        //             value;
        // };

        // template<class NewT, class Environ> //
        // struct EvalUnderEnv< ListAppend<Unit, NewT>, Environ >
        // {
        //     typedef typename EvalUnderEnv<NewT, Environ>::value NewTVal;
        //     typedef Pair<NewTVal, Unit> value;
        // };



        // // ------------------------------------
        // // If_Then_Else
        // template<class Cond, class T1, class T2> struct If_Then_Else;

        // template<class Cond, class T1, class T2, class Environ> //
        // struct EvalUnderEnv< If_Then_Else<Cond, T1, T2>, Environ >
        // {
        //     typedef typename EvalUnderEnv<Cond, Environ>::value CondVal;
        //     typedef typename EvalUnderEnv< If_Then_Else<CondVal, T1, T2>, Environ >::value value;
        // };

        // template<class T1, class T2, class Environ> //
        // struct EvalUnderEnv< If_Then_Else<Bool<true>, T1, T2>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T1, Environ>::value T1Val;
        //     typedef T1Val value;
        // };

        // template<class T1, class T2, class Environ> //
        // struct EvalUnderEnv< If_Then_Else<Bool<false>, T1, T2>, Environ >
        // {
        //     typedef typename EvalUnderEnv<T2, Environ>::value T2Val;
        //     typedef T2Val value;
        // };



        // // ---------------------------------------------
        // // Lambda

        // // ParamList
        // template<class... Param> struct ParamList;

        // template<class Param, class... ParamTail, class Environ> //
        // struct EvalUnderEnv< ParamList< Param, ParamTail... >, Environ >
        // { typedef ParamList< Param, ParamTail... > value; };


        // // Closure
        // template<class Environ, class Fn> struct Closure;

        // template<class E, class Fn, class Environ> //
        // struct EvalUnderEnv< Closure<E, Fn>, Environ >
        // { typedef Closure<E, Fn> value; };


        // // Lambda
        // template<class ParamL, class Body> struct Lambda;

        // template<class Param, class... ParamTail, class Body, class Environ> //
        // struct EvalUnderEnv< Lambda< ParamList< Param, ParamTail... >, Body >, Environ >
        // {
        //     typedef ParamList< Param, ParamTail... > PVal;
        //     typedef Closure< Environ, Lambda<PVal, Body> > value;
        // };



        // // ------------------------------------------------------------
        // // Apply

        // // Binding
        // template<class VarValL, class Environ, class PL, class... Val> struct Binding;

        // template<class VarValL, class Environ, //
        //          int N, class... ParamRest,
        //          class Val, class... ValRest>
        // struct Binding< VarValL, Environ,
        //                 ParamList< Var<N>, ParamRest... >,
        //                 Val, ValRest... >
        // {
        //     typedef typename EvalUnderEnv<Val, Environ>::value TVal;
        //     typedef typename VarValListExtend< Var<N>, TVal, VarValL >::value ExtVarValL;

        //     static const int ParamNum = sizeof...(ParamRest)+1;
        //     static const int ValNum = sizeof...(ValRest)+1;

        //     typedef typename Select< ParamNum ==1 && ValNum == 1,
        //                              EmptyType,
        //                              ParamList<ParamRest...> >::value
        //             TmpParam;

        //     typedef typename Binding< ExtVarValL, Environ, TmpParam, ValRest... >::value value;
        // };
        // template<class VarValL, class Environ, class... ValRest> //
        // struct Binding< VarValL, Environ, EmptyType, ValRest... >
        // { typedef VarValL value; };


        // // Call
        // template<class Fn, class... Val> struct Call;

        // template<class T, class Val, class... ValRest, class Environ> //
        // struct EvalUnderEnv< Call< T, Val, ValRest... >, Environ >
        // {
        //     typedef typename EvalUnderEnv< T, Environ >::value TVal;
        //     typedef typename EvalUnderEnv< Call<TVal, Val, ValRest...>, Environ >::value value;
        // };

        // template<class E, //
        //          class Param, class... ParamRest, class Body,
        //          class Val, class... ValRest,
        //          class Environ>
        // struct EvalUnderEnv< Call< Closure< E,
        //                                     Lambda< ParamList<Param, ParamRest...>, Body > >,
        //                            Val, ValRest... >,
        //                      Environ >
        // {
        //     typedef typename Binding< EmptyVarValList, Environ,
        //                               ParamList<Param, ParamRest...>,
        //                               Val, ValRest... >::value
        //             ExtVarValL;
        //     typedef typename EnvExtend<ExtVarValL, E>::value ExtEnv;

        //     typedef typename EvalUnderEnv<Body, ExtEnv>::value value;
        // };

    } // namespace internal

} // namespace TPL

#endif //EVALUATOR_H_