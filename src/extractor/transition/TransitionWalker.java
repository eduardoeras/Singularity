package extractor.transition;

import global.structure.*;

import java.util.ArrayList;
import java.util.List;

public class TransitionWalker {
    //Attributes
    private List<State> main;
    private List<State> functions;
    private TransitionTools tools;
    private Transition last;

    //Constructor
    public TransitionWalker () {
        main = new ArrayList<>();
        functions = new ArrayList<>();
        tools = new TransitionTools();
        last = tools.createInitialState();
    }

    //Public Methods
    public void walk (List<State> states, List<Transition> transitions) {
        createMainFunction(states);
        collectFunctions(states);
        for (State destiny : main) {
            Transition transition = tools.createTransition(last.getEvent().getEvent(), last.getFrom(), destiny);
            LevelIterator levelIterator = new LevelIterator(destiny.getScopeLevel());
            last = levelIterator.iterate(transition, states, functions, transitions);
        }
        main.clear();
    }

    //Private Methods
    private void createMainFunction (List<State> states) {
        for (State state : states) {
            switch (state.getElement()) {
                case CONSTRUCTOR:
                case FUNCTION:
                case OPERATOR:
                    if (state.getVisibility() != Visibility.PRIVATE) {
                        main.add(state);
                    }
                    break;
            }
        }
    }

    private void collectFunctions(List<State> states) {
        for (State state : states) {
            if (state.getElement() == Element.FUNCTION) {
                functions.add(state);
            }
        }
    }

}
