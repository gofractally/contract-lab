#pragma once
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/mp11/tuple.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/facilities/check_empty.hpp>
#include <boost/preprocessor/logical/bitand.hpp>
#include <boost/preprocessor/logical/compl.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <eosio/action.hpp>

extern "C"
{
   /**
    * Set the action return value which will be included in the action_receipt
    * @brief Set the action return value
    * @param return_value - serialized return value
    * @param size - size of serialized return value in bytes
    * @pre `return_value` is a valid pointer to an array at least `size` bytes long
    */
   [[clang::import_name("set_action_return_value")]] void set_action_return_value(
       void* return_value,
       size_t size);
}  // extern "C"

namespace eosio
{
   inline constexpr name any_contract;

   /**
    * Unpack the received action and execute the correponding action handler
    *
    * @ingroup dispatcher
    * @tparam T - The contract class that has the correponding action handler, this contract should
    * be derived from eosio::contract
    * @tparam Q - The namespace of the action handler function
    * @tparam Args - The arguments that the action handler accepts, i.e. members of the action
    * @param obj - The contract object that has the correponding action handler
    * @param func - The action handler
    * @return true
    */
   template <typename T, typename R, typename... Args>
   bool execute_action(name self, name code, R (T::*func)(Args...))
   {
      size_t size = action_data_size();

      // using malloc/free here potentially is not exception-safe, although WASM doesn't support
      // exceptions
      void* buffer = nullptr;
      if (size > 0)
      {
         buffer = malloc(size);
         read_action_data(buffer, size);
      }

      std::tuple<std::decay_t<Args>...> args;
      datastream<const char*> ds((char*)buffer, size);
      ds >> args;

      T inst(self, code, ds);

      auto f2 = [&](auto... a) { return ((&inst)->*func)(a...); };

      if constexpr (!std::is_same_v<void, R>)
      {
         auto r = eosio::pack(boost::mp11::tuple_apply(f2, args));
         ::set_action_return_value(r.data(), r.size());
      }
      else
      {
         boost::mp11::tuple_apply(f2, args);
      }

      if (size)
      {
         free(buffer);
      }
      return true;
   }

   template <auto Action>
   struct action_type_wrapper
   {
      using args = detail::deduced<Action>;
   };

