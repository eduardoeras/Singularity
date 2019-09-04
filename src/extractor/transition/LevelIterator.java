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
                process(state, functions, transitions);
                iterate(next,states, functions, transitions);
            }
        } catch (Exception e) {
            System.out.println("NOTHING NEXT");
        }
    }

    //Private Methods
    private void process(State state, List<State> functions, List<Transition> transitions) {
        if (state.getType() == Type.STATE) {
            switch (state.getElement()) {
                case JUMP:
                    System.out.println("JUMP: " + state.getLabel());
                    findFunctionCall(state, functions, transitions);
                    break;
                case ATTRIBUTION:
                    System.out.println("ATTRIBUTION: " + state.getLabel());
                    findFunctionCall(state, functions, transitions);
                    break;
                case STATEMENT:
                    System.out.println("STATEMENT: " + state.getLabel());
                    findFunctionCall(state, functions, transitions);
                    break;
                default:
            }
        } else {
            switch (state.getElement()) {
                case LOOP:
                    System.out.println("LOOP: " + state.getLabel());
                    findFunctionCall(state, functions, transitions);
                    break;
                case DECISION:
                    System.out.println("DECISION: " + state.getLabel());
                    findFunctionCall(state, functions, transitions);
                    break;
                case EXCEPTION:
                    System.out.println("EXCEPTION: " + state.getLabel());
                    findFunctionCall(state, functions, transitions);
                    break;
                case NAMESPACE:
                    System.out.println("NAMESPACE: " + state.getLabel());
                    break;
                case CLASS:
                    System.out.println("CLASS: " + state.getLabel());
                    break;
                case STRUCT:
                    System.out.println("STRUCT: " + state.getLabel());
                    break;
                case FUNCTION:
                    System.out.println("FUNCTION: " + state.getLabel());
                    break;
                case CONSTRUCTOR:
                    System.out.println("CONSTRUCTOR: " + state.getLabel());
                    break;
                case DESTRUCTOR:
                    System.out.println("DESTRUCTOR: " + state.getLabel());
                    break;
                case OPERATOR:
                    System.out.println("OPERATOR: " + state.getLabel());
                    break;
                default:
            }
        }
    }

    private void findFunctionCall(State state, List<State> functions, List<Transition> transitions) {
        for (ParseTree word : state.getLine().getContent()) {
            if (isFunction(word, functions)) {
                System.out.println("### FUNCTION CALL: " + word.getText());
                Transition transition = new Transition();
                Event event = new Event();
                event.setEvent("lambda");
                transition.setEvent(event);
                transition.setFrom(state);
                transition.setTo(getDestinyFunction(word, functions));
                transitions.add(transition);
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
}
