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

    /*public Transition decisionLoop (Transition transition,List<State> states, List<State> functions, List<Transition> transitions) {
        if (decision) {
            State nextTrue = getNextState(transition.getTo(), states);
            LevelIterator levelIterator = new LevelIterator(transition.getTo().getScopeLevel());
            return levelIterator.iterate(
                    createTransition("TRUE", transition.getTo(), nextTrue),
                    states,
                    functions,
                    transitions
            );
        } else {
            State nextFalse = getNextState(transition.getTo(), states);
            while (nextFalse.getScopeLevel() > transition.getTo().getScopeLevel()) {
                nextFalse = getNextState(nextFalse, states);
            }
            return createTransition("FALSE", transition.getTo(), nextFalse);
        }
    }*/

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
