package extractor.transition;

import global.structure.State;
import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class TransitionWalker {
    //Attributes
    private List<State> main;

    //Constructor
    public TransitionWalker () {
        main = new ArrayList<>();
    }

    //Public Methods
    public void walk (List<State> states, List<Transition> transitions) {
        createMainFunction(states);
        for (State state : main) {
            findNext(state, states);
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
                    main.add(state);
                    break;
                default:
                    //do nothing
            }
        }
    }

    private void findNext(State state, List<State> states) {
        System.out.println(states.get(states.indexOf(state)).getLabel());
        switch (state.getElement()) {
            case FUNCTION:
            case OPERATOR:
            case CONSTRUCTOR:
            case DESTRUCTOR:
                try {
                    findNext(states.get(states.indexOf(state) + 1), states);
                } catch (Exception e){
                    System.out.println("NOTHING NEXT");
                }
        }
    }

}
