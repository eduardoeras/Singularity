package extractor.transition;

import global.structure.State;
import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class Iterator {
    //Attributes
    private TransitionTools tools;
    private int scopeLevel;
    //Constructor
    public Iterator (int scopeLevel) {
        tools = new TransitionTools();
        this.scopeLevel = scopeLevel;
    }

    //Methods
    public List<Transition> iterate (State state, List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse;
        do {
            newResponse = (process (state, response, states, transitions));
            response = newResponse;
            state = tools.getNextState(state, states);
        } while (state.getScopeLevel() > scopeLevel);
        return newResponse;
    }

    private List<Transition> process (State state, List<Transition> response, List<State> states, List<Transition> transitions) {
        List<Transition> newResponse = new ArrayList<>();
        switch (state.getElement()) {
            case ATTRIBUTION:
            case STATEMENT:
                for (Transition transition : response) {
                    transition.setTo(state);
                    transitions.add(transition);
                }
                Transition newTransition = tools.createTransition("lambda", state, null);
                newResponse.add(newTransition);
                break;
            default:
                return response;
        }
        return newResponse;
    }
}
