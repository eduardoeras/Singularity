package extractor.transition;

import global.structure.Element;
import global.structure.State;
import global.structure.Transition;
import global.structure.Visibility;

import java.util.ArrayList;
import java.util.List;

public class TransitionWalker {
    //Attributes
    private List<State> main;
    private List<State> functions;

    //Constructor
    public TransitionWalker () {
        main = new ArrayList<>();
        functions = new ArrayList<>();
    }

    //Public Methods
    public void walk (List<State> states, List<Transition> transitions) {
        createMainFunction(states);
        collectFunctions(states);
        for (State state : main) {
            LevelIterator levelIterator = new LevelIterator(state.getScopeLevel());
            levelIterator.iterate(state, states, functions, transitions);
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
