//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_FUNCTIONAL_CALLBACK_H_
#define SRC_BASE_FUNCTIONAL_CALLBACK_H_

#include <assert.h>
#include <memory>
#include <tuple>
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {

template <typename Functor, typename... BoundArgs>
decltype(auto) BindOnce(Functor&& functor, BoundArgs&&... args);

class BindStateBase {
 public:
  using InvokeFuncStorage = void (*)();

 private:
  template <typename Functor, typename... BoundArgs>
  friend struct BindState;
  friend class BindStateHolder;

  BindStateBase() = default;
  BindStateBase(const BindStateBase&) = delete;
  BindStateBase(InvokeFuncStorage polymorphic_invoke)
      : polymorphic_invoke_(polymorphic_invoke) {}

  InvokeFuncStorage polymorphic_invoke_;
};

template <typename Functor, typename... BoundArgs>
struct BindState : public BindStateBase {
  static BindState* Create(BindStateBase::InvokeFuncStorage invoke_fun,
                           Functor&& functor,
                           BoundArgs&&... args) {
    return new BindState(invoke_fun, std::forward<Functor>(functor),
                         std::forward<BoundArgs>(args)...);
  }

  Functor functor;
  std::tuple<BoundArgs...> bound_args;

 private:
  BindState() = delete;
  BindState(const BindState&) = delete;
  BindState& operator=(const BindState&) = delete;

  BindState(BindStateBase::InvokeFuncStorage invoke_fun,
            Functor functor,
            BoundArgs&&... args)
      : BindStateBase(invoke_fun), functor(functor), bound_args(args...) {}
};

class BindStateHolder {
 public:
  BindStateHolder() = default;
  BindStateHolder(const BindStateHolder&) = delete;
  BindStateHolder& operator=(const BindStateHolder&) = delete;

  BindStateHolder(BindStateHolder&& holder) noexcept = default;
  BindStateHolder& operator=(BindStateHolder&&) noexcept = default;

  ~BindStateHolder() = default;

  explicit BindStateHolder(BindStateBase* bind_state)
      : bind_state_(bind_state) {}

  bool is_null() const { return bind_state_.get() == nullptr; }

  BindStateBase::InvokeFuncStorage polymorphic_invoke() {
    return bind_state_->polymorphic_invoke_;
  }

  BindStateBase* bind_state() const { return bind_state_.get(); }

 private:
  std::unique_ptr<BindStateBase> bind_state_;
};

template <typename StorageType, typename UnboundRunType>
struct Invoker;

template <typename StorageType, typename R, typename... UnboundArgs>
struct Invoker<StorageType, R(UnboundArgs...)> {
  static R RunOnce(BindStateBase* base, UnboundArgs... args) {
    StorageType* storage = static_cast<StorageType*>(base);

    static constexpr size_t num_bound_args =
        std::tuple_size_v<decltype(storage->bound_args)>;
    return RunImpl(std::move(storage->functor), std::move(storage->bound_args),
                   std::make_index_sequence<num_bound_args>{},
                   std::forward<UnboundArgs>(args)...);
  }

 private:
  template <typename Functor, typename BoundArgsTuple, size_t... indices>
  static R RunImpl(Functor&& functor,
                   BoundArgsTuple&& bound,
                   std::index_sequence<indices...>,
                   UnboundArgs&&... unbound_args) {
    return functor(std::get<indices>(std::forward<BoundArgsTuple>(bound))...,
                   std::forward<UnboundArgs>(unbound_args)...);
  }
};

template <typename T>
class OnceCallback;

template <typename R, typename... Args>
class OnceCallback<R(Args...)> {
 public:
  using PolymorphicInvoke = R (*)(BindStateBase*, Args...);

  OnceCallback() = default;

  OnceCallback(std::nullptr_t) = delete;

  OnceCallback(const OnceCallback&) = delete;
  OnceCallback& operator=(const OnceCallback&) = delete;
  OnceCallback(OnceCallback&&) noexcept = default;
  OnceCallback& operator=(OnceCallback&) noexcept = default;

  OnceCallback(BindStateBase* bind_base) : holder_(bind_base) {}

  R Run(Args... args) && {
    assert(!holder_.is_null());

    BindStateHolder holder = std::move(holder_);

    PolymorphicInvoke f =
        reinterpret_cast<PolymorphicInvoke>(holder.polymorphic_invoke());
    return f(holder.bind_state(), std::forward<Args>(args)...);
  }

 private:
  BindStateHolder holder_;
};

template <typename Functor, typename... BoundArgs>
decltype(auto) BindOnce(Functor&& functor, BoundArgs&&... args) {
  using InvokeFuncStorage = BindStateBase::InvokeFuncStorage;

  using BindState = BindState<Functor, BoundArgs...>;
  using PolymorphicInvoke =
      typename OnceCallback<void(BoundArgs...)>::PolymorphicInvoke;

  using UnboundRunType = void(BoundArgs...);

  using Invoker = Invoker<BindState, UnboundRunType>;
  PolymorphicInvoke invoke_fun = Invoker::RunOnce;

  auto bind_state = BindState::Create(
      reinterpret_cast<InvokeFuncStorage>(invoke_fun),
      std::forward<Functor>(functor), std::forward<BoundArgs>(args)...);

  return OnceCallback<void(BoundArgs...)>(bind_state);
}

}  // namespace _LIB_NAMESPACE::base

#endif