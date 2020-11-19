
#include "lsystem.h"
#include <stack>
#include <memory>
#include <iostream>

std::string LindenmayerSystemDeterministic::expandSymbol(unsigned char const& sym) {
    /*============================================================
        TODO 1.1
        For a given symbol in the sequence, what should it be replaced with after expansion?

        You may find useful:
            map.find: Iterator to an element with key equivalent to key. If no such element is found, past-the-end (see end()) iterator is returned.
            http://en.cppreference.com/w/cpp/container/unordered_map/find
    */

    auto it = rules.find(sym);
    if(it == rules.end()) return {char(sym)}; // this constructs string from char
    return it->second;
    
}

std::string LindenmayerSystem::expandOnce(std::string const& symbol_sequence) {
    /*============================================================
        TODO 1.2
        Perform one iteration of grammar expansion on `symbol_sequence`.
        Use the expandSymbol method
    */
    
    std::string new_sequence;
    for(size_t i = 0; symbol_sequence[i] != '\0'; ++i)
    {
        new_sequence += expandSymbol(symbol_sequence[i]);
    }

    return new_sequence;

}

std::string LindenmayerSystem::expand(std::string const& initial, uint32_t num_iters) {
    /*============================================================
        TODO 1.3
        Perform `num_iters` iterations of grammar expansion (use expandOnce)
    */

    std::string sequence = initial;
    for(size_t i = 0; i<num_iters;++i)
    {
        sequence = expandOnce(sequence);
    }

    return sequence;
}

std::vector<Segment> LindenmayerSystem::draw(std::string const& symbols) {
    std::vector<Segment> lines; // this is already initialized as empty vector
    /*============================================================
        TODO 2
        Build line segments according to the sequence of symbols
        The initial position is (0, 0) and the initial direction is "up" (0, 1)
        Segment is std::pair<vec2, vec2>

        you can create a Segment as {p1, p2}, where p1 and p2 are vec2, e.g.:
        lines.push_back({p1, p2});

        You may also find std::stack useful:

        https://en.cppreference.com/w/cpp/container/stack

        There also is a mat2 class in utils/vec.* you may find useful for
        implementing rotations.
    */

    std::stack<std::pair<vec2,vec2>> saved_states;

    vec2 current_pos = vec2(0,0);
    vec2 new_pos;
    vec2 current_dir = vec2(0,1);

    double cos_angle = cos(rotation_angle_deg*M_PI/180.0);
    double sin_angle = sin(rotation_angle_deg*M_PI/180.0);

    mat2 rotation = mat2(cos_angle, -sin_angle, sin_angle, cos_angle);
    mat2 m_rotation = transpose(rotation);
    std::pair<vec2,vec2> saved_state;

    for(size_t i = 0; symbols[i] != '\0'; ++i)
    {
        switch (symbols[i]) {
            case '+':
                current_dir = rotation * current_dir;
                break;
            case '-':
                current_dir = m_rotation * current_dir;
                break;
            case '[':
                saved_states.push(std::make_pair(current_pos,current_dir));
                break;
            case ']':
                saved_state = saved_states.top();
                saved_states.pop();
                current_pos = saved_state.first;
                current_dir = saved_state.second;
                break;
            default:
                new_pos = current_pos + current_dir;
                lines.push_back(Segment(current_pos,new_pos));
                current_pos = new_pos;
                break;
        }
    }

    //============================================================
    return lines;
}

std::string LindenmayerSystemStochastic::expandSymbol(unsigned char const& sym) {
    /*============================================================
        TODO 4
        For a given symbol in the sequence, what should it be replaced with after expansion?
        (stochastic case)

        Use dice.roll() to get a random number between 0 and 1
    */
    
    std::vector<StochasticRule> rulevec;
    auto rnd = dice.roll();
    auto it = rules.find(sym);
    double cdf = 0;
    size_t i = 0;
    if(it == rules.end()) return {char(sym)}; // this constructs string from char

    rulevec = it->second;
    cdf += rulevec[0].probability;

    while(cdf < rnd)
    {
        ++i;
        cdf += rulevec[i].probability;
    }
    return rulevec[i].expansion;

}

void LindenmayerSystemDeterministic::addRuleDeterministic(unsigned char sym, std::string const& expansion) {
    rules[sym] = expansion;
}

void LindenmayerSystemStochastic::addRuleStochastic(unsigned char sym, std::vector<StochasticRule> expansions_with_ps) {
    rules[sym] = expansions_with_ps;
}
