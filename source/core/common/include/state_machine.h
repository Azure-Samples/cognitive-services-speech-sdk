//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// state_machine.h: Generic state machine class definition.
//
#pragma once

#include <functional>
#include <map>
#include <set>
#include <type_traits>
#include <utility>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/**
 * Template class that can be used to model a finite state machine.
 * Conceptually, is composed of nodes (which represent the states) and transitions (which are the edges).
 * Each node has zero or more transitions to other nodes, this transitions encapsulate the side effect of moving from state A to B.
 * e.g:
 * auto sm =
 * {
 *     {
 *          State::A, // Current node
 *          {
 *              {
 *                  State::B, // Transiton to B
 *                  []() { std::cout << "Moving from A to B" << std::endl; } // Side effect
 *              }
 *          }
 *      },
 *      {
 *          State::B,
 *          {
 *              {
 *                  State::A,
 *                  []() { std::cout << "Moving from B to A" << std::endl; }
 *              },
 *              {
 *                  State::B,
 *                  []() { std::cout << "Looping in B" << std::endl; }
 *              }
 *          }
 *      }
 *  }
 *
 */
template<typename S, S initial_state, typename transition_fn = std::function<void()>, typename = std::enable_if_t<std::is_enum<S>::value>>
struct state_machine
{
    struct state_transition
    {
        state_transition(S state, transition_fn transition_action) : state{ state }, transition_action{ transition_action }
        {}

        S state;
        transition_fn transition_action;

        bool operator<(const state_transition& rhs) const
        {
            return state < rhs.state;
        }
    };

    struct state_node
    {
        state_node(S state, std::set<state_transition> transitions) : state{ state }
        {
            for (auto t : transitions)
            {
                state_transitions.insert(std::make_pair( t.state, t ));
            }
        }

        S state;
        std::map<S, state_transition> state_transitions;
    };

    state_machine(std::initializer_list<state_node> states) : current_state(initial_state)
    {
        for (auto state : states)
        {
            nodes.insert(std::make_pair( state.state, state ));
        }
    }

    state_machine() : current_state(initial_state), nodes()
    {}

    state_machine(const state_machine&) = delete;

    state_machine(state_machine&& other)
    {
        current_state = other.current_state;
        nodes = std::move(other.nodes);
    }

    void operator=(state_machine&& other)
    {
        current_state = other.current_state;
        nodes = std::move(other.nodes);
    }

    S current_state;
    std::map<S, state_node> nodes;

    template<typename... Args>
    void transition(const S new_state, Args... args)
    {
        auto& current_node = nodes.at(current_state);
        try
        {
            const auto& t = current_node.state_transitions.at(new_state);
            t.transition_action(args...);
            current_state = new_state;
        }
        catch (std::out_of_range&)
        {
            throw std::runtime_error("Illegal transition");
        }

    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
