package extractor.transition;

import global.structure.State;
import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class Iterator {
    //Attributes
    private TransitionTools tools;
    private int scopeLevel;
    private State state;

    //Constructor
    public Iterator (State state, int scopeLevel) {
        tools = new TransitionTools();
        this.scopeLevel = scopeLevel;
        this.state = state;
    }

    //Methods
    public List<Transition> iterate (List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse;
        do {
            newResponse = (process (response, states, transitions));
            response = newResponse;
        } while (state.getScopeLevel() > scopeLevel);
        return newResponse;
    }

    private List<Transition> process (List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse = new ArrayList<>();
        switch (state.getElement()) {
            case ATTRIBUTION:
            case STATEMENT:
                createTransition(response, state, transitions);
                String event = tools.extractEvent(state.getLine());
                newResponse.add(tools.createTransition(event, state, null));
                state = tools.getNextState(state, states);
                break;
            case DECISION:
                createTransition(response, state, transitions);
                if (state.getLabel().equals("if")) {
                    while (true) {
                        List<Transition> falseResponse = new ArrayList<>();
                        falseResponse.add(tools.createTransition("FALSE", state, null));
                        newResponse.addAll(iterateLoop(states, transitions));
                        state = tools.getNextSameLevelState(state, states);
                        if (!state.getLabel().equals("else_if") && !state.getLabel().equals("else")) {
                            newResponse.addAll(falseResponse);
                            break;
                        } else {
                            createTransition(falseResponse, state, transitions);
                        }
                    }
                }
                break;
            default:
                state = tools.getNextState(state, states);
                return response;
        }
        return newResponse;
    }

    private List<Transition> iterateLoop (List<State> states, List<Transition> transitions) {
        List<Transition> trueResponse = new ArrayList<>();
        trueResponse.add(tools.createTransition("TRUE", state, null));
        Iterator iterator = new Iterator(tools.getNextState(state, states), state.getScopeLevel());
        return (iterator.iterate(trueResponse, states, transitions));
    }

    private void createTransition (List<Transition> response, State state, List<Transition> transitions) {
        for (Transition transition : response) {
            transition.setTo(state);
            transitions.add(transition);
        }
    }

}
