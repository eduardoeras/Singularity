package extractor.transition;

import global.structure.State;
import global.structure.Transition;
import global.structure.Type;

import java.util.List;

public class LevelIterator {
    //Attributes
    private int scopeLevel;
    private TransitionTools tools;

    //Constructor
    public LevelIterator (int scopeLevel) {
        this.scopeLevel = scopeLevel;
        tools = new TransitionTools();
    }

    //Public Methods
    public Transition iterate(Transition transition, List<State> states, List<State> functions, List<Transition> transitions) {
        do {
            transition = process(transition, functions, states, transitions);
        } while (transition.getTo() != null && transition.getTo().getScopeLevel() > scopeLevel);
        return transition;
    }

    //Private Methods
    private Transition process(Transition transition, List<State> functions, List<State> states, List<Transition> transitions) {
        switch (transition.getTo().getElement()) {
            case ATTRIBUTION:
            case STATEMENT:
                transitions.add(transition);
                tools.findFunctionCall(transition, functions, states, transitions);
                State next = tools.getNextState(transition.getTo(), states);
                return tools.createTransition("lambda",transition.getTo(),next);
            case DECISION:
                transitions.add(transition);
                tools.findFunctionCall(transition, functions, states, transitions);

                State nextTrue = tools.getNextState(transition.getTo(), states);
                if (nextTrue.getScopeLevel() > transition.getTo().getScopeLevel()) {
                    LevelIterator newLevel = new LevelIterator(transition.getTo().getScopeLevel());
                    newLevel.iterate(tools.createTransition("TRUE", transition.getTo(), nextTrue), states, functions, transitions);
                }
                State nextFalse = tools.getNextState(transition.getTo(), states);
                while (nextFalse.getScopeLevel() > transition.getTo().getScopeLevel()) {
                    nextFalse = tools.getNextState(nextFalse, states);
                }
                return tools.createTransition("FALSE", transition.getTo(), nextFalse);
            default:
                State nextState = tools.getNextState(transition.getTo(), states);
                return tools.createTransition(transition.getEvent().getEvent(),transition.getFrom(),nextState);
        }
    }

    /*public void decisionLoopTransition (State state, List<State> states, List<Transition> transitions, int scopeLevel) {
        State nextTrue = getNextState(state, states);
        nextTrue = validateNextState(nextTrue, states, scopeLevel);
        createTransition("TRUE", state, nextTrue, transitions);
        State nextFalse = getNextState(state, states);
        while (nextFalse.getScopeLevel() > state.getScopeLevel()) {
            nextFalse = getNextState(nextFalse, states);
        }
        nextFalse = validateNextState(nextFalse, states, scopeLevel);
        createTransition("FALSE", state, nextFalse, transitions);
    }*/

}

