package extractor.transition;

import global.structure.Event;
import global.structure.State;
import global.structure.Transition;
import global.structure.Type;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class LevelIterator {
    //Attributes
    private int scopeLevel;

    //Constructor
    public LevelIterator (int scopeLevel) {
        this.scopeLevel = scopeLevel;
    }

    //Public Methods
    public void iterate(State state, List<State> states, List<State> functions, List<Transition> transitions) {
        try {
            State next = states.get(states.indexOf(state) + 1);
            if (next.getScopeLevel() > scopeLevel) {
                process(state, functions, states, transitions);
                iterate(next,states, functions, transitions);
            }
        } catch (Exception e) {
            System.out.println("NOTHING NEXT");
        }
    }

    //Private Methods
    private void process(State state, List<State> functions, List<State> states, List<Transition> transitions) {
        if (state.getType() == Type.STATE) {
            switch (state.getElement()) {
                case JUMP:
                    findFunctionCall(state, functions, states, transitions);
                    break;
                case ATTRIBUTION:
                    findFunctionCall(state, functions, states, transitions);
                    break;
                case STATEMENT:
                    findFunctionCall(state, functions, states, transitions);
                    break;
                default:
            }
        } else {
            switch (state.getElement()) {
                case LOOP:
                    findFunctionCall(state, functions, states, transitions);
                    switch (state.getLabel()) {
                        case "for":
                            decisionLoopTransition(state, states, transitions);
                            break;
                        case "do":
                            //System.out.println("D O   L O O P");
                            break;
                        case "while":
                            //System.out.println("W H I L E   L O O P");
                            break;
                        case "switch":
                            //System.out.println("S W I T C H   L O O P");
                            break;
                    }
                    break;
                case DECISION:
                    findFunctionCall(state, functions, states, transitions);
                    decisionLoopTransition(state, states, transitions);
                    break;
                case EXCEPTION:
                    findFunctionCall(state, functions, states, transitions);
                    break;
                case NAMESPACE:
                    break;
                case CLASS:
                    break;
                case STRUCT:
                    break;
                case FUNCTION:
                    break;
                case CONSTRUCTOR:
                    break;
                case DESTRUCTOR:
                    break;
                case OPERATOR:
                    break;
                default:
            }
        }
    }

    private void findFunctionCall(State state, List<State> functions, List<State> states, List<Transition> transitions) {
        for (ParseTree word : state.getLine().getContent()) {
            if (isFunction(word, functions)) {
                State destiny = getDestinyFunction(word, functions);
                createTransition("lambda", state, destiny, transitions);
                LevelIterator levelIterator = new LevelIterator(destiny.getScopeLevel());
                levelIterator.iterate(destiny, states, functions, transitions);
                break;
            }
        }
    }

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

    private void createTransition (String eventName, State origin, State destiny, List<Transition> transitions) {
        if (origin != null && destiny != null) {
            Transition transition = new Transition();
            Event event = new Event();
            event.setEvent(eventName);
            transition.setEvent(event);
            transition.setFrom(origin);
            transition.setTo(destiny);
            transitions.add(transition);
        }
    }

    private State getNextState (State state, List<State> states) {
        try {
            return states.get(states.indexOf(state) + 1);
        } catch (Exception e) {
            return null;
        }
    }

    private State validateNextState(State state, List<State> states) {
        while (state.getId() < 1) {
            state = getNextState(state, states);
            if (state == null) {
                break;
            }
        }
        return state;
    }

    private void decisionLoopTransition (State state, List<State> states, List<Transition> transitions) {
        State nextTrue = getNextState(state, states);
        nextTrue = validateNextState(nextTrue, states);
        createTransition("TRUE", state, nextTrue, transitions);
        State nextFalse = getNextState(state, states);
        while (nextFalse.getScopeLevel() > state.getScopeLevel()) {
            nextFalse = getNextState(nextFalse, states);
        }
        nextFalse = validateNextState(nextFalse, states);
        createTransition("FALSE", state, nextFalse, transitions);
    }
}
