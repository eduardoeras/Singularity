package extractor.transition;

import global.structure.*;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class TransitionTools {
    //Attributes

    //Constructor

    //Public Methods
    public Transition createInitialState () {
        State initial = new State();
        initial.setElement(Element.INITIAL);
        initial.setLabel("initial");
        initial.setId(-1);
        initial.setVisibility(Visibility.NONE);
        initial.setType(Type.NONE);
        initial.setScopeLevel(-1);
        return createTransition("lambda", initial, null);
    }

    public State createFinalState () {
        State finalState = new State();
        finalState.setElement(Element.FINAL);
        finalState.setLabel("final");
        finalState.setId(-1);
        finalState.setVisibility(Visibility.NONE);
        finalState.setType(Type.NONE);
        finalState.setScopeLevel(-1);
        return finalState;
    }

    public State getNextState (State state, List<State> states) {
        try {
            return states.get(states.indexOf(state) + 1);
        } catch (Exception e) {
            return null;
        }
    }

    public Transition createTransition (String eventName, State origin, State destiny) {
        Transition transition = new Transition();
        Event event = new Event();
        event.setEvent(eventName);
        transition.setEvent(event);
        transition.setFrom(origin);
        transition.setTo(destiny);
        return transition;
    }

    public void findFunctionCall(Transition transition, List<State> functions, List<State> states, List<Transition> transitions) {
        for (ParseTree word : transition.getTo().getLine().getContent()) {
            if (isFunction(word, functions)) {
                State destiny = getDestinyFunction(word, functions);
                Transition newTransition = createTransition("lambda", transition.getTo(), destiny);
                LevelIterator levelIterator = new LevelIterator(destiny.getScopeLevel());
                Transition external = levelIterator.iterate(newTransition, states, functions, transitions);
                if (external.getFrom() != transition.getTo()) {
                    transitions.add(createTransition(external.getEvent().getEvent(), external.getFrom(), transition.getTo()));
                }
                break;
            }
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

    public State validateNextState(State state, List<State> states, int scopeLevel) {
        while (state.getId() < 1 && state.getScopeLevel() > scopeLevel) {
            state = getNextState(state, states);
            if (state == null) {
                break;
            }
        }
        return state;
    }

    //Private Methods
    private boolean isFunction (ParseTree word, List<State> functions) {
        for (State function : functions) {
            if (function.getLabel().equals(word.getText())) {
                return true;
            }
        }
        return false;
    }

    private State getDestinyFunction (ParseTree word, List<State> functions) {
        for (State function : functions) {
            if (function.getLabel().equals(word.getText())) {
                return function;
            }
        }
        return null;
    }

}
