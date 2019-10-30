package extractor.transition;

import global.structure.*;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class TransitionTools {
    //Attributes
    private StringTools stringTools;

    //Constructor
    public TransitionTools () {
        stringTools = new StringTools();
    }

    //Public Methods
    public Transition createInitialState () {
        State initial = new State();
        initial.setElement(Element.INITIAL);
        initial.setLabel("initial");
        initial.setId(-1);
        initial.setVisibility(Visibility.NONE);
        initial.setType(Type.NONE);
        initial.setScopeLevel(-1);
        initial.setLine(new Line());
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
        finalState.setLine(new Line());
        return finalState;
    }

    public State getNextState (State state, List<State> states) {
        try {
            return states.get(states.indexOf(state) + 1);
        } catch (Exception e) {
            return null;
        }
    }

    public State getNextSameLevelState (State state, List<State> states) {
        State output = state;
        try {
            do {
                output = states.get(states.indexOf(output) + 1);
                if (output.getScopeLevel() < state.getScopeLevel()) {
                    break;
                }
            } while (output.getScopeLevel() != state.getScopeLevel() || output.getLabel().equals("}"));
            return output;
        } catch (Exception e) {
            return null;
        }
    }

    public State getNextSameLevelDecision (State state, List<State> states) {
        State output = state;
        try {
            do {
                output = states.get(states.indexOf(output) + 1);
                if (output.getScopeLevel() < state.getScopeLevel()) {
                    break;
                }
            } while (output.getScopeLevel() != state.getScopeLevel() || output.getElement() != Element.DECISION);
            return output;
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

    public String extractEvent(Line line) {
        boolean flag = false;
        for (ParseTree element : line.getContent()) {
            if (element.getText().equals("cout")) {
                flag = true;
                continue;
            }
            if (flag && !element.getText().equals("<<")) {
                return stringTools.noSpecialCharacters(element.getText());
            }
        }
        return "lambda";
    }

    public void finalizeTransition(List<Transition> response, State destiny, List<Transition> transitions) {
        for (Transition transition : response) {
            transition.setTo(destiny);
            if (!exists(transition, transitions)) {
                transitions.add(transition);
            }
        }
    }

    //Private Methods
    private boolean exists(Transition neo, List<Transition> transitions) {
        for (Transition saved : transitions) {
            if (neo.isEqual(saved)) {
                return true;
            }
        }
        return false;
    }

}
