package extractor.transition;

import global.structure.State;
import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class Iterator {
    //Attributes
    private TransitionTools tools;
    private int scopeLevel;
    private State next;

    //Constructor
    public Iterator (State state, int scopeLevel) {
        tools = new TransitionTools();
        this.scopeLevel = scopeLevel;
        next = state;
    }

    //Methods
    public List<Transition> iterate (List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse;
        do {
            newResponse = (process (next, response, states, transitions));
            response = newResponse;
        } while (next.getScopeLevel() > scopeLevel);
        return newResponse;
    }

    private List<Transition> process (State state, List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse = new ArrayList<>();
        switch (state.getElement()) {
            case ATTRIBUTION:
            case STATEMENT:
                createTransition(response, state, transitions);
                String event = tools.extractEvent(state.getLine());
                newResponse.add(tools.createTransition(event, state, null));
                next = tools.getNextState(state, states);
                break;
            case DECISION:
                if (state.getLabel().equals("if")) {
                    createTransition(response, state, transitions);
                    List<Transition> trueResponse = new ArrayList<>();
                    trueResponse.add(tools.createTransition("TRUE", state, null));
                    Iterator iterator = new Iterator(tools.getNextState(state, states), state.getScopeLevel());
                    newResponse.addAll(iterator.iterate(trueResponse, states, transitions));
                    do {
                        next = tools.getNextSameLevelState(next, states);
                    } while (next.getLabel().equals("else") || next.getLabel().equals("else_if"));
                    newResponse.add(tools.createTransition("FALSE", state, null));
                }
                break;
            default:
                next = tools.getNextState(state, states);
                return response;
        }
        return newResponse;
    }

    private void createTransition (List<Transition> response, State state, List<Transition> transitions) {
        for (Transition transition : response) {
            transition.setTo(state);
            transitions.add(transition);
        }
    }

}