   template <typename T, typename R, typename Session, typename... Args>
   void execute_verb(eosio::name contract,
                     R (T::*func)(const Session& current_session, Args...),
                     const auto& current_session,
                     eosio::datastream<const char*>& ds)
   {
      std::tuple<std::remove_cvref_t<Args>...> t;
      ds >> t;
      T inst(contract, contract, ds);
      std::apply([&](auto&... args) { (inst.*func)(current_session, std::move(args)...); }, t);
   }

#define EOSIO_EMPTY(...)
#define EOSIO_COMMA_STRINGIZE(arg) , BOOST_PP_STRINGIZE(arg)
#define EOSIO_DEFAULT_IF_EMPTY(x, def) BOOST_PP_IIF(BOOST_PP_CHECK_EMPTY(x), def, x)

#define EOSIO_MATCH_CHECK_N(x, n, r, ...) \
   BOOST_PP_BITAND(n, BOOST_PP_COMPL(BOOST_PP_CHECK_EMPTY(r)))
#define EOSIO_MATCH_CHECK(...) EOSIO_MATCH_CHECK_N(__VA_ARGS__, 0, )
#define EOSIO_MATCH_YES ~, 1,
#define EOSIO_MATCH(base, x) EOSIO_MATCH_CHECK(BOOST_PP_CAT(base, x))

#define EOSIO_MATCH_ACTION_VERB(x) EOSIO_MATCH(EOSIO_MATCH_ACTION_VERB, x)
#define EOSIO_MATCH_ACTION_VERBaction EOSIO_MATCH_YES
#define EOSIO_MATCH_ACTION_VERBverb EOSIO_MATCH_YES
#define EOSIO_MATCH_ACTION_VERBaction_verb EOSIO_MATCH_YES
#define EOSIO_EXTRACT_ACTION_VERB_NAME(x) \
   BOOST_PP_IIF(EOSIO_MATCH_ACTION_VERB(x), BOOST_PP_CAT(EOSIO_EXTRACT_ACTION_VERB_NAME, x), x)
#define EOSIO_EXTRACT_ACTION_VERB_NAMEaction(name, ...) name
#define EOSIO_EXTRACT_ACTION_VERB_NAMEverb(name, index, ...) name
#define EOSIO_EXTRACT_ACTION_VERB_NAMEaction_verb(name, index, ...) name

#define EOSIO_HAS_ACTION_VERB_ARGS(action)          \
   BOOST_PP_BITAND(EOSIO_MATCH_ACTION_VERB(action), \
                   BOOST_PP_COMPL(BOOST_PP_CHECK_EMPTY(EOSIO_EXTRACT_ACTION_VERB_ARGS(action))))
#define EOSIO_EXTRACT_ACTION_VERB_ARGS(x) BOOST_PP_CAT(EOSIO_EXTRACT_ACTION_VERB_ARGS, x)
#define EOSIO_EXTRACT_ACTION_VERB_ARGSaction(name, ...) __VA_ARGS__
#define EOSIO_EXTRACT_ACTION_VERB_ARGSverb(name, index, ...) __VA_ARGS__
#define EOSIO_EXTRACT_ACTION_VERB_ARGSaction_verb(name, index, ...) __VA_ARGS__

#define EOSIO_ACTION_ARG_NAME_STRINGS_1(r, _, i, arg) \
   BOOST_PP_IIF(EOSIO_MATCH_RICARDIAN(arg), EOSIO_EMPTY, EOSIO_COMMA_STRINGIZE)(arg)
#define EOSIO_ACTION_ARG_NAME_STRINGS_2(action)                \
   BOOST_PP_SEQ_FOR_EACH_I(EOSIO_ACTION_ARG_NAME_STRINGS_1, _, \
                           BOOST_PP_VARIADIC_TO_SEQ(EOSIO_EXTRACT_ACTION_VERB_ARGS(action)))
#define EOSIO_ACTION_ARG_NAME_STRINGS(action)                                                     \
   BOOST_PP_IIF(EOSIO_HAS_ACTION_VERB_ARGS(action), EOSIO_ACTION_ARG_NAME_STRINGS_2, EOSIO_EMPTY) \
   (action)

#define EOSIO_MATCH_RICARDIAN(action_arg) EOSIO_MATCH(EOSIO_MATCH_RICARDIAN, action_arg)
#define EOSIO_MATCH_RICARDIANricardian_contract EOSIO_MATCH_YES
#define EOSIO_EXTRACT_RICARDIAN(action_arg) BOOST_PP_CAT(EOSIO_EXTRACT_RICARDIAN, action_arg)
#define EOSIO_EXTRACT_RICARDIANricardian_contract(value) value

#define EOSIO_GET_RICARDIAN_3(r, _, i, arg) \
   BOOST_PP_IIF(EOSIO_MATCH_RICARDIAN(arg), EOSIO_EXTRACT_RICARDIAN, EOSIO_EMPTY)(arg)
#define EOSIO_GET_RICARDIAN_2(action)                \
   BOOST_PP_SEQ_FOR_EACH_I(EOSIO_GET_RICARDIAN_3, _, \
                           BOOST_PP_VARIADIC_TO_SEQ(EOSIO_EXTRACT_ACTION_VERB_ARGS(action)))
#define EOSIO_GET_RICARDIAN_1(action) \
   BOOST_PP_IIF(EOSIO_HAS_ACTION_VERB_ARGS(action), EOSIO_GET_RICARDIAN_2, EOSIO_EMPTY)(action)
#define EOSIO_GET_RICARDIAN(action) EOSIO_DEFAULT_IF_EMPTY(EOSIO_GET_RICARDIAN_1(action), "")

#define EOSIO_MATCH_NOTIFY(x) EOSIO_MATCH(EOSIO_MATCH_NOTIFY, x)
#define EOSIO_MATCH_NOTIFYnotify EOSIO_MATCH_YES
#define EOSIO_EXTRACT_NOTIFY_CODE(x) BOOST_PP_CAT(EOSIO_EXTRACT_NOTIFY_CODE, x)
#define EOSIO_EXTRACT_NOTIFY_CODEnotify(code, action) code
#define EOSIO_EXTRACT_NOTIFY_ACTION(x) BOOST_PP_CAT(EOSIO_EXTRACT_NOTIFY_ACTION, x)
#define EOSIO_EXTRACT_NOTIFY_ACTIONnotify(code, action) action

// Things which shouldn't be handled by EOSIO_DISPATCH_ACTION
#define EOSIO_MATCH_NOT_DISPATCH_ACTION(x) EOSIO_MATCH(EOSIO_MATCH_NOT_DISPATCH_ACTION, x)
#define EOSIO_MATCH_NOT_DISPATCH_ACTIONnotify EOSIO_MATCH_YES
#define EOSIO_MATCH_NOT_DISPATCH_ACTIONverb EOSIO_MATCH_YES

#define EOSIO_DISPATCH_ACTION_INTERNAL_1(r, type, member)                              \
   case eosio::hash_name(BOOST_PP_STRINGIZE(EOSIO_EXTRACT_ACTION_VERB_NAME(member))):  \
      executed = eosio::execute_action(eosio::name(receiver), eosio::name(code),       \
                                       &type::EOSIO_EXTRACT_ACTION_VERB_NAME(member)); \
      break;
#define EOSIO_DISPATCH_ACTION_INTERNAL(r, type, member)               \
   BOOST_PP_IIF(EOSIO_MATCH_NOT_DISPATCH_ACTION(member), EOSIO_EMPTY, \
                EOSIO_DISPATCH_ACTION_INTERNAL_1)                     \
   (r, type, member)
#define EOSIO_DISPATCH_ACTION(type, MEMBERS) \
   BOOST_PP_SEQ_FOR_EACH(EOSIO_DISPATCH_ACTION_INTERNAL, type, MEMBERS)

#define EOSIO_NOTIFY_ACTION_INTERNAL_1(r, type, notification)                                    \
   else if ((EOSIO_EXTRACT_NOTIFY_CODE(notification) == eosio::any_contract ||                   \
             eosio::name{code} == EOSIO_EXTRACT_NOTIFY_CODE(notification)) &&                    \
            action ==                                                                            \
                eosio::hash_name(BOOST_PP_STRINGIZE(EOSIO_EXTRACT_NOTIFY_ACTION(notification)))) \
   {                                                                                             \
      eosio::execute_action(                                                                     \
          eosio::name(receiver), eosio::name(code),                                              \
          &type::BOOST_PP_CAT(notify_, EOSIO_EXTRACT_NOTIFY_ACTION(notification)));              \
   }
#define EOSIO_DISPATCH_NOTIFY_INTERNAL(r, type, notification)                                  \
   BOOST_PP_IIF(EOSIO_MATCH_NOTIFY(notification), EOSIO_NOTIFY_ACTION_INTERNAL_1, EOSIO_EMPTY) \
   (r, type, notification)
#define EOSIO_DISPATCH_NOTIFY(type, MEMBERS) \
   BOOST_PP_SEQ_FOR_EACH(EOSIO_DISPATCH_NOTIFY_INTERNAL, type, MEMBERS)

#define EOSIO_ACTION_WRAPPER_DECL_1(r, data, action)                                 \
   using EOSIO_EXTRACT_ACTION_VERB_NAME(action) = eosio::action_wrapper<             \
       BOOST_PP_CAT(BOOST_PP_STRINGIZE(EOSIO_EXTRACT_ACTION_VERB_NAME(action)), _h), \
       &contract_class::EOSIO_EXTRACT_ACTION_VERB_NAME(action), contract_account>;
#define EOSIO_ACTION_WRAPPER_DECL_0(r, data, action)
#define EOSIO_ACTION_WRAPPER_DECL(r, data, action)                                      \
   BOOST_PP_CAT(EOSIO_ACTION_WRAPPER_DECL_, BOOST_PP_COMPL(EOSIO_MATCH_NOTIFY(action))) \
   (r, data, action)

#define EOSIO_REFLECT_ACTION_1(r, data, action)                                             \
   f(BOOST_PP_CAT(BOOST_PP_STRINGIZE(EOSIO_EXTRACT_ACTION_VERB_NAME(action)), _h),          \
     eosio::action_type_wrapper<&contract_class::EOSIO_EXTRACT_ACTION_VERB_NAME(action)>{}, \
     EOSIO_GET_RICARDIAN(action) EOSIO_ACTION_ARG_NAME_STRINGS(action));
#define EOSIO_REFLECT_ACTION(r, data, action) \
   BOOST_PP_IIF(EOSIO_MATCH_NOTIFY(action), EOSIO_EMPTY, EOSIO_REFLECT_ACTION_1)(r, data, action)

#define EOSIO_MATCH_VERB(x) EOSIO_MATCH(EOSIO_MATCH_VERB, x)
#define EOSIO_MATCH_VERBverb EOSIO_MATCH_YES
#define EOSIO_MATCH_VERBaction_verb EOSIO_MATCH_YES

#define EOSIO_EXTRACT_VERB_INDEX(x) BOOST_PP_CAT(EOSIO_EXTRACT_VERB_INDEX, x)
#define EOSIO_EXTRACT_VERB_INDEXverb(name, index, ...) index
#define EOSIO_EXTRACT_VERB_INDEXaction_verb(name, index, ...) index

#define EOSIO_DISPATCH_VERB_INTERNAL_1(r, type, member)                              \
   case EOSIO_EXTRACT_VERB_INDEX(member):                                            \
      ::eosio::execute_verb(contract, &type::EOSIO_EXTRACT_ACTION_VERB_NAME(member), \
                            current_session, ds);                                    \
      return true;
#define EOSIO_DISPATCH_VERB_INTERNAL(r, type, member)                                  \
   BOOST_PP_IIF(EOSIO_MATCH_VERB(member), EOSIO_DISPATCH_VERB_INTERNAL_1, EOSIO_EMPTY) \
   (r, type, member)
#define EOSIO_DISPATCH_VERB(type, MEMBERS) \
   BOOST_PP_SEQ_FOR_EACH(EOSIO_DISPATCH_VERB_INTERNAL, type, MEMBERS)

#define EOSIO_FOR_EACH_VERB_INTERNAL_1(r, type, member)                                            \
   f(EOSIO_EXTRACT_VERB_INDEX(member), BOOST_PP_STRINGIZE(EOSIO_EXTRACT_ACTION_VERB_NAME(member)), \
     &type::EOSIO_EXTRACT_ACTION_VERB_NAME(member));
#define EOSIO_FOR_EACH_VERB_INTERNAL(r, type, member)                                  \
   BOOST_PP_IIF(EOSIO_MATCH_VERB(member), EOSIO_FOR_EACH_VERB_INTERNAL_1, EOSIO_EMPTY) \
   (r, type, member)
#define EOSIO_FOR_EACH_VERB(type, MEMBERS) \
   BOOST_PP_SEQ_FOR_EACH(EOSIO_FOR_EACH_VERB_INTERNAL, type, MEMBERS)

#define EOSIO_VERB_INDEX_TO_NAME_INTERNAL_1(r, type, member) \
   case EOSIO_EXTRACT_VERB_INDEX(member):                    \
      return BOOST_PP_CAT(BOOST_PP_STRINGIZE(EOSIO_EXTRACT_ACTION_VERB_NAME(member)), _n);
#define EOSIO_VERB_INDEX_TO_NAME_INTERNAL(r, type, member)                                  \
   BOOST_PP_IIF(EOSIO_MATCH_VERB(member), EOSIO_VERB_INDEX_TO_NAME_INTERNAL_1, EOSIO_EMPTY) \
   (r, type, member)
#define EOSIO_VERB_INDEX_TO_NAME(type, MEMBERS) \
   BOOST_PP_SEQ_FOR_EACH(EOSIO_VERB_INDEX_TO_NAME_INTERNAL, type, MEMBERS)

#define EOSIO_VERB_NAME_TO_INDEX_INTERNAL_1(r, type, member)                                 \
   if (name == BOOST_PP_CAT(BOOST_PP_STRINGIZE(EOSIO_EXTRACT_ACTION_VERB_NAME(member)), _n)) \
      return EOSIO_EXTRACT_VERB_INDEX(member);
#define EOSIO_VERB_NAME_TO_INDEX_INTERNAL(r, type, member)                                  \
   BOOST_PP_IIF(EOSIO_MATCH_VERB(member), EOSIO_VERB_NAME_TO_INDEX_INTERNAL_1, EOSIO_EMPTY) \
   (r, type, member)
#define EOSIO_VERB_NAME_TO_INDEX(type, MEMBERS) \
   BOOST_PP_SEQ_FOR_EACH(EOSIO_VERB_NAME_TO_INDEX_INTERNAL, type, MEMBERS)

#define EOSIO_ACTIONS(CONTRACT_CLASS, CONTRACT_ACCOUNT, ...)                                     \
   namespace actions                                                                             \
   {                                                                                             \
      static constexpr auto contract_account = CONTRACT_ACCOUNT;                                 \
      using contract_class = CONTRACT_CLASS;                                                     \
      BOOST_PP_SEQ_FOR_EACH(EOSIO_ACTION_WRAPPER_DECL, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
                                                                                                 \
      template <typename F>                                                                      \
      void for_each_action(F&& f)                                                                \
      {                                                                                          \
         BOOST_PP_SEQ_FOR_EACH(EOSIO_REFLECT_ACTION, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))   \
      }                                                                                          \
                                                                                                 \
      inline void eosio_apply(uint64_t receiver, uint64_t code, uint64_t action)                 \
      {                                                                                          \
         if (code == receiver)                                                                   \
         {                                                                                       \
            bool executed = false;                                                               \
            switch (action)                                                                      \
            {                                                                                    \
               EOSIO_DISPATCH_ACTION(CONTRACT_CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))      \
            }                                                                                    \
            eosio::check(executed == true, "unknown action");                                    \
         }                                                                                       \
         else                                                                                    \
         {                                                                                       \
            if (false)                                                                           \
            {                                                                                    \
            }                                                                                    \
            EOSIO_DISPATCH_NOTIFY(CONTRACT_CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))         \
         }                                                                                       \
      }                                                                                          \
                                                                                                 \
      inline bool dispatch_verb(eosio::name contract,                                            \
                                uint32_t index,                                                  \
                                const auto& current_session,                                     \
                                eosio::datastream<const char*>& ds)                              \
      {                                                                                          \
         switch (index)                                                                          \
         {                                                                                       \
            EOSIO_DISPATCH_VERB(CONTRACT_CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))           \
         }                                                                                       \
         return false;                                                                           \
      }                                                                                          \
                                                                                                 \
      template <typename F>                                                                      \
      void for_each_verb(F f)                                                                    \
      {                                                                                          \
         EOSIO_FOR_EACH_VERB(CONTRACT_CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))              \
      }                                                                                          \
                                                                                                 \
      inline eosio::name verb_index_to_name(uint32_t index)                                      \
      {                                                                                          \
         switch (index)                                                                          \
         {                                                                                       \
            EOSIO_VERB_INDEX_TO_NAME(CONTRACT_CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))      \
         }                                                                                       \
         return {};                                                                              \
      }                                                                                          \
                                                                                                 \
      inline std::optional<uint32_t> verb_name_to_index(eosio::name name)                        \
      {                                                                                          \
         EOSIO_VERB_NAME_TO_INDEX(CONTRACT_CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))         \
         return {};                                                                              \
      }                                                                                          \
   }

#ifdef COMPILING_CONTRACT
#define EOSIO_ACTION_DISPATCHER(NAMESPACE)                          \
   extern "C"                                                       \
   {                                                                \
      void __wasm_call_ctors();                                     \
      void apply(uint64_t receiver, uint64_t code, uint64_t action) \
      {                                                             \
         __wasm_call_ctors();                                       \
         NAMESPACE ::eosio_apply(receiver, code, action);           \
      }                                                             \
   }
#else
#define EOSIO_ACTION_DISPATCHER(NAMESPACE)
#endif

}  // namespace eosio
